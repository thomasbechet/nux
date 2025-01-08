#include "project.h"

#include "nulib/string.h"

#include <cJSON/cJSON.h>

static const struct
{
    const nu_char_t *name;
    nux_chunk_type_t type;
} name_to_chunk_type[] = {
    { "raw", NUX_CHUNK_RAW },
    { "wasm", NUX_CHUNK_WASM },
    { "texture", NUX_CHUNK_TEXTURE },
    { "mesh", NUX_CHUNK_MESH },
};

nu_byte_t *
load_bytes (nu_sv_t filename, nu_size_t *size)
{
    char buf[NU_PATH_MAX];
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    FILE *f = fopen(buf, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", buf);
        return NU_NULL;
    }
    NU_ASSERT(!fseek(f, 0, SEEK_END));
    long fsize = ftell(f);
    NU_ASSERT(fsize != -1);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)malloc(fsize);
    if (!bytes)
    {
        printf("Failed to allocate %lu bytes for file %s\n", fsize, buf);
        fclose(f);
        return NU_NULL;
    }
    fread(bytes, fsize, 1, f);
    fclose(f);
    if (size)
    {
        *size = fsize;
    }
    return bytes;
}
nu_bool_t
save_bytes (nu_sv_t filename, const nu_byte_t *data, nu_size_t size)
{
    char buf[NU_PATH_MAX]; // TODO: support PATH_MAX
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    FILE *f = fopen(buf, "wb");
    if (!f)
    {
        printf("Failed to open file %s\n", buf);
        return NU_NULL;
    }
    nu_int_t n = fwrite(data, size, 1, f);
    fclose(f);
    return n == 1;
}

static nu_f32_t
parse_f32 (const cJSON *object, const nu_char_t *name)
{
    NU_ASSERT(object && name && cJSON_IsObject(object));
    cJSON *f = cJSON_GetObjectItem(object, name);
    NU_ASSERT(f && cJSON_IsNumber(f));
    return cJSON_GetNumberValue(f);
}
static nu_bool_t
write_f32 (cJSON *object, const nu_char_t *name, nu_f32_t value)
{
    cJSON *jvalue = cJSON_CreateNumber(value);
    NU_CHECK(jvalue, return NU_FALSE);
    cJSON_AddItemToObject(object, name, jvalue);
    return NU_TRUE;
}

static void
parse_target (const cJSON        *jchunk,
              nux_chunk_type_t    type,
              nux_chunk_target_t *target)
{
    const cJSON *jtarget = cJSON_GetObjectItem(jchunk, "target");
    switch (type)
    {
        case NUX_CHUNK_RAW: {
            NU_ASSERT(jtarget && cJSON_IsObject(jtarget));
            target->raw.addr = parse_f32(jtarget, "addr");
        }
        break;
        case NUX_CHUNK_WASM:
            break;
        case NUX_CHUNK_TEXTURE: {
            NU_ASSERT(jtarget && cJSON_IsObject(jtarget));
            target->texture.slot = parse_f32(jtarget, "slot");
            target->texture.x    = parse_f32(jtarget, "x");
            target->texture.y    = parse_f32(jtarget, "y");
            target->texture.w    = parse_f32(jtarget, "w");
            target->texture.h    = parse_f32(jtarget, "h");
        }
        break;
        case NUX_CHUNK_MESH: {
            NU_ASSERT(jtarget && cJSON_IsObject(jtarget));
            target->mesh.first = parse_f32(jtarget, "first");
            target->mesh.count = parse_f32(jtarget, "count");
        }
        break;
    }
}
static void
write_target (cJSON                    *chunk,
              nux_chunk_type_t          type,
              const nux_chunk_target_t *target)
{
    cJSON *jtarget = cJSON_CreateObject();
    NU_ASSERT(jtarget);
    cJSON_AddItemToObject(chunk, "target", jtarget);
    switch (type)
    {
        case NUX_CHUNK_RAW: {
            NU_ASSERT(write_f32(jtarget, "addr", target->raw.addr));
        }
        break;
        case NUX_CHUNK_WASM:
            break;
        case NUX_CHUNK_TEXTURE: {
            NU_ASSERT(write_f32(jtarget, "slot", target->texture.slot));
            NU_ASSERT(write_f32(jtarget, "x", target->texture.x));
            NU_ASSERT(write_f32(jtarget, "y", target->texture.y));
            NU_ASSERT(write_f32(jtarget, "w", target->texture.w));
            NU_ASSERT(write_f32(jtarget, "h", target->texture.h));
        }
        break;
        case NUX_CHUNK_MESH: {
            NU_ASSERT(write_f32(jtarget, "first", target->mesh.first));
            NU_ASSERT(write_f32(jtarget, "count", target->mesh.count));
        }
        break;
    }
}

nu_bool_t
nux_project_load (nux_project_t *project, nu_sv_t path)
{
    // Initialize project
    nu_memset(project, 0, sizeof(*project));
    nu_char_t json_path[NU_PATH_MAX];
    nu_sv_t   json_path_sv
        = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV("nux.json"));

    // Parse file
    nu_byte_t *jfile = load_bytes(json_path_sv, NU_NULL);
    NU_ASSERT(jfile);
    cJSON *jroot = cJSON_Parse((const char *)jfile);
    NU_ASSERT(jroot);
    NU_ASSERT(cJSON_IsObject(jroot));

    // Compute target name
    nu_path_concat(project->target_path, NU_PATH_MAX, path, NU_SV("cart.bin"));

    // Parse entries
    cJSON *jentries = cJSON_GetObjectItem(jroot, "chunks");
    NU_ASSERT(cJSON_IsArray(jentries));
    project->entry_count = cJSON_GetArraySize(jentries);
    project->entries = malloc(sizeof(nux_chunk_entry_t) * project->entry_count);
    NU_ASSERT(project->entries);
    cJSON *jchunk = jentries->child;
    for (nu_size_t i = 0; i < project->entry_count; ++i)
    {
        // Check fields
        cJSON *jtype = cJSON_GetObjectItem(jchunk, "type");
        NU_ASSERT(jtype);
        NU_ASSERT(cJSON_IsString(jtype));
        cJSON *jsource = cJSON_GetObjectItem(jchunk, "source");
        NU_ASSERT(jsource);
        NU_ASSERT(cJSON_IsString(jsource));

        // Check type
        nu_sv_t          type_str = nu_sv_cstr(cJSON_GetStringValue(jtype));
        nux_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
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
        project->entries[i].header.type = type;
        nu_sv_to_cstr(nu_sv_cstr(cJSON_GetStringValue(jsource)),
                      project->entries[i].source_path,
                      NU_PATH_MAX);

        // Parse target object
        parse_target(jchunk, type, &project->entries[i].header.target);

        jchunk = jchunk->next;
    }
    cJSON_Delete(jroot);
    free(jfile);
    return NU_TRUE;
}
nu_bool_t
nux_project_save (const nux_project_t *project, nu_sv_t path)
{
    // Build json object
    cJSON *jroot = cJSON_CreateObject();
    NU_CHECK(jroot, goto cleanup0);

    cJSON *jtarget_path = cJSON_CreateString(project->target_path);
    NU_CHECK(jtarget_path, goto cleanup1);
    cJSON_AddItemToObject(jroot, "target", jtarget_path);

    cJSON *jchunks = cJSON_CreateArray();
    NU_CHECK(jchunks, goto cleanup1);
    cJSON_AddItemToObject(jroot, "chunks", jchunks);

    NU_ASSERT(project->entries);
    for (nu_size_t i = 0; i < project->entry_count; ++i)
    {
        cJSON *jchunk = cJSON_CreateObject();
        NU_CHECK(jchunk, goto cleanup1);
        cJSON_AddItemToArray(jchunks, jchunk);

        cJSON *jsource = cJSON_CreateString(project->entries[i].source_path);
        NU_CHECK(jsource, goto cleanup1);
        cJSON_AddItemToObject(jchunk, "source", jsource);

        const nu_char_t *type_str = NU_NULL;
        nu_bool_t        found    = NU_FALSE;
        for (nu_size_t j = 0; j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
             ++j)
        {
            if (name_to_chunk_type[j].type == project->entries[i].header.type)
            {
                type_str = name_to_chunk_type[j].name;
                found    = NU_TRUE;
            }
        }
        NU_ASSERT(found);

        // Write chunk target
        write_target(jchunk,
                     project->entries[i].header.type,
                     &project->entries[i].header.target);
    }

    // Write json file
    nu_char_t json_path[NU_PATH_MAX];
    nu_sv_t   json_path_sv
        = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV("nux.json"));

    nu_char_t *json = cJSON_Print(jroot);
    NU_ASSERT(json);
    save_bytes(json_path_sv, (const nu_byte_t *)json, nu_strlen(json) + 1);

    cJSON_free(json);

    return NU_TRUE;

cleanup1:
    cJSON_free(jroot);
cleanup0:
    return NU_FALSE;
}
void
nux_project_free (nux_project_t *project)
{
    if (project->entries)
    {
        NU_ASSERT(project->entry_count);
        free(project->entries);
    }
}
