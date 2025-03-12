#include "sdk.h"

#include "templates_data.c.inc"

#include <runtime/runtime.h>

#define PROJECT_CART_DEFAULT "cart.bin"
#define PROJECT_JSON         "nux.json"
#define PROJECT_TARGET       "target"
#define PROJECT_PREBUILD     "prebuild"
#define PROJECT_ASSETS       "assets"
#define PROJECT_ASSET_TYPE   "type"
#define PROJECT_ASSET_ID     "id"
#define PROJECT_ASSET_SOURCE "source"
#define PROJECT_ASSET_IGNORE "ignore"

static NU_ENUM_MAP(asset_type_map,
                   NU_ENUM_NAME(SDK_ASSET_WASM, "wasm"),
                   NU_ENUM_NAME(SDK_ASSET_TEXTURE, "texture"),
                   NU_ENUM_NAME(SDK_ASSET_MODEL, "model"));

static void
project_init (sdk_project_t *project, nu_sv_t path)
{
    nu_memset(project, 0, sizeof(*project));
    nu_path_concat(
        project->target_path, NU_PATH_MAX, path, NU_SV(PROJECT_CART_DEFAULT));
    project->pcg = nu_pcg(52873423, 12986124);
}

nu_status_t
sdk_generate_template (nu_sv_t path, nu_sv_t lang)
{
    nu_status_t status = NU_SUCCESS;

    // Detect existing project
    {
        nu_char_t json_path[NU_PATH_MAX];
        nu_sv_t   project_path
            = nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
        nu_size_t size;
        if (nu_load_bytes(project_path, NU_NULL, &size) == NU_SUCCESS)
        {
            logger_log(NU_LOG_ERROR,
                       "Directory '" NU_SV_FMT
                       "' already contains a " PROJECT_JSON " file",
                       NU_SV_ARGS(path));
            return NU_FAILURE;
        }
    }

    // Find lang
    sdk_template_file_t *template_file = NU_NULL;
    if (nu_sv_eq(lang, NU_SV("c")))
    {
        template_file = template_c_files;
    }
    else if (nu_sv_eq(lang, NU_SV("cxx")))
    {
        template_file = template_cxx_files;
    }
    else if (nu_sv_eq(lang, NU_SV("rust")))
    {
        template_file = template_rust_files;
    }
    else
    {
        logger_log(NU_LOG_ERROR,
                   "Project language '" NU_SV_FMT
                   "' not found or not supported",
                   NU_SV_ARGS(lang));
        return NU_FAILURE;
    }

    // Template found, generate files
    if (template_file)
    {
        while (template_file->path)
        {
            nu_char_t filepath[NU_PATH_MAX];
            nu_sv_t   filepath_sv
                = nu_path_concat(filepath,
                                 NU_PATH_MAX,
                                 path,
                                 nu_sv(template_file->path, NU_PATH_MAX));
            NU_ASSERT(nu_save_bytes(
                filepath_sv, template_file->data, template_file->size));
            ++template_file;
        }
        logger_log(NU_LOG_INFO,
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
        logger_log(NU_LOG_INFO, "Empty project generated");
    }
    return status;
}
nu_status_t
sdk_compile (sdk_project_t *project)
{
    nu_status_t status = NU_SUCCESS;

    // Prepare compilation context
    if (!project->entries)
    {
        project->entries
            = native_malloc(SDK_INIT_ENTRY_CAPA * sizeof(*project->entries));
        NU_ASSERT(project->entries);
        project->entries_capa = SDK_INIT_ENTRY_CAPA;
    }
    nu_memset(
        project->entries, 0, project->entries_capa * sizeof(*project->entries));
    project->entries_size = 0;
    if (!project->data)
    {
        project->data = native_malloc(SDK_INIT_DATA_CAPA);
        NU_ASSERT(project->data);
        project->data_capa = SDK_INIT_DATA_CAPA;
    }
    nu_memset(project->data, 0, project->data_capa);
    project->data_size = 0;
    project->next_id   = 1;

    // Execute prebuild command
    if (nu_strnlen(project->prebuild, SDK_NAME_MAX))
    {
        logger_log(
            NU_LOG_INFO, "Executing prebuild command '%s'", project->prebuild);
#ifdef NU_PLATFORM_UNIX
        nu_int_t errcode = system(project->prebuild);
        if (errcode)
        {
            logger_log(NU_LOG_ERROR,
                       "Prebuild command '%s' was not executed sucessfully "
                       "(errocode %d)",
                       project->prebuild,
                       errcode);
            return NU_FAILURE;
        }
#endif
    }

    if (project->assets_count == 0)
    {
        logger_log(NU_LOG_WARNING, "Compiling project with no assets");
    }

    // Compile assets
    for (nu_size_t i = 0; i < project->assets_count; ++i)
    {
        sdk_project_asset_t *asset = project->assets + i;

        nu_f32_t percent
            = (nu_f32_t)(i + 1) / (nu_f32_t)project->assets_count * 100.0;
        const nu_char_t *ignored = "";
        if (asset->ignore)
        {
            ignored = "(ignored) ";
        }
        logger_log(NU_LOG_INFO,
                   "[% 3d%][%d] %s%s : %s",
                   (nu_u32_t)percent,
                   i,
                   ignored,
                   nu_enum_to_cstr(asset->type, asset_type_map),
                   asset->source);

        if (asset->ignore)
        {
            continue;
        }

        switch (asset->type)
        {
            case SDK_ASSET_WASM:
                NU_CHECK(sdk_wasm_compile(project, asset), goto cleanup0);
                break;
            case SDK_ASSET_TEXTURE:
                NU_CHECK(sdk_texture_compile(project, asset), goto cleanup0);
                break;
            case SDK_ASSET_MODEL:
                NU_CHECK(sdk_model_compile(project, asset), goto cleanup0);
                break;
        }
    }

    // End last chunk entry
    if (project->current_entry)
    {
        project->current_entry->length
            = project->data_size - project->current_entry->offset;
    }

    // Open cart
    FILE *f = fopen(project->target_path, "wb");
    if (!f)
    {
        logger_log(NU_LOG_ERROR,
                   "Failed to create cartridge file %s",
                   project->target_path);
        return NU_FAILURE;
    }
    else
    {
        logger_log(NU_LOG_INFO, "Compiling cartridge %s", project->target_path);
    }

    // Write header
    const nu_u32_t version = VM_VERSION;
    NU_CHECK(fwrite(&version, sizeof(version), 1, f) == 1, goto cleanup1);
    NU_CHECK(fwrite(&project->entries_size, sizeof(project->entries_size), 1, f)
                 == 1,
             goto cleanup1);

    // Write chunk table
    for (nu_size_t i = 0; i < project->entries_size; ++i)
    {
        const cart_chunk_entry_t *entry = project->entries + i;
        nu_u32_t                  type  = entry->type;
        NU_CHECK(fwrite(&type, sizeof(type), 1, f) == 1, return NU_FAILURE);
        NU_CHECK(fwrite(&entry->id, sizeof(entry->id), 1, f) == 1,
                 return NU_FAILURE);
        NU_CHECK(fwrite(&entry->offset, sizeof(entry->offset), 1, f) == 1,
                 return NU_FAILURE);
        NU_CHECK(fwrite(&entry->length, sizeof(entry->length), 1, f) == 1,
                 return NU_FAILURE);
    }

    // Write data
    NU_CHECK(fwrite(project->data, project->data_size, 1, f) == 1,
             goto cleanup1);

    logger_log(NU_LOG_INFO, "Compilation success");

cleanup1:
    fclose(f);

cleanup0:
    return status;
}
nu_status_t
sdk_project_load (sdk_project_t *proj, nu_sv_t path)
{
    nu_status_t status = NU_SUCCESS;

    // Parse file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    logger_log(NU_LOG_INFO, "Loading project file %s", json_path);
    JSON_Value *jrootv = json_parse_file(json_path);
    if (!jrootv)
    {
        logger_log(NU_LOG_ERROR, "Failed to parse project file %s", json_path);
        return NU_FAILURE;
    }

    project_init(proj, path);

    JSON_Object *jroot = json_object(jrootv);
    if (!jroot)
    {
        logger_log(NU_LOG_ERROR, "Project json root is not an object");
        status = NU_FAILURE;
        goto cleanup0;
    }

    // Assets
    JSON_Object *jassets = json_object_get_object(jroot, PROJECT_ASSETS);
    if (jassets)
    {
        proj->assets_count = json_object_get_count(jassets);
        if (proj->assets_count)
        {
            proj->assets
                = native_malloc(sizeof(*proj->assets) * proj->assets_count);
            NU_ASSERT(proj->assets);
            nu_memset(
                proj->assets, 0, sizeof(*proj->assets) * proj->assets_count);
        }
        for (nu_size_t i = 0; i < proj->assets_count; ++i)
        {
            sdk_project_asset_t *asset   = proj->assets + i;
            JSON_Value          *jassetv = json_object_get_value_at(jassets, i);
            JSON_Object         *jasset  = json_object(jassetv);
            if (!jasset)
            {
                logger_log(
                    NU_LOG_ERROR, "Unexpected asset object type at %d", i);
                continue;
            }
            const nu_char_t *name_string = json_object_get_name(jassets, i);
            NU_ASSERT(name_string);

            // Parse type
            const nu_char_t *type_string
                = json_object_get_string(jasset, PROJECT_ASSET_TYPE);
            NU_ASSERT(type_string);
            nu_bool_t        found;
            sdk_asset_type_t type = nu_sv_to_enum(
                nu_sv(type_string, SDK_NAME_MAX), asset_type_map, &found);
            if (!found)
            {
                logger_log(NU_LOG_ERROR, "Invalid asset type %s", type_string);
                status = NU_FAILURE;
                goto cleanup0;
            }
            proj->assets[i].type = type;

            // Parse name
            nu_strncpy(asset->name, name_string, SDK_NAME_MAX);

            // Parse id
            proj->assets[i].id
                = json_object_get_number(jasset, PROJECT_ASSET_ID);
            if (proj->assets[i].id == 0
                || proj->assets[i].id > SYS_MAX_RESOURCE_COUNT)
            {
                logger_log(NU_LOG_ERROR,
                           "Invalid or missing asset id for '%s'",
                           proj->assets[i].name);
                status = NU_FAILURE;
                goto cleanup0;
            }

            // Parse source
            const nu_char_t *source_string
                = json_object_get_string(jasset, PROJECT_ASSET_SOURCE);
            NU_ASSERT(source_string);
            nu_strncpy(asset->source, source_string, NU_PATH_MAX);

            // Parse ignore
            int ignore = json_object_get_boolean(jasset, PROJECT_ASSET_IGNORE);
            if (ignore != -1)
            {
                asset->ignore = ignore;
            }

            // Parse asset
            switch (type)
            {
                case SDK_ASSET_WASM:
                    NU_CHECK(sdk_wasm_load(asset, jasset), goto cleanup0);
                    break;
                case SDK_ASSET_TEXTURE:
                    NU_CHECK(sdk_texture_load(asset, jasset), goto cleanup0);
                    break;
                case SDK_ASSET_MODEL:
                    NU_CHECK(sdk_model_load(asset, jasset), goto cleanup0);
                    break;
            }
        }
    }

    // Check id coherency
    for (nu_u32_t i = 0; i < proj->assets_count; ++i)
    {
        for (nu_u32_t j = i + 1; j < proj->assets_count; ++j)
        {
            if (proj->assets[i].id == proj->assets[j].id)
            {
                logger_log(NU_LOG_ERROR,
                           "Conflict between asset id %u of '%s' and '%s'",
                           proj->assets[i].id,
                           proj->assets[i].name,
                           proj->assets[j].name);
                status = NU_FAILURE;
                goto cleanup0;
            }
        }
    }

    // Prebuild
    const nu_char_t *jprebuild
        = json_object_get_string(jroot, PROJECT_PREBUILD);
    if (jprebuild)
    {
        nu_strncpy(proj->prebuild, jprebuild, SDK_NAME_MAX);
    }

    json_value_free(jrootv);
    return NU_SUCCESS;

cleanup0:
    json_value_free(jrootv);
    sdk_project_free(proj);
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

    // Assets
    if (nu_strnlen(project->target_path, NU_PATH_MAX))
    {
        JSON_Value *jassetsv = json_value_init_object();
        NU_CHECK(jassetsv, goto cleanup1);
        json_object_set_value(jroot, PROJECT_ASSETS, jassetsv);
        JSON_Object *jassets = json_object(jassetsv);

        for (nu_size_t i = 0; i < project->assets_count; ++i)
        {
            sdk_project_asset_t *asset = project->assets + i;

            JSON_Value *jassetv = json_value_init_object();
            NU_CHECK(jassetv, goto cleanup1);
            json_object_set_value(jassets, asset->name, jassetv);
            JSON_Object *jasset = json_object(jassetv);

            // Type
            const nu_char_t *type_str
                = nu_enum_to_cstr(asset->type, asset_type_map);
            json_object_set_string(jasset, PROJECT_ASSET_TYPE, type_str);

            // Id
            json_object_set_number(jasset, PROJECT_ASSET_ID, asset->id);

            // Source
            json_object_set_string(
                jasset, PROJECT_ASSET_SOURCE, project->assets[i].source);

            // Ignore
            json_object_set_boolean(
                jasset, PROJECT_ASSET_IGNORE, project->assets[i].ignore);

            // Save asset
            switch (asset->type)
            {
                case SDK_ASSET_WASM:
                    NU_CHECK(sdk_wasm_save(asset, jasset), goto cleanup1);
                    break;
                case SDK_ASSET_TEXTURE:
                    NU_CHECK(sdk_texture_save(asset, jasset), goto cleanup1);
                    break;
                case SDK_ASSET_MODEL:
                    NU_CHECK(sdk_model_save(asset, jasset), goto cleanup1);
                    break;
            }
        }
    }

    // Prebuild
    if (nu_strnlen(project->prebuild, NU_PATH_MAX))
    {
        json_object_set_string(jroot, PROJECT_PREBUILD, project->prebuild);
    }

    // Write json file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    if (json_serialize_to_file_pretty(jrootv, json_path))
    {
        logger_log(
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
    if (project->assets)
    {
        native_free(project->assets);
    }
    if (project->entries)
    {
        native_free(project->entries);
    }
    if (project->data)
    {
        native_free(project->data);
    }
}

cart_chunk_entry_t *
sdk_begin_entry (sdk_project_t *proj, nu_u32_t id, resource_type_t type)
{
    if (proj->current_entry)
    {
        proj->current_entry->length
            = proj->data_size - proj->current_entry->offset;
        // logger_log(NU_LOG_INFO,
        //         "[END ENTRY %s length %d]",
        //         nu_enum_to_cstr(proj->current_entry->type,
        //         cart_chunk_type_map), proj->current_entry->length);
        proj->current_entry = NU_NULL;
    }
    NU_ASSERT(proj->entries);
    if (proj->entries_size >= proj->entries_capa)
    {
        proj->entries_capa *= 2;
        proj->entries = realloc(proj->entries,
                                proj->entries_capa * sizeof(*proj->entries));
        NU_ASSERT(proj->entries);
    }
    cart_chunk_entry_t *entry = proj->entries + proj->entries_size;
    ++proj->entries_size;
    proj->current_entry         = entry;
    proj->current_entry->type   = type;
    proj->current_entry->id     = id;
    proj->current_entry->offset = proj->data_size;
    proj->current_entry->length = 0;
    // logger_log(NU_LOG_INFO,
    //         "[BEGIN ENTRY %s offset %d]",
    //         nu_enum_to_cstr(entry->type, cart_chunk_type_map),
    //         proj->current_entry->offset);
    return entry;
}
nu_u32_t
sdk_next_id (sdk_project_t *proj)
{
    nu_u32_t  id    = 0;
    nu_bool_t found = NU_TRUE;
    while (found)
    {
        id    = proj->next_id++;
        found = NU_FALSE;
        for (nu_u32_t i = 0; i < proj->assets_count; ++i)
        {
            if (proj->assets[i].id == id)
            {
                found = NU_TRUE;
                break;
            }
        }
    }
    return id;
}
typedef struct
{
    nu_u32_t           index;
    cart_chunk_entry_t data;
} indexed_entry_t;
static int
chunk_entry_cmp (const void *a, const void *b)
{
    const indexed_entry_t *ea = a;
    const indexed_entry_t *eb = b;
    return eb->data.length - ea->data.length;
}
nu_status_t
sdk_dump (nu_sv_t path, nu_bool_t sort, nu_bool_t display_table, nu_u32_t num)
{
    nu_size_t   size;
    nu_status_t status = nu_load_bytes(path, NU_NULL, &size);
    if (!status)
    {
        logger_log(NU_LOG_ERROR, "Failed to load " NU_SV_FMT, NU_SV_ARGS(path));
        return NU_FAILURE;
    }

    nu_byte_t *data = native_malloc(size);
    NU_ASSERT(data);
    status = nu_load_bytes(path, data, &size);
    NU_CHECK(status, goto cleanup0);

    cart_header_t header;
    status = cart_parse_header(data, &header);
    NU_CHECK(status, goto cleanup0);

    indexed_entry_t *entries
        = native_malloc(header.chunk_count * sizeof(*entries));
    NU_ASSERT(entries);
    nu_byte_t *entry_data = data + CART_HEADER_SIZE;
    for (nu_size_t i = 0; i < header.chunk_count; ++i)
    {
        NU_CHECK(cart_parse_entries(entry_data + i * CART_CHUNK_ENTRY_SIZE,
                                    1,
                                    &entries[i].data),
                 goto cleanup1);
        entries[i].index = i;
    }

    if (sort)
    {
        qsort(entries, header.chunk_count, sizeof(*entries), chunk_entry_cmp);
    }

    nu_u32_t total_chunk_size   = 0;
    nu_u32_t total_texture_size = 0;
    nu_u32_t total_mesh_size    = 0;
    nu_u32_t chunk_sizes[5]     = { 0 };
    for (nu_size_t i = 0; i < header.chunk_count; ++i)
    {
        const cart_chunk_entry_t *entry = &entries[i].data;
        total_chunk_size += entry->length;
        chunk_sizes[entry->type] += entry->length;
    }

    printf("Cartridge info:\n\n");
    printf("     %-18s %d.%d.%d\n",
           "Version",
           VM_VERSION_MAJOR(header.version),
           VM_VERSION_MINOR(header.version),
           VM_VERSION_PATCH(header.version));
    printf("     %-18s %-8d\n", "Chunk count", header.chunk_count);
    printf("     %-18s %-8d bytes\n", "Total size", (nu_u32_t)size);
    printf("     %-18s %-8d bytes\n", "Total chunk size", total_chunk_size);
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(chunk_sizes); ++i)
    {
        if (chunk_sizes[i])
        {
            nu_char_t buf[32];
            snprintf(buf,
                     sizeof(buf),
                     "Total %s size",
                     nu_enum_to_cstr(i, resource_enum_map()));
            printf("     %-18s %-8d bytes (%.2lf%%)\n",
                   buf,
                   chunk_sizes[i],
                   ((nu_f32_t)chunk_sizes[i] / (nu_f32_t)total_chunk_size)
                       * 100);
        }
    }
    printf("\n");

    if (display_table)
    {
        printf("Chunk table:\n\n");
        printf("     %-5s %-8s %-10s %-8s %-8s\n",
               "Id",
               "Type",
               "Offset",
               "Length",
               "Usage");

        printf("   --------------------------------------------\n");
        nu_u32_t display_entry = num ? num : (nu_u32_t)-1;
        for (nu_size_t i = 0; i < header.chunk_count && i < display_entry; ++i)
        {
            const indexed_entry_t *entry = entries + i;
            nu_char_t              buf[32];
            snprintf(buf,
                     sizeof(buf),
                     "%.2lf%%",
                     ((nu_f32_t)entry->data.length / (nu_f32_t)total_chunk_size)
                         * 100);
            printf("     %-5u %-8s 0x%-8x %-8d %-8s\n",
                   entry->data.id,
                   nu_enum_to_cstr(entry->data.type, resource_enum_map()),
                   entry->data.offset,
                   entry->data.length,
                   buf);
        }
    }

cleanup1:
    native_free(entries);
cleanup0:
    native_free(data);
    return status;
}
