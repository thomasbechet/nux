#include "package.h"

#include "nulib/string.h"

#include <cJSON/cJSON.h>

nu_byte_t *
load_bytes (nu_sv_t filename, nu_size_t *size)
{
    char buf[256]; // TODO: support PATH_MAX
    nu_sv_to_cstr(filename, buf, sizeof(buf));
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

static nux_chunk_entry_t *
parse_package (nu_sv_t path, nux_package_t *pkg)
{
    // Parse file
    nu_byte_t *jfile = load_bytes(path, NU_NULL);
    NU_ASSERT(jfile);
    cJSON *root = cJSON_Parse((const char *)jfile);
    NU_ASSERT(root);
    NU_ASSERT(cJSON_IsObject(root));

    // Parse project name
    cJSON *jcart = cJSON_GetObjectItem(root, "name");
    NU_ASSERT(jcart);
    NU_ASSERT(cJSON_IsString(jcart));
    nu_sv_to_cstr(
        nu_sv_cstr(cJSON_GetStringValue(jcart)), pkg->name, NUX_NAME_MAX);

    // Compute target name
    nu_char_t target_name[NUX_NAME_MAX];
    nu_sv_join(target_name, NUX_NAME_MAX, nu_sv_cstr(pkg->name), NU_SV(".bin"));
    nu_path_concat(
        pkg->target_path, NU_PATH_MAX, path, nu_sv_cstr(target_name));

    // Parse entries
    cJSON *jentries = cJSON_GetObjectItem(root, "chunks");
    NU_ASSERT(cJSON_IsArray(jentries));
    pkg->entry_count = cJSON_GetArraySize(jentries);
    nux_chunk_entry_t *entries
        = malloc(sizeof(nux_chunk_entry_t) * pkg->entry_count);
    NU_ASSERT(entries);
    cJSON *jchild = jentries->child;
    for (nu_size_t i = 0; i < pkg->entry_count; ++i)
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
        nu_sv_t          type_str = nu_sv_cstr(cJSON_GetStringValue(jtype));
        nux_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
        const struct
        {
            const nu_char_t *name;
            nux_chunk_type_t type;
        } name_to_chunk_type[] = {
            { "raw", NUX_CHUNK_RAW },
            { "wasm", NUX_CHUNK_WASM },
            { "texture", NUX_CHUNK_TEXTURE },
            { "mesh", NUX_CHUNK_MESH },
        };
        for (nu_size_t j = 0; j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
             ++j)
        {
            if (nu_sv_eq(nu_sv_cstr(name_to_chunk_type[j].name), type_str))
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
        nu_sv_to_cstr(nu_sv_cstr(cJSON_GetStringValue(jsource)),
                      entries[i].source_path,
                      NU_PATH_MAX);

        // parse target object
        parse_target(jtarget, type, &entries[i].header.target);

        jchild = jchild->next;
    }
    cJSON_Delete(root);
    free(jfile);
    return entries;
}

void
nux_package_load (nu_sv_t path, nux_package_t *package)
{
    nu_memset(package, 0, sizeof(*package));
    nu_char_t json_path[NU_PATH_MAX];
    nu_sv_t   json_path_sv
        = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV("nux.json"));
    parse_package(json_path_sv, package);
}
void
nux_package_free (nux_package_t *package)
{
    if (package->entries)
    {
        NU_ASSERT(package->entry_count);
        free(package->entries);
    }
}
