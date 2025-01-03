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
    nu_u32_t         dst;
} project_entry_t;

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
        cJSON *jdst = cJSON_GetObjectItem(jchild, "dst");
        NU_ASSERT(jdst);
        NU_ASSERT(cJSON_IsNumber(jdst));

        // check type
        nu_str_t type_str
            = nu_str_from_cstr((nu_byte_t *)cJSON_GetStringValue(jtype));
        nux_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
        const struct
        {
            nu_str_t         name;
            nux_chunk_type_t type;
        } name_to_chunk_type[] = { { NU_STR("wasm"), NUX_CHUNK_WASM },
                                   { NU_STR("tex128"), NUX_CHUNK_TEX128 } };
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
        entries[i].dst = cJSON_GetNumberValue(jdst);

        jchild = jchild->next;
    }
    cJSON_Delete(root);
    free(jfile);
    return entries;
}

static nu_bool_t
validate_entries (const project_entry_t *entries, nu_size_t entry_count)
{
    for (nu_size_t i = 0; i < entry_count; ++i)
    {
        for (nu_size_t j = 0; j < entry_count; j++)
        {
            if (i != j)
            {
                const project_entry_t *a = entries + i;
                const project_entry_t *b = entries + j;
                if (a->type == b->type && a->dst == b->dst)
                {
                    printf(
                        "Conflict destination for entry %lu and %lu\n", i, j);
                    return NU_FALSE;
                }
            }
        }
    }
    return NU_TRUE;
}
static void
write_chunk_header (FILE *f, const project_entry_t *entry, nu_size_t length)
{
    nu_u32_t t = nu_u32_le(entry->type);
    nu_u32_t l = nu_u32_le(length);
    nu_u32_t d = nu_u32_le(entry->dst);
    NU_ASSERT(fwrite(&t, sizeof(t), 1, f));
    NU_ASSERT(fwrite(&l, sizeof(l), 1, f));
    NU_ASSERT(fwrite(&d, sizeof(d), 1, f));
    printf("[chunk %d s:'%s' d:%d l:%lu]\n",
           entry->type,
           entry->src,
           entry->dst,
           length);
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

    // Validate entries
    NU_ASSERT(validate_entries(entries, entry_count));

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
                write_chunk_header(f, entry, size);
                NU_ASSERT(fwrite(buffer, size, 1, f));
                free(buffer);
            }
            break;
            case NUX_CHUNK_TEX64: {
            }
            break;
            case NUX_CHUNK_TEX128: {
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
                write_chunk_header(f, entry, length);
                NU_ASSERT(fwrite(output, length, 1, f));
                free(output);
                stbi_image_free(img);
            }
            break;
            case NUX_CHUNK_TEX256:
                break;
        }
    }

    // Free resources
    fclose(f);
cleanup0:
    free(entries);
}
