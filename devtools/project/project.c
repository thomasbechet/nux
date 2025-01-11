#include "project.h"

#include <parson/parson.h>

#define PROJECT_JSON "nux.json"

static const struct
{
    const nu_char_t *name;
    vm_chunk_type_t type;
} name_to_chunk_type[] = {
    { "raw", VM_CHUNK_RAW },
    { "wasm", VM_CHUNK_WASM },
    { "texture", VM_CHUNK_TEXTURE },
    { "mesh", VM_CHUNK_MESH },
};

static nu_f32_t
parse_f32 (const JSON_Object *object, const nu_char_t *name)
{
    NU_ASSERT(object && name);
    return json_object_get_number(object, name);
}
static void
write_f32 (JSON_Object *object, const nu_char_t *name, nu_f32_t value)
{
    json_object_set_number(object, name, value);
}

static void
parse_target (const JSON_Object  *jchunk,
              vm_chunk_type_t    type,
              vm_chunk_target_t *target)
{
    const JSON_Object *jtarget = json_object_get_object(jchunk, "target");
    switch (type)
    {
        case VM_CHUNK_RAW: {
            target->raw.addr = parse_f32(jtarget, "addr");
        }
        break;
        case VM_CHUNK_WASM:
            break;
        case VM_CHUNK_TEXTURE: {
            target->texture.slot = parse_f32(jtarget, "slot");
            target->texture.x    = parse_f32(jtarget, "x");
            target->texture.y    = parse_f32(jtarget, "y");
            target->texture.w    = parse_f32(jtarget, "w");
            target->texture.h    = parse_f32(jtarget, "h");
        }
        break;
        case VM_CHUNK_MESH: {
            target->mesh.first = parse_f32(jtarget, "first");
            target->mesh.count = parse_f32(jtarget, "count");
        }
        break;
    }
}
static void
write_target (JSON_Object              *chunk,
              vm_chunk_type_t          type,
              const vm_chunk_target_t *target)
{
    JSON_Value *jtargetv = json_value_init_object();
    NU_ASSERT(jtargetv);
    json_object_set_value(chunk, "target", jtargetv);
    JSON_Object *jtarget = json_object(jtargetv);
    switch (type)
    {
        case VM_CHUNK_RAW: {
            write_f32(jtarget, "addr", target->raw.addr);
        }
        break;
        case VM_CHUNK_WASM:
            break;
        case VM_CHUNK_TEXTURE: {
            write_f32(jtarget, "slot", target->texture.slot);
            write_f32(jtarget, "x", target->texture.x);
            write_f32(jtarget, "y", target->texture.y);
            write_f32(jtarget, "w", target->texture.w);
            write_f32(jtarget, "h", target->texture.h);
        }
        break;
        case VM_CHUNK_MESH: {
            write_f32(jtarget, "first", target->mesh.first);
            write_f32(jtarget, "count", target->mesh.count);
        }
        break;
    }
}

nu_bool_t
project_init_empty (project_t *project, nu_sv_t path)
{
    nu_memset(project, 0, sizeof(*project));
    nu_path_concat(project->target_path, NU_PATH_MAX, path, NU_SV("cart.bin"));
    return NU_TRUE;
}
nu_bool_t
project_load (project_t *project, nu_sv_t path)
{
    project_init_empty(project, path);

    // Parse file
    nu_char_t json_path[NU_PATH_MAX];
    nu_sv_t   json_path_sv
        = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    JSON_Value *jrootv = json_parse_file(json_path);
    NU_ASSERT(jrootv);
    JSON_Object *jroot = json_object(jrootv);
    NU_ASSERT(jroot);

    // Entries
    JSON_Array *jentries   = json_object_get_array(jroot, "chunks");
    project->entries_count = json_array_get_count(jentries);
    if (project->entries_count)
    {
        project->entries
            = malloc(sizeof(*project->entries) * project->entries_count);
        NU_ASSERT(project->entries);
    }
    for (nu_size_t i = 0; i < project->entries_count; ++i)
    {
        JSON_Object *jchunk = json_array_get_object(jentries, i);
        // Check fields
        const nu_char_t *type_string = json_object_get_string(jchunk, "type");
        NU_ASSERT(type_string);
        const nu_char_t *source_string
            = json_object_get_string(jchunk, "source");
        NU_ASSERT(source_string);

        // Check type
        nu_sv_t          type_sv = nu_sv_cstr(type_string);
        vm_chunk_type_t type;
        nu_bool_t        found = NU_FALSE;
        for (nu_size_t j = 0; j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
             ++j)
        {
            if (nu_sv_eq(nu_sv_cstr(name_to_chunk_type[j].name), type_sv))
            {
                type  = name_to_chunk_type[j].type;
                found = NU_TRUE;
            }
        }
        if (!found)
        {
            printf("Chunk type not found for entry %lu\n", i);
            goto cleanup0;
        }
        project->entries[i].header.type = type;
        nu_sv_to_cstr(nu_sv_cstr(source_string),
                      project->entries[i].source_path,
                      NU_PATH_MAX);

        // Parse target object
        parse_target(jchunk, type, &project->entries[i].header.target);
    }

    // Prebuild
    const nu_char_t *jprebuild = json_object_get_string(jroot, "prebuild");
    if (jprebuild)
    {
        nu_sv_to_cstr(
            nu_sv_cstr(jprebuild), project->prebuild, PROJECT_NAME_MAX);
    }

    json_value_free(jrootv);
    return NU_TRUE;
cleanup0:
    json_value_free(jrootv);
    project_free(project);
    return NU_FALSE;
}
nu_bool_t
project_save (const project_t *project, nu_sv_t path)
{
    // Build json object
    JSON_Value *jrootv = json_value_init_object();
    NU_CHECK(jrootv, goto cleanup0);
    JSON_Object *jroot = json_object(jrootv);

    // Target
    json_object_set_string(jroot, "target", project->target_path);

    // Chunks
    JSON_Value *jchunksv = json_value_init_array();
    NU_CHECK(jchunksv, goto cleanup1);
    json_object_set_value(jroot, "chunks", jchunksv);
    JSON_Array *jchunks = json_array(jchunksv);

    for (nu_size_t i = 0; i < project->entries_count; ++i)
    {
        JSON_Value *jchunkv = json_value_init_object();
        NU_CHECK(jchunkv, goto cleanup1);
        json_array_append_value(jchunks, jchunkv);
        JSON_Object *jchunk = json_object(jchunkv);

        // Type
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
        json_object_set_string(jchunk, "type", type_str);

        // Source
        json_object_set_string(
            jchunk, "source", project->entries[i].source_path);

        // Write chunk target
        write_target(jchunk,
                     project->entries[i].header.type,
                     &project->entries[i].header.target);
    }

    // Prebuild
    if (nu_strlen(project->prebuild))
    {
        json_object_set_string(jroot, "prebuild", project->prebuild);
    }

    // Write json file
    nu_char_t json_path[NU_PATH_MAX];
    nu_sv_t   json_path_sv
        = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));

    json_serialize_to_file_pretty(jrootv, json_path);

    json_value_free(jrootv);

    return NU_TRUE;

cleanup1:
    json_value_free(jrootv);
cleanup0:
    return NU_FALSE;
}
void
project_free (project_t *project)
{
    if (project->entries)
    {
        NU_ASSERT(project->entries_count);
        free(project->entries);
    }
}
