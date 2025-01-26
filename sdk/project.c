#include "sdk.h"

#include "templates_data.h"

#define PROJECT_CART_DEFAULT "cart.bin"
#define PROJECT_JSON         "nux.json"
#define PROJECT_TARGET       "target"
#define PROJECT_PREBUILD     "prebuild"
#define PROJECT_ASSETS       "assets"
#define PROJECT_ASSET_TYPE   "type"
#define PROJECT_ASSET_SOURCE "source"

static NU_ENUM_MAP(cart_chunk_type_map,
                   NU_ENUM_NAME(CART_CHUNK_RAW, "raw"),
                   NU_ENUM_NAME(CART_CHUNK_MESH, "mesh"),
                   NU_ENUM_NAME(CART_CHUNK_WASM, "wasm"),
                   NU_ENUM_NAME(CART_CHUNK_TEXTURE, "texture"),
                   NU_ENUM_NAME(CART_CHUNK_MODEL, "model"));

static NU_ENUM_MAP(asset_type_map,
                   NU_ENUM_NAME(SDK_ASSET_WASM, "wasm"),
                   NU_ENUM_NAME(SDK_ASSET_IMAGE, "image"));

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
sdk_compile (const sdk_project_t *project)
{
    nu_status_t status = NU_SUCCESS;

    // Execute prebuild command
    if (nu_strlen(project->prebuild))
    {
        sdk_log(
            NU_LOG_INFO, "Executing prebuild command : %s", project->prebuild);
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

    if (project->assets_count == 0)
    {
        sdk_log(NU_LOG_WARNING, "Compiling project with no assets");
    }

    // Write header
    const nu_u32_t version = 100;
    NU_CHECK(cart_write_u32(f, version), goto cleanup0);
    NU_CHECK(cart_write_u32(f, project->assets_count), goto cleanup0);

    // Compile assets
    for (nu_size_t i = 0; i < project->assets_count; ++i)
    {
        sdk_project_asset_t *asset = project->assets + i;

        nu_f32_t percent
            = (nu_f32_t)i / (nu_f32_t)project->assets_count * 100.0;
        sdk_log(NU_LOG_INFO,
                "[% 3d%][%d] %s : %s",
                (nu_u32_t)percent,
                i,
                nu_enum_to_cstr(asset->type, asset_type_map),
                asset->source_path);

        switch (asset->type)
        {
            case SDK_ASSET_WASM:
                NU_CHECK(sdk_wasm_compile(asset, f), goto cleanup0);
                break;
            case SDK_ASSET_IMAGE:
                NU_CHECK(sdk_image_compile(asset, f), goto cleanup0);
                break;
        }
    }

    sdk_log(NU_LOG_INFO, "Compilation success");

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
                nu_sv_cstr(source_string), asset->source_path, NU_PATH_MAX);

            // Parse asset
            switch (type)
            {
                case SDK_ASSET_WASM:
                    NU_CHECK(sdk_wasm_load(asset, jasset), goto cleanup0);
                    break;
                case SDK_ASSET_IMAGE:
                    NU_CHECK(sdk_image_load(asset, jasset), goto cleanup0);
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
                jasset, PROJECT_ASSET_SOURCE, project->assets[i].source_path);

            // Save asset
            switch (asset->type)
            {
                case SDK_ASSET_WASM:
                    NU_CHECK(sdk_wasm_save(asset, jasset), goto cleanup1);
                    break;
                case SDK_ASSET_IMAGE:
                    NU_CHECK(sdk_image_save(asset, jasset), goto cleanup1);
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
        NU_ASSERT(project->assets_count);
        free(project->assets);
    }
}
