#include "builder.h"
#include "nulib/math.h"

#include <vmcore/config.h>
#include <cJSON/cJSON.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_resize2.h>

static nu_byte_t *
load_bytes (nu_str_t filename, nu_size_t *size)
{
    char buf[256]; // TODO: support PATH_MAX
    nu_str_to_cstr(filename, buf, sizeof(buf));
    FILE *f = fopen(buf, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", buf);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)malloc(fsize);
    fread(bytes, fsize, 1, f);
    fclose(f);
    if (size)
    {
        *size = fsize;
    }
    return bytes;
}

#define MAX_NAME_SIZE 256

typedef struct
{
    nux_chunk_header_t header;
    nu_char_t          source[MAX_NAME_SIZE];
} chunk_entry_t;

static nu_f32_t
parse_f32 (cJSON *object, const nu_char_t *name)
{
    NU_ASSERT(object && name && cJSON_IsObject(object));
    cJSON *f = cJSON_GetObjectItem(object, name);
    NU_ASSERT(f && cJSON_IsNumber(f));
    return cJSON_GetNumberValue(f);
}

static void
parse_target (cJSON *j, nux_chunk_type_t type, nux_chunk_target_t *target)
{
    switch (type)
    {
        case NUX_CHUNK_RAW: {
            NU_ASSERT(cJSON_IsObject(j));
            target->raw.addr = parse_f32(j, "addr");
        }
        break;
        case NUX_CHUNK_WASM:
            break;
        case NUX_CHUNK_TEXTURE: {
            NU_ASSERT(cJSON_IsObject(j));
            target->texture.slot = parse_f32(j, "slot");
            target->texture.x    = parse_f32(j, "x");
            target->texture.y    = parse_f32(j, "y");
            target->texture.w    = parse_f32(j, "w");
            target->texture.h    = parse_f32(j, "h");
        }
        break;
        case NUX_CHUNK_MESH: {
            NU_ASSERT(cJSON_IsObject(j));
            target->mesh.first = parse_f32(j, "first");
            target->mesh.count = parse_f32(j, "count");
        }
        break;
    }
}
static chunk_entry_t *
parse_json (nu_str_t path, char target[MAX_NAME_SIZE], nu_size_t *entry_count)
{
    // load file
    nu_byte_t *jfile = load_bytes(path, NU_NULL);
    NU_ASSERT(jfile);
    cJSON *root = cJSON_Parse((const char *)jfile);
    NU_ASSERT(root);
    NU_ASSERT(cJSON_IsObject(root));

    // load target
    cJSON *jcart = cJSON_GetObjectItem(root, "target");
    NU_ASSERT(jcart);
    NU_ASSERT(cJSON_IsString(jcart));
    nu_str_to_cstr(
        nu_str_from_cstr(cJSON_GetStringValue(jcart)), target, MAX_NAME_SIZE);

    // parse entries
    cJSON *jentries = cJSON_GetObjectItem(root, "chunks");
    NU_ASSERT(cJSON_IsArray(jentries));
    *entry_count = cJSON_GetArraySize(jentries);
    NU_ASSERT(*entry_count);
    chunk_entry_t *entries = malloc(sizeof(chunk_entry_t) * (*entry_count));
    NU_ASSERT(entries);
    cJSON *jchild = jentries->child;
    for (nu_size_t i = 0; i < *entry_count; ++i)
    {
        // check fields
        cJSON *jtype = cJSON_GetObjectItem(jchild, "type");
        NU_ASSERT(jtype);
        NU_ASSERT(cJSON_IsString(jtype));
        cJSON *jtarget = cJSON_GetObjectItem(jchild, "target");
        cJSON *jsource = cJSON_GetObjectItem(jchild, "source");
        NU_ASSERT(jsource);
        NU_ASSERT(cJSON_IsString(jsource));

        // check type
        nu_str_t type_str = nu_str_from_cstr(cJSON_GetStringValue(jtype));
        nux_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
        const struct
        {
            nu_str_t         name;
            nux_chunk_type_t type;
        } name_to_chunk_type[] = {
            { NU_STR("raw"), NUX_CHUNK_RAW },
            { NU_STR("wasm"), NUX_CHUNK_WASM },
            { NU_STR("texture"), NUX_CHUNK_TEXTURE },
            { NU_STR("mesh"), NUX_CHUNK_MESH },
        };
        for (nu_size_t j = 0; j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
             ++j)
        {
            if (nu_str_eq(name_to_chunk_type[j].name, type_str))
            {
                type  = name_to_chunk_type[j].type;
                found = NU_TRUE;
            }
        }
        if (!found)
        {
            printf("Chunk type not found for entry %lu\n", i);
            return NU_NULL;
        }
        entries[i].header.type = type;
        nu_str_to_cstr(nu_str_from_cstr(cJSON_GetStringValue(jsource)),
                       entries[i].source,
                       MAX_NAME_SIZE);

        // parse target object
        parse_target(jtarget, type, &entries[i].header.target);

        jchild = jchild->next;
    }
    cJSON_Delete(root);
    free(jfile);
    return entries;
}

static void
write_u32 (FILE *f, nu_u32_t v)
{
    nu_u32_t a = nu_u32_le(v);
    NU_ASSERT(fwrite(&a, sizeof(a), 1, f));
}
static void
write_v4u (FILE *f, nu_v4u_t v)
{
    for (nu_size_t i = 0; i < NU_V4_SIZE; ++i)
    {
        nu_u32_t a = nu_u32_le(v.data[i]);
        NU_ASSERT(fwrite(&a, sizeof(a), 1, f));
    }
}
static void
write_chunk_header (FILE *f, nux_chunk_header_t *header)
{
    // type / length
    write_u32(f, header->type);
    write_u32(f, header->length);
    // target
    switch (header->type)
    {
        case NUX_CHUNK_RAW: {
        }
        break;
        case NUX_CHUNK_WASM: {
        }
        break;
        case NUX_CHUNK_TEXTURE: {
            write_u32(f, header->target.texture.slot);
            write_u32(f, header->target.texture.x);
            write_u32(f, header->target.texture.y);
            write_u32(f, header->target.texture.w);
            write_u32(f, header->target.texture.h);
        }
        break;
        case NUX_CHUNK_MESH: {
        }
        break;
    }
}
void
nux_build_cart (nu_str_t path)
{
    // Parse json entries
    nu_char_t      target[MAX_NAME_SIZE];
    nu_size_t      entry_count;
    chunk_entry_t *entries = NU_NULL;
    entries                = parse_json(path, target, &entry_count);
    NU_ASSERT(entries);

    // Open cart
    FILE *f = fopen((char *)target, "wb");
    if (!f)
    {
        printf("Failed to open file\n");
        goto cleanup0;
    }

    // Write header
    const nu_u32_t version = 100;
    NU_ASSERT(fwrite(&version, sizeof(version), 1, f));
    const nu_u32_t chunk_count = entry_count;
    NU_ASSERT(fwrite(&chunk_count, sizeof(chunk_count), 1, f));

    // Compile entries
    for (nu_size_t i = 0; i < entry_count; ++i)
    {
        chunk_entry_t *entry = entries + i;
        switch (entry->header.type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM: {
                nu_size_t  size;
                nu_byte_t *buffer
                    = load_bytes(nu_str_from_cstr(entry->source), &size);
                NU_ASSERT(buffer);
                // header
                entry->header.length = size;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(buffer, size, 1, f));
                free(buffer);
            }
            break;
            case NUX_CHUNK_TEXTURE: {
                int        w, h, n;
                nu_byte_t  fn[256];
                nu_byte_t *img = stbi_load(
                    (char *)entry->source, &w, &h, &n, STBI_default);
                NU_ASSERT(img);
                nu_v2u_t  target_size = nu_v2u(128, 128);
                nu_u32_t  target_comp = 4;
                nu_size_t length      = sizeof(nu_byte_t) * target_size.x
                                   * target_size.y * target_comp;
                nu_byte_t *output = malloc(length);
                NU_ASSERT(output);
                NU_ASSERT(stbir_resize_uint8_linear(img,
                                                    w,
                                                    h,
                                                    w * n,
                                                    output,
                                                    target_size.x,
                                                    target_size.y,
                                                    target_size.x * target_comp,
                                                    STBIR_RGBA));

                // header
                entry->header.length = length;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(output, length, 1, f));

                free(output);
                stbi_image_free(img);
            }
            break;
            case NUX_CHUNK_MESH: {
                // // header
                // write_chunk_header(f, entry, length);
                // // destination
                // write_u32(f, entry->dst.mesh.first);
                // // data
                // NU_ASSERT(fwrite(output, length, 1, f));
            }
            break;
        }
    }

    // Free resources
    fclose(f);
cleanup0:
    free(entries);
}
