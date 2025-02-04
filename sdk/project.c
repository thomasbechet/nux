#include "native/core/cartridge.h"
#include "sdk.h"

#include "templates_data.h"
#include <stdio.h>

#define PROJECT_CART_DEFAULT "cart.bin"
#define PROJECT_JSON         "nux.json"
#define PROJECT_TARGET       "target"
#define PROJECT_PREBUILD     "prebuild"
#define PROJECT_ASSETS       "assets"
#define PROJECT_ASSET_TYPE   "type"
#define PROJECT_ASSET_SOURCE "source"
#define PROJECT_ASSET_IGNORE "ignore"

static NU_ENUM_MAP(cart_chunk_type_map,
                   NU_ENUM_NAME(CART_CHUNK_RAW, "raw"),
                   NU_ENUM_NAME(CART_CHUNK_MESH, "mesh"),
                   NU_ENUM_NAME(CART_CHUNK_WASM, "wasm"),
                   NU_ENUM_NAME(CART_CHUNK_TEXTURE, "texture"),
                   NU_ENUM_NAME(CART_CHUNK_MODEL, "model"));

static NU_ENUM_MAP(asset_type_map,
                   NU_ENUM_NAME(SDK_ASSET_WASM, "wasm"),
                   NU_ENUM_NAME(SDK_ASSET_IMAGE, "image"),
                   NU_ENUM_NAME(SDK_ASSET_MODEL, "model"));

static void
project_init (sdk_project_t *project, nu_sv_t path)
{
    nu_memset(project, 0, sizeof(*project));
    nu_path_concat(
        project->target_path, NU_PATH_MAX, path, NU_SV(PROJECT_CART_DEFAULT));
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
sdk_compile (sdk_project_t *project)
{
    nu_status_t status = NU_SUCCESS;

    // Prepare compilation context
    project->next_mesh_index    = 0;
    project->next_texture_index = 0;
    project->next_model_index   = 0;
    if (!project->entries)
    {
        project->entries
            = malloc(SDK_INIT_ENTRY_CAPA * sizeof(*project->entries));
        NU_ASSERT(project->entries);
        project->entries_capa = SDK_INIT_ENTRY_CAPA;
    }
    nu_memset(
        project->entries, 0, project->entries_capa * sizeof(*project->entries));
    project->entries_size = 0;
    if (!project->data)
    {
        project->data = malloc(SDK_INIT_DATA_CAPA);
        NU_ASSERT(project->data);
        project->data_capa = SDK_INIT_DATA_CAPA;
    }
    nu_memset(project->data, 0, project->data_capa);
    project->data_size = 0;

    // Execute prebuild command
    if (nu_strlen(project->prebuild))
    {
        sdk_log(
            NU_LOG_INFO, "Executing prebuild command '%s'", project->prebuild);
#ifdef NU_PLATFORM_UNIX
        nu_int_t errcode = system(project->prebuild);
        if (errcode)
        {
            sdk_log(NU_LOG_ERROR,
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
        sdk_log(NU_LOG_WARNING, "Compiling project with no assets");
    }

    // Compile assets
    for (nu_size_t i = 0; i < project->assets_count; ++i)
    {
        sdk_project_asset_t *asset = project->assets + i;

        nu_f32_t percent
            = (nu_f32_t)i / (nu_f32_t)project->assets_count * 100.0;
        const nu_char_t *ignored = "";
        if (asset->ignore)
        {
            ignored = "(ignored) ";
        }
        sdk_log(NU_LOG_INFO,
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
            case SDK_ASSET_IMAGE:
                NU_CHECK(sdk_image_compile(project, asset), goto cleanup0);
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
        sdk_log(NU_LOG_ERROR,
                "Failed to create cartridge file %s",
                project->target_path);
        return NU_FAILURE;
    }
    else
    {
        sdk_log(NU_LOG_INFO, "Compiling cartridge %s", project->target_path);
    }

    // Write header
    const nu_u32_t version = 100;
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
        NU_CHECK(fwrite(&entry->offset, sizeof(entry->offset), 1, f) == 1,
                 return NU_FAILURE);
        NU_CHECK(fwrite(&entry->length, sizeof(entry->length), 1, f) == 1,
                 return NU_FAILURE);
        switch (entry->type)
        {
            case CART_CHUNK_RAW:
            case CART_CHUNK_WASM: {
                nu_u32_t index = 0; // Padding
                NU_CHECK(fwrite(&index, sizeof(index), 1, f) == 1,
                         return NU_FAILURE);
            }
            break;
            case CART_CHUNK_TEXTURE: {
                nu_u32_t index = entry->extra.texture.index;
                NU_CHECK(fwrite(&index, sizeof(index), 1, f) == 1,
                         return NU_FAILURE);
            }
            break;
            case CART_CHUNK_MESH: {
                nu_u32_t index = entry->extra.mesh.index;
                NU_CHECK(fwrite(&index, sizeof(index), 1, f) == 1,
                         return NU_FAILURE);
            }
            break;
            case CART_CHUNK_MODEL: {
                nu_u32_t index = entry->extra.model.index;
                NU_CHECK(fwrite(&index, sizeof(index), 1, f) == 1,
                         return NU_FAILURE);
            }
            break;
        }
    }

    // Write data
    NU_CHECK(fwrite(project->data, project->data_size, 1, f) == 1,
             goto cleanup1);

    sdk_log(NU_LOG_INFO, "Compilation success");

cleanup1:
    fclose(f);

cleanup0:
    return status;
}
nu_status_t
sdk_project_load (sdk_project_t *project, nu_sv_t path)
{
    nu_status_t status = NU_SUCCESS;

    // Parse file
    nu_char_t json_path[NU_PATH_MAX];
    nu_path_concat(json_path, NU_PATH_MAX, path, NU_SV(PROJECT_JSON));
    sdk_log(NU_LOG_INFO, "Loading project file %s", json_path);
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

    // Assets
    JSON_Array *jassets = json_object_get_array(jroot, PROJECT_ASSETS);
    if (jassets)
    {
        project->assets_count = json_array_get_count(jassets);
        if (project->assets_count)
        {
            project->assets
                = malloc(sizeof(*project->assets) * project->assets_count);
            NU_ASSERT(project->assets);
            nu_memset(project->assets,
                      0,
                      sizeof(*project->assets) * project->assets_count);
        }
        for (nu_size_t i = 0; i < project->assets_count; ++i)
        {
            sdk_project_asset_t *asset  = project->assets + i;
            JSON_Object         *jasset = json_array_get_object(jassets, i);

            // Parse type and source
            const nu_char_t *type_string
                = json_object_get_string(jasset, PROJECT_ASSET_TYPE);
            NU_ASSERT(type_string);
            const nu_char_t *source_string
                = json_object_get_string(jasset, PROJECT_ASSET_SOURCE);
            NU_ASSERT(source_string);
            int ignore = json_object_get_boolean(jasset, PROJECT_ASSET_IGNORE);
            if (ignore != -1)
            {
                asset->ignore = ignore;
            }

            // Check type
            nu_bool_t        found;
            sdk_asset_type_t type = nu_sv_to_enum(
                nu_sv_cstr(type_string), asset_type_map, &found);
            if (!found)
            {
                sdk_log(NU_LOG_ERROR, "Invalid asset type %s", type_string);
                status = NU_FAILURE;
                goto cleanup0;
            }
            project->assets[i].type = type;
            nu_sv_to_cstr(
                nu_sv_cstr(source_string), asset->source, NU_PATH_MAX);

            // Parse asset
            switch (type)
            {
                case SDK_ASSET_WASM:
                    NU_CHECK(sdk_wasm_load(asset, jasset), goto cleanup0);
                    break;
                case SDK_ASSET_IMAGE:
                    NU_CHECK(sdk_image_load(asset, jasset), goto cleanup0);
                    break;
                case SDK_ASSET_MODEL:
                    NU_CHECK(sdk_model_load(asset, jasset), goto cleanup0);
                    break;
            }
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

    // Assets
    if (nu_strlen(project->target_path))
    {
        JSON_Value *jassetsv = json_value_init_array();
        NU_CHECK(jassetsv, goto cleanup1);
        json_object_set_value(jroot, PROJECT_ASSETS, jassetsv);
        JSON_Array *jassets = json_array(jassetsv);

        for (nu_size_t i = 0; i < project->assets_count; ++i)
        {
            sdk_project_asset_t *asset = project->assets + i;

            JSON_Value *jassetv = json_value_init_object();
            NU_CHECK(jassetv, goto cleanup1);
            json_array_append_value(jassets, jassetv);
            JSON_Object *jasset = json_object(jassetv);

            // Type
            const nu_char_t *type_str
                = nu_enum_to_cstr(asset->type, asset_type_map);
            json_object_set_string(jasset, PROJECT_ASSET_TYPE, type_str);

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
                case SDK_ASSET_IMAGE:
                    NU_CHECK(sdk_image_save(asset, jasset), goto cleanup1);
                    break;
                case SDK_ASSET_MODEL:
                    NU_CHECK(sdk_model_save(asset, jasset), goto cleanup1);
                    break;
            }
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
    if (project->assets)
    {
        free(project->assets);
    }
    if (project->entries)
    {
        free(project->entries);
    }
    if (project->data)
    {
        free(project->data);
    }
}

cart_chunk_entry_t *
sdk_begin_entry (sdk_project_t *proj, cart_chunk_type_t type)
{
    if (proj->current_entry)
    {
        proj->current_entry->length
            = proj->data_size - proj->current_entry->offset;
        // sdk_log(NU_LOG_INFO,
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
    proj->current_entry->offset = proj->data_size;
    proj->current_entry->length = 0;
    // sdk_log(NU_LOG_INFO,
    //         "[BEGIN ENTRY %s offset %d]",
    //         nu_enum_to_cstr(entry->type, cart_chunk_type_map),
    //         proj->current_entry->offset);
    return entry;
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
        sdk_log(NU_LOG_ERROR, "Failed to load " NU_SV_FMT, NU_SV_ARGS(path));
        return NU_FAILURE;
    }

    nu_byte_t *data = malloc(size);
    NU_ASSERT(data);
    status = nu_load_bytes(path, data, &size);
    NU_CHECK(status, goto cleanup0);

    cart_header_t header;
    status = cart_parse_header(data, &header);
    NU_CHECK(status, goto cleanup0);

    indexed_entry_t *entries = malloc(header.chunk_count * sizeof(*entries));
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
    printf("    %-18s %-8d\n", "Version", header.version);
    printf("    %-18s %-8d\n", "Chunk count", header.version);
    printf("    %-18s %-8d bytes\n", "Total size", (nu_u32_t)size);
    printf("    %-18s %-8d bytes\n", "Total chunk size", total_chunk_size);
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(chunk_sizes); ++i)
    {
        nu_char_t buf[32];
        snprintf(buf,
                 sizeof(buf),
                 "Total %s size",
                 nu_enum_to_cstr(i, cart_chunk_type_map));
        printf("    %-18s %-8d bytes (%.2lf%%)\n",
               buf,
               chunk_sizes[i],
               ((nu_f32_t)chunk_sizes[i] / (nu_f32_t)total_chunk_size) * 100);
    }
    printf("\n");

    if (display_table)
    {
        printf("Chunk table:\n\n");
        printf("     %-8s %-8s %-8s %-8s %-8s %-8s\n",
               "Index",
               "Type",
               "Offset",
               "Length",
               "Usage",
               "Extra");
        printf("   ------------------------------------------------------\n");
        nu_u32_t display_entry = num ? num : (nu_u32_t)-1;
        for (nu_size_t i = 0; i < header.chunk_count && i < display_entry; ++i)
        {
            const indexed_entry_t *entry = entries + i;
            nu_u32_t               extra = -1;
            switch (entry->data.type)
            {
                case CART_CHUNK_RAW:
                case CART_CHUNK_WASM:
                    break;
                case CART_CHUNK_TEXTURE:
                    extra = entry->data.extra.texture.index;
                    break;
                case CART_CHUNK_MESH:
                    extra = entry->data.extra.mesh.index;
                    break;
                case CART_CHUNK_MODEL:
                    extra = entry->data.extra.model.index;
                    break;
            }
            nu_char_t buf[32];
            snprintf(buf,
                     sizeof(buf),
                     "%.2lf%%",
                     ((nu_f32_t)entry->data.length / (nu_f32_t)total_chunk_size)
                         * 100);
            printf("     %-8d %-8s %-8x %-8d %-8s %-8d\n",
                   entry->index,
                   nu_enum_to_cstr(entry->data.type, cart_chunk_type_map),
                   entry->data.offset,
                   entry->data.length,
                   buf,
                   extra);
        }
    }

cleanup1:
    free(entries);
cleanup0:
    free(data);
    return status;
}
