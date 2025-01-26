#ifndef SDK_H
#define SDK_H

#include <native/core/vm.h>
#include <parson/parson.h>

#define SDK_NAME_MAX 256

typedef enum
{
    SDK_ASSET_WASM,
    SDK_ASSET_IMAGE,
    SDK_ASSET_MODEL,
} sdk_asset_type_t;

typedef struct
{
    sdk_asset_type_t type;
    nu_char_t        source_path[NU_PATH_MAX];
    nu_bool_t        ignore;
    union
    {
        struct
        {
            nu_u32_t           target_index;
            gpu_texture_size_t target_size;
        } image;
        struct
        {
            nu_u32_t target_index;
        } model;
    };
} sdk_project_asset_t;

typedef struct
{
    nu_char_t            target_path[NU_PATH_MAX];
    sdk_project_asset_t *assets;
    nu_size_t            assets_count;
    nu_char_t            prebuild[SDK_NAME_MAX];
} sdk_project_t;

typedef void (*sdk_log_callback_t)(nu_log_level_t   level,
                                   const nu_char_t *fmt,
                                   va_list          args);

NU_API void sdk_set_log_callback(sdk_log_callback_t callback);
NU_API void sdk_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);
NU_API void sdk_log(nu_log_level_t level, const nu_char_t *fmt, ...);

NU_API nu_status_t sdk_generate_template(nu_sv_t path, nu_sv_t lang);
NU_API nu_status_t sdk_compile(const sdk_project_t *project);
NU_API nu_status_t sdk_project_load(sdk_project_t *project, nu_sv_t path);
NU_API nu_status_t sdk_project_save(const sdk_project_t *project, nu_sv_t path);
NU_API void        sdk_project_free(sdk_project_t *project);

NU_API nu_status_t sdk_wasm_load(sdk_project_asset_t *asset,
                                 JSON_Object         *jasset);
NU_API nu_status_t sdk_wasm_save(sdk_project_asset_t *asset,
                                 JSON_Object         *jasset);
NU_API nu_status_t sdk_wasm_compile(sdk_project_asset_t *asset, FILE *f);

NU_API nu_status_t sdk_image_load(sdk_project_asset_t *asset,
                                  JSON_Object         *jasset);
NU_API nu_status_t sdk_image_save(sdk_project_asset_t *asset,
                                  JSON_Object         *jasset);
NU_API nu_status_t sdk_image_compile(sdk_project_asset_t *asset, FILE *f);

NU_API nu_status_t sdk_model_load(sdk_project_asset_t *asset,
                                  JSON_Object         *jasset);
NU_API nu_status_t sdk_model_save(sdk_project_asset_t *asset,
                                  JSON_Object         *jasset);
NU_API nu_status_t sdk_model_compile(sdk_project_asset_t *asset, FILE *f);

NU_API nu_status_t json_parse_f32(const JSON_Object *object,
                                  const nu_char_t   *name,
                                  nu_f32_t          *v);
NU_API nu_status_t json_write_f32(JSON_Object     *object,
                                  const nu_char_t *name,
                                  nu_f32_t         value);
NU_API nu_status_t json_parse_u32(const JSON_Object *object,
                                  const nu_char_t   *name,
                                  nu_u32_t          *v);
NU_API nu_status_t json_write_u32(JSON_Object     *object,
                                  const nu_char_t *name,
                                  nu_u32_t         value);

NU_API nu_status_t cart_write(FILE *f, void *p, nu_size_t n);
NU_API nu_status_t cart_write_u32(FILE *f, nu_u32_t v);
NU_API nu_status_t cart_write_m4(FILE *f, nu_m4_t v);
NU_API nu_status_t cart_write_chunk_header(FILE             *f,
                                           cart_chunk_type_t type,
                                           nu_u32_t          length);

#endif
