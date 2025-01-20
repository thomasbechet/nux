#include "sdk.h"

#include "templates_data.h"
#include <parson/parson.h>

#define PROJECT_CART_DEFAULT "cart.bin"
#define PROJECT_JSON         "nux.json"
#define PROJECT_TARGET       "target"
#define PROJECT_CHUNKS       "chunks"
#define PROJECT_PREBUILD     "prebuild"
#define PROJECT_CHUNK_TYPE   "type"
#define PROJECT_CHUNK_SOURCE "source"
#define PROJECT_CHUNK_META   "meta"

static const struct
{
    const nu_char_t  *name;
    cart_chunk_type_t type;
} name_to_chunk_type[] = {
    { "raw", CART_CHUNK_RAW },
    { "wasm", CART_CHUNK_WASM },
    { "texture", CART_CHUNK_TEXTURE },
    { "mesh", CART_CHUNK_MESH },
};

static void
project_init (sdk_project_t *project, nu_sv_t path)
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
parse_meta (const JSON_Object *jchunk,
            cart_chunk_type_t  type,
            cart_chunk_meta_t *meta)
{
    const JSON_Object *jmeta
        = json_object_get_object(jchunk, PROJECT_CHUNK_META);
    switch (type)
    {
        case CART_CHUNK_RAW: {
            meta->raw.addr = parse_f32(jmeta, "addr");
        }
        break;
        case CART_CHUNK_WASM:
            break;
        case CART_CHUNK_TEXTURE: {
            meta->texture.index  = parse_f32(jmeta, "index");
            meta->texture.width  = parse_f32(jmeta, "width");
            meta->texture.height = parse_f32(jmeta, "height");
        }
        break;
        case CART_CHUNK_MESH: {
            meta->mesh.index = parse_f32(jmeta, "index");
            meta->mesh.count = parse_f32(jmeta, "count");
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
write_meta (JSON_Object             *chunk,
            cart_chunk_type_t        type,
            const cart_chunk_meta_t *meta)
{
    JSON_Value *jmetav = json_value_init_object();
    NU_ASSERT(jmetav);
    json_object_set_value(chunk, PROJECT_CHUNK_META, jmetav);
    JSON_Object *jmeta = json_object(jmetav);
    switch (type)
    {
        case CART_CHUNK_RAW: {
            write_f32(jmeta, "addr", meta->raw.addr);
        }
        break;
        case CART_CHUNK_WASM:
            break;
        case CART_CHUNK_TEXTURE: {
            write_f32(jmeta, "slot", meta->texture.index);
            write_f32(jmeta, "width", meta->texture.width);
            write_f32(jmeta, "height", meta->texture.height);
        }
        break;
        case CART_CHUNK_MESH: {
            write_f32(jmeta, "index", meta->mesh.index);
            write_f32(jmeta, "count", meta->mesh.count);
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
write_chunk_header (FILE *f, cart_chunk_header_t *header)
{
    // type / length
    write_u32(f, header->type);
    write_u32(f, header->length);
    // meta
    switch (header->type)
    {
        case CART_CHUNK_RAW: {
        }
        break;
        case CART_CHUNK_WASM: {
        }
        break;
        case CART_CHUNK_TEXTURE: {
            write_u32(f, header->meta.texture.index);
            write_u32(f, header->meta.texture.width);
            write_u32(f, header->meta.texture.height);
        }
        break;
        case CART_CHUNK_MESH: {
        }
        break;
    }
}

nu_status_t
sdk_generate_template (nu_sv_t path, nu_sv_t lang)
{
    nu_status_t status = NU_SUCCESS;

    // Find lang
    sdk_template_file_t *template_file = NU_NULL;
    if (nu_sv_eq(lang, NU_SV("c")))
    {
        template_file = template_c_files;
    }
    else if (nu_sv_eq(lang, NU_SV("rust")))
    {
        template_file = template_rust_files;
    }
    else if (!nu_sv_is_null(lang))
    {
        sdk_log(NU_LOG_ERROR,
                "Project language '" NU_SV_FMT "' not found or not supported",
                NU_SV_ARGS(lang));
        return NU_FAILURE;
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
        sdk_log(NU_LOG_INFO,
                "Project generated with language '" NU_SV_FMT "'",
                NU_SV_ARGS(lang));
    }
    else
    {
        // Generate empty sdk file
        sdk_project_t project;
        project_init(&project, path);
        status = sdk_project_save(&project, path);
        sdk_project_free(&project);
        sdk_log(NU_LOG_INFO, "Empty project generated");
    }
    return status;
}
nu_status_t
sdk_build (const sdk_project_t *project)
{
    nu_status_t status = NU_SUCCESS;
    // Execute prebuild command
    if (nu_strlen(project->prebuild))
    {
        sdk_log(
            NU_LOG_INFO, "Executing prebuild command: %s", project->prebuild);
#ifdef NU_PLATFORM_UNIX
        system(project->prebuild);
#endif
    }

    // Open cart
    FILE *f = fopen(project->target_path, "wb");
    if (!f)
    {
        sdk_log(NU_LOG_ERROR,
                "Failed to create cartridge file %s",
                project->target_path);
        return NU_FAILURE;
    }
    else
    {
        sdk_log(NU_LOG_INFO, "Compiling cartridge %s...", project->target_path);
    }

    // Write header
    const nu_u32_t version = 100;
    NU_ASSERT(fwrite(&version, sizeof(version), 1, f));
    const nu_u32_t chunk_count = project->chunks_count;
    NU_ASSERT(fwrite(&chunk_count, sizeof(chunk_count), 1, f));

    // Compile entries
    for (nu_size_t i = 0; i < project->chunks_count; ++i)
    {
        sdk_project_chunk_t *entry = project->chunks + i;
        switch (entry->header.type)
        {
            case CART_CHUNK_RAW:
                break;
            case CART_CHUNK_WASM: {
                sdk_log(
                    NU_LOG_INFO, "Compiling wasm chunk %s", entry->source_path);
                nu_size_t  size;
                nu_byte_t *buffer;
                if (!nu_load_bytes(
                        nu_sv_cstr(entry->source_path), NU_NULL, &size))
                {
                    sdk_log(NU_LOG_ERROR,
                            "Failed to load wasm file %s",
                            entry->source_path);
                    status = NU_FAILURE;
                    goto cleanup0;
                }
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
            case CART_CHUNK_TEXTURE: {
                sdk_log(NU_LOG_INFO,
                        "Compiling texture chunk %s",
                        entry->source_path);
                nu_v2u_t   size;
                nu_byte_t *data
                    = sdk_load_image(nu_sv_cstr(entry->source_path), &size);
                if (!data)
                {
                    sdk_log(NU_LOG_ERROR,
                            "Failed to load texture file %s",
                            entry->source_path);
                    status = NU_FAILURE;
                    goto cleanup0;
                }
                nu_size_t length = sizeof(nu_byte_t) * size.x * size.y * 4;

                // header
                entry->header.length = length;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(data, length, 1, f));

                free(data);
            }
            break;
            case CART_CHUNK_MESH: {
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

    sdk_log(NU_LOG_INFO, "Compilation success.");

cleanup0:
    fclose(f);
    return status;
}
nu_status_t
sdk_project_load (sdk_project_t *project, nu_sv_t path)
{
    nu_status_t status = NU_SUCCESS;

    // Parse file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    JSON_Value *jrootv = json_parse_file(json_path);
    if (!jrootv)
    {
        sdk_log(NU_LOG_ERROR, "Failed to parse project file %s", json_path);
        return NU_FAILURE;
    }

    project_init(project, path);

    JSON_Object *jroot = json_object(jrootv);
    if (!jroot)
    {
        sdk_log(NU_LOG_ERROR, "Project json root is not an object");
        status = NU_FAILURE;
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
            nu_sv_t           type_sv = nu_sv_cstr(type_string);
            cart_chunk_type_t type;
            nu_bool_t         found = NU_FALSE;
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
                sdk_log(NU_LOG_ERROR, "Invalid chunk type %s", type_string);
                status = NU_FAILURE;
                goto cleanup0;
            }
            project->chunks[i].header.type = type;
            nu_sv_to_cstr(nu_sv_cstr(source_string),
                          project->chunks[i].source_path,
                          NU_PATH_MAX);

            // Parse meta
            parse_meta(jchunk, type, &project->chunks[i].header.meta);
        }
    }

    // Prebuild
    const nu_char_t *jprebuild
        = json_object_get_string(jroot, PROJECT_PREBUILD);
    if (jprebuild)
    {
        nu_sv_to_cstr(nu_sv_cstr(jprebuild), project->prebuild, SDK_NAME_MAX);
    }

    json_value_free(jrootv);
    return NU_SUCCESS;

cleanup0:
    json_value_free(jrootv);
    sdk_project_free(project);
    return status;
}
nu_status_t
sdk_project_save (const sdk_project_t *project, nu_sv_t path)
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

            // Write chunk meta
            write_meta(jchunk,
                       project->chunks[i].header.type,
                       &project->chunks[i].header.meta);
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
        sdk_log(
            NU_LOG_ERROR, "Failed to serialize project to json %s", json_path);
        status = NU_FAILURE;
        goto cleanup1;
    }

cleanup1:
    json_value_free(jrootv);
cleanup0:
    return status;
}
void
sdk_project_free (sdk_project_t *project)
{
    if (project->chunks)
    {
        NU_ASSERT(project->chunks_count);
        free(project->chunks);
    }
}
