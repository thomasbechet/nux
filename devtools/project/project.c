#include "project.h"

#include "templates_data.h"
#include <parson/parson.h>
#include <importer/importer.h>

#define PROJECT_CART_DEFAULT "cart.bin"
#define PROJECT_JSON         "nux.json"
#define PROJECT_TARGET       "target"
#define PROJECT_CHUNKS       "chunks"
#define PROJECT_PREBUILD     "prebuild"
#define PROJECT_CHUNK_TYPE   "type"
#define PROJECT_CHUNK_SOURCE "source"
#define PROJECT_CHUNK_TARGET "target"

static const struct
{
    const nu_char_t *name;
    vm_chunk_type_t  type;
} name_to_chunk_type[] = {
    { "raw", VM_CHUNK_RAW },
    { "wasm", VM_CHUNK_WASM },
    { "texture", VM_CHUNK_TEXTURE },
    { "mesh", VM_CHUNK_MESH },
};

static void
project_init (project_t *project, nu_sv_t path)
{
    nu_memset(project, 0, sizeof(*project));
    nu_path_concat(
        project->target_path, NU_PATH_MAX, path, NU_SV(PROJECT_CART_DEFAULT));
}

static nu_f32_t
parse_f32 (const JSON_Object *object, const nu_char_t *name)
{
    NU_ASSERT(object && name);
    return json_object_get_number(object, name);
}
static void
parse_target (const JSON_Object *jchunk,
              vm_chunk_type_t    type,
              vm_chunk_target_t *target)
{
    const JSON_Object *jtarget
        = json_object_get_object(jchunk, PROJECT_CHUNK_TARGET);
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
write_f32 (JSON_Object *object, const nu_char_t *name, nu_f32_t value)
{
    json_object_set_number(object, name, value);
}
static void
write_target (JSON_Object             *chunk,
              vm_chunk_type_t          type,
              const vm_chunk_target_t *target)
{
    JSON_Value *jtargetv = json_value_init_object();
    NU_ASSERT(jtargetv);
    json_object_set_value(chunk, PROJECT_CHUNK_TARGET, jtargetv);
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

static void
write_u32 (FILE *f, nu_u32_t v)
{
    nu_u32_t a = nu_u32_le(v);
    NU_ASSERT(fwrite(&a, sizeof(a), 1, f));
}
static void
write_chunk_header (FILE *f, vm_chunk_header_t *header)
{
    // type / length
    write_u32(f, header->type);
    write_u32(f, header->length);
    // target
    switch (header->type)
    {
        case VM_CHUNK_RAW: {
        }
        break;
        case VM_CHUNK_WASM: {
        }
        break;
        case VM_CHUNK_TEXTURE: {
            write_u32(f, header->target.texture.slot);
            write_u32(f, header->target.texture.x);
            write_u32(f, header->target.texture.y);
            write_u32(f, header->target.texture.w);
            write_u32(f, header->target.texture.h);
        }
        break;
        case VM_CHUNK_MESH: {
        }
        break;
    }
}

nu_status_t
project_generate_template (nu_sv_t path, nu_sv_t lang, project_error_t *error)
{
    nu_status_t status = NU_SUCCESS;

    printf("Initialize new project " NU_SV_FMT " with language " NU_SV_FMT "\n",
           NU_SV_ARGS(path),
           NU_SV_ARGS(lang));

    // Find lang
    project_template_file_t *template_file = NU_NULL;
    if (nu_sv_eq(lang, NU_SV("c")))
    {
        template_file = template_c_files;
    }
    else if (nu_sv_eq(lang, NU_SV("rust")))
    {
        template_file = template_rust_files;
    }

    // Template found, generate files
    if (template_file)
    {
        while (template_file->path)
        {
            nu_char_t filepath[NU_PATH_MAX];
            nu_sv_t   filepath_sv = nu_path_concat(
                filepath, NU_PATH_MAX, path, nu_sv_cstr(template_file->path));
            NU_ASSERT(nu_save_bytes(
                filepath_sv, template_file->data, template_file->size));
            ++template_file;
        }
    }
    else
    {
        // Generate empty project file
        project_t project;
        project_init(&project, path);
        status = project_save(&project, path, error);
        project_free(&project);
    }
    return status;
}
nu_status_t
project_build (const project_t *project, project_error_t *error)
{
    // Execute prebuild command
    if (nu_strlen(project->prebuild))
    {
        printf("Execute prebuild command: %s\n", project->prebuild);
#ifdef NU_PLATFORM_UNIX
        system(project->prebuild);
#endif
    }

    // Open cart
    FILE *f = fopen(project->target_path, "wb");
    if (!f)
    {
        error->code = PROJECT_ERROR_IO_CART;
        return NU_FAILURE;
    }
    else
    {
        printf("Cartridge %s generated.\n", project->target_path);
    }

    // Write header
    const nu_u32_t version = 100;
    NU_ASSERT(fwrite(&version, sizeof(version), 1, f));
    const nu_u32_t chunk_count = project->chunks_count;
    NU_ASSERT(fwrite(&chunk_count, sizeof(chunk_count), 1, f));

    // Compile entries
    for (nu_size_t i = 0; i < project->chunks_count; ++i)
    {
        project_chunk_entry_t *entry = project->chunks + i;
        switch (entry->header.type)
        {
            case VM_CHUNK_RAW:
                break;
            case VM_CHUNK_WASM: {
                nu_size_t  size;
                nu_byte_t *buffer;
                NU_ASSERT(nu_load_bytes(
                    nu_sv_cstr(entry->source_path), NU_NULL, &size));
                buffer = malloc(size);
                NU_ASSERT(buffer);
                NU_ASSERT(nu_load_bytes(
                    nu_sv_cstr(entry->source_path), buffer, &size));
                // header
                entry->header.length = size;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(buffer, size, 1, f));
                free(buffer);
            }
            break;
            case VM_CHUNK_TEXTURE: {
                nu_v2u_t   size;
                nu_byte_t *data = importer_load_image(
                    nu_sv_cstr(entry->source_path), &size);
                nu_size_t length = sizeof(nu_byte_t) * size.x * size.y * 4;

                // header
                entry->header.length = length;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(data, length, 1, f));

                free(data);
            }
            break;
            case VM_CHUNK_MESH: {
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

    fclose(f);
    return NU_SUCCESS;
}
nu_status_t
project_load (project_t *project, nu_sv_t path, project_error_t *error)
{
    nu_status_t status = NU_SUCCESS;

    // Parse file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    JSON_Value *jrootv = json_parse_file(json_path);
    if (!jrootv)
    {
        error->code = PROJECT_ERROR_IO_PROJECT;
        return NU_FAILURE;
    }

    project_init(project, path);

    JSON_Object *jroot = json_object(jrootv);
    if (!jroot)
    {
        error->code = PROJECT_ERROR_MALFORMED;
        status      = NU_FAILURE;
        goto cleanup0;
    }

    // Entries
    JSON_Array *jchunks = json_object_get_array(jroot, PROJECT_CHUNKS);
    if (jchunks)
    {
        project->chunks_count = json_array_get_count(jchunks);
        if (project->chunks_count)
        {
            project->chunks
                = malloc(sizeof(*project->chunks) * project->chunks_count);
            NU_ASSERT(project->chunks);
        }
        for (nu_size_t i = 0; i < project->chunks_count; ++i)
        {
            JSON_Object *jchunk = json_array_get_object(jchunks, i);
            // Check fields
            const nu_char_t *type_string
                = json_object_get_string(jchunk, PROJECT_CHUNK_TYPE);
            NU_ASSERT(type_string);
            const nu_char_t *source_string
                = json_object_get_string(jchunk, PROJECT_CHUNK_SOURCE);
            NU_ASSERT(source_string);

            // Check type
            nu_sv_t         type_sv = nu_sv_cstr(type_string);
            vm_chunk_type_t type;
            nu_bool_t       found = NU_FALSE;
            for (nu_size_t j = 0;
                 j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
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
                error->code = PROJECT_ERROR_MALFORMED;
                status      = NU_FAILURE;
                goto cleanup0;
            }
            project->chunks[i].header.type = type;
            nu_sv_to_cstr(nu_sv_cstr(source_string),
                          project->chunks[i].source_path,
                          NU_PATH_MAX);

            // Parse target object
            parse_target(jchunk, type, &project->chunks[i].header.target);
        }
    }

    // Prebuild
    const nu_char_t *jprebuild
        = json_object_get_string(jroot, PROJECT_PREBUILD);
    if (jprebuild)
    {
        nu_sv_to_cstr(
            nu_sv_cstr(jprebuild), project->prebuild, PROJECT_NAME_MAX);
    }

    json_value_free(jrootv);
    return NU_SUCCESS;

cleanup0:
    json_value_free(jrootv);
    project_free(project);
    return status;
}
nu_status_t
project_save (const project_t *project, nu_sv_t path, project_error_t *error)
{
    nu_status_t status = NU_SUCCESS;

    // Build json object
    JSON_Value *jrootv = json_value_init_object();
    NU_CHECK(jrootv, goto cleanup0);
    JSON_Object *jroot = json_object(jrootv);

    // Target
    json_object_set_string(jroot, PROJECT_TARGET, project->target_path);

    // Chunks
    if (nu_strlen(project->target_path))
    {
        JSON_Value *jchunksv = json_value_init_array();
        NU_CHECK(jchunksv, goto cleanup1);
        json_object_set_value(jroot, PROJECT_CHUNKS, jchunksv);
        JSON_Array *jchunks = json_array(jchunksv);

        for (nu_size_t i = 0; i < project->chunks_count; ++i)
        {
            JSON_Value *jchunkv = json_value_init_object();
            NU_CHECK(jchunkv, goto cleanup1);
            json_array_append_value(jchunks, jchunkv);
            JSON_Object *jchunk = json_object(jchunkv);

            // Type
            const nu_char_t *type_str = NU_NULL;
            nu_bool_t        found    = NU_FALSE;
            for (nu_size_t j = 0;
                 j < NU_ARRAY_SIZE(name_to_chunk_type) && !found;
                 ++j)
            {
                if (name_to_chunk_type[j].type
                    == project->chunks[i].header.type)
                {
                    type_str = name_to_chunk_type[j].name;
                    found    = NU_TRUE;
                }
            }
            NU_ASSERT(found);
            json_object_set_string(jchunk, PROJECT_CHUNK_TYPE, type_str);

            // Source
            json_object_set_string(
                jchunk, PROJECT_CHUNK_SOURCE, project->chunks[i].source_path);

            // Write chunk target
            write_target(jchunk,
                         project->chunks[i].header.type,
                         &project->chunks[i].header.target);
        }
    }

    // Prebuild
    if (nu_strlen(project->prebuild))
    {
        json_object_set_string(jroot, PROJECT_PREBUILD, project->prebuild);
    }

    // Write json file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    if (json_serialize_to_file_pretty(jrootv, json_path))
    {
        error->code = PROJECT_ERROR_IO_PROJECT;
        status      = NU_FAILURE;
        goto cleanup1;
    }

cleanup1:
    json_value_free(jrootv);
cleanup0:
    return status;
}
void
project_free (project_t *project)
{
    if (project->chunks)
    {
        NU_ASSERT(project->chunks_count);
        free(project->chunks);
    }
}
