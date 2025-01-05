#include "builder.h"

#include <vmcore/config.h>
#include <cJSON/cJSON.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_resize2.h>

static nu_byte_t *
load_bytes (const nu_byte_t *filename, nu_size_t *size)
{
    FILE *f = fopen((char *)filename, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", filename);
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
    nux_chunk_type_t type;
    nu_byte_t        src[MAX_NAME_SIZE];
    union
    {
        struct
        {
            nu_u32_t slot;
            nu_v4u_t area;
        } texture;
        struct
        {
            nu_u32_t first;
        } mesh;
    } dst;
} project_entry_t;

static nu_bool_t
parse_area (cJSON *object, nu_v4u_t *area)
{
    NU_ASSERT(cJSON_IsObject(object));
    cJSON *jx = cJSON_GetObjectItem(object, "x");
    NU_ASSERT(jx && cJSON_IsNumber(jx));
    cJSON *jy = cJSON_GetObjectItem(object, "y");
    NU_ASSERT(jy && cJSON_IsNumber(jy));
    cJSON *jw = cJSON_GetObjectItem(object, "w");
    NU_ASSERT(jw && cJSON_IsNumber(jw));
    cJSON *jh = cJSON_GetObjectItem(object, "h");
    NU_ASSERT(jh && cJSON_IsNumber(jh));
    area->x = cJSON_GetNumberValue(jx);
    area->y = cJSON_GetNumberValue(jy);
    area->w = cJSON_GetNumberValue(jw);
    area->z = cJSON_GetNumberValue(jh);
    return NU_TRUE;
}

static project_entry_t *
parse_json (const nu_byte_t *path,
            nu_byte_t        target[MAX_NAME_SIZE],
            nu_size_t       *entry_count)
{
    // load file
    nu_byte_t *jfile = load_bytes(path, NU_NULL);
    NU_ASSERT(jfile);
    cJSON *root = cJSON_Parse((const char *)jfile);
    NU_ASSERT(root);
    NU_ASSERT(cJSON_IsObject(root));

    // load target
    cJSON *jtarget = cJSON_GetObjectItem(root, "target");
    NU_ASSERT(jtarget);
    NU_ASSERT(cJSON_IsString(jtarget));
    nu_str_to_cstr(nu_str_from_cstr((nu_byte_t *)cJSON_GetStringValue(jtarget)),
                   target,
                   MAX_NAME_SIZE);

    // parse entries
    cJSON *jentries = cJSON_GetObjectItem(root, "chunks");
    NU_ASSERT(cJSON_IsArray(jentries));
    *entry_count = cJSON_GetArraySize(jentries);
    NU_ASSERT(*entry_count);
    project_entry_t *entries = malloc(sizeof(project_entry_t) * (*entry_count));
    NU_ASSERT(entries);
    cJSON *jchild = jentries->child;
    for (nu_size_t i = 0; i < *entry_count; ++i)
    {
        // check fields
        cJSON *jtype = cJSON_GetObjectItem(jchild, "type");
        NU_ASSERT(jtype);
        NU_ASSERT(cJSON_IsString(jtype));
        cJSON *jsrc = cJSON_GetObjectItem(jchild, "src");
        NU_ASSERT(jsrc);
        NU_ASSERT(cJSON_IsString(jsrc));

        // check type
        nu_str_t type_str
            = nu_str_from_cstr((nu_byte_t *)cJSON_GetStringValue(jtype));
        nux_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
        const struct
        {
            nu_str_t         name;
            nux_chunk_type_t type;
        } name_to_chunk_type[] = {
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
        entries[i].type = type;
        nu_str_to_cstr(
            nu_str_from_cstr((nu_byte_t *)cJSON_GetStringValue(jsrc)),
            entries[i].src,
            MAX_NAME_SIZE);

        // parse dst object
        switch (type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM:
                break;
            case NUX_CHUNK_TEXTURE: {
                cJSON *jdst = cJSON_GetObjectItem(jchild, "dst");
                NU_ASSERT(jdst);
                NU_ASSERT(cJSON_IsObject(jdst));
                cJSON *jarea = cJSON_GetObjectItem(jdst, "area");
                NU_ASSERT(jarea);
                parse_area(jarea, &entries[i].dst.texture.area);
                cJSON *jslot = cJSON_GetObjectItem(jdst, "slot");
                NU_ASSERT(cJSON_IsNumber(jslot));
                entries[i].dst.texture.slot = cJSON_GetNumberValue(jslot);
            }
            break;
            case NUX_CHUNK_MESH: {
                cJSON *jdst = cJSON_GetObjectItem(jchild, "dst");
                NU_ASSERT(jdst);
                NU_ASSERT(cJSON_IsObject(jdst));
                cJSON *jfirst = cJSON_GetObjectItem(jdst, "first");
                NU_ASSERT(cJSON_IsNumber(jfirst));
                entries[i].dst.mesh.first = cJSON_GetNumberValue(jfirst);
            }
            break;
        }

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
write_chunk_header (FILE *f, const project_entry_t *entry, nu_size_t length)
{
    write_u32(f, entry->type);
    write_u32(f, length);
    printf("[chunk %d s:'%s' l:%lu]\n", entry->type, entry->src, length);
}
void
nux_build_cart (const nu_byte_t *path)
{
    // Parse json entries
    nu_byte_t        target[MAX_NAME_SIZE];
    nu_size_t        entry_count;
    project_entry_t *entries = NU_NULL;
    entries                  = parse_json(path, target, &entry_count);
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
        const project_entry_t *entry = entries + i;
        switch (entry->type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM: {
                nu_size_t  size;
                nu_byte_t *buffer = load_bytes(entry->src, &size);
                NU_ASSERT(buffer);
                // header
                write_chunk_header(f, entry, size);
                // data
                NU_ASSERT(fwrite(buffer, size, 1, f));
                free(buffer);
            }
            break;
            case NUX_CHUNK_TEXTURE: {
                int        w, h, n;
                nu_byte_t  fn[256];
                nu_byte_t *img
                    = stbi_load((char *)entry->src, &w, &h, &n, STBI_default);
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
                write_chunk_header(f, entry, length);
                // destination
                write_u32(f, entry->dst.texture.slot);
                write_v4u(f, entry->dst.texture.area);
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
