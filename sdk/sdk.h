#ifndef SDK_H
#define SDK_H

#include <native/core/vm.h>
#include <parson/parson.h>

#define SDK_NAME_MAX        256
#define SDK_INIT_DATA_CAPA  NU_MEM_128M
#define SDK_INIT_ENTRY_CAPA 1024

typedef enum
{
    SDK_ASSET_WASM,
    SDK_ASSET_TEXTURE,
    SDK_ASSET_MODEL,
} sdk_asset_type_t;

typedef struct
{
    sdk_asset_type_t type;
    nu_char_t        name[SDK_NAME_MAX];
    nu_char_t        source[NU_PATH_MAX];
    nu_bool_t        ignore;

    union
    {
        struct
        {
            nu_u32_t target_size;
        } image;
    };
} sdk_project_asset_t;

typedef struct
{
    // Project state
    nu_char_t            target_path[NU_PATH_MAX];
    sdk_project_asset_t *assets;
    nu_size_t            assets_count;
    nu_char_t            prebuild[SDK_NAME_MAX];
    nu_pcg_t             pcg;

    // Compilation state
    cart_chunk_entry_t *entries;
    nu_u32_t            entries_capa;
    nu_u32_t            entries_size;
    nu_byte_t          *data;
    nu_u32_t            data_capa;
    nu_u32_t            data_size;
    cart_chunk_entry_t *current_entry;
} sdk_project_t;

typedef void (*sdk_log_callback_t)(nu_log_level_t   level,
                                   const nu_char_t *fmt,
                                   va_list          args);

NU_API void  sdk_set_log_callback(sdk_log_callback_t callback);
NU_API void  sdk_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);
NU_API void  sdk_log(nu_log_level_t level, const nu_char_t *fmt, ...);
NU_API void *sdk_malloc(nu_size_t n);
NU_API void  sdk_free(void *p);

NU_API nu_status_t sdk_generate_template(nu_sv_t path, nu_sv_t lang);
NU_API nu_status_t sdk_compile(sdk_project_t *project);
NU_API nu_status_t sdk_dump(nu_sv_t   path,
                            nu_bool_t sort,
                            nu_bool_t display_table,
                            nu_u32_t  num);
NU_API nu_status_t sdk_project_load(sdk_project_t *project, nu_sv_t path);
NU_API nu_status_t sdk_project_save(const sdk_project_t *project, nu_sv_t path);
NU_API void        sdk_project_free(sdk_project_t *project);

cart_chunk_entry_t *sdk_begin_entry(sdk_project_t    *proj,
                                    const nu_char_t  *name,
                                    cart_chunk_type_t type);

nu_status_t sdk_wasm_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_wasm_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_wasm_compile(sdk_project_t       *project,
                             sdk_project_asset_t *asset);

nu_status_t sdk_image_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_image_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_image_compile(sdk_project_t       *project,
                              sdk_project_asset_t *asset);

nu_status_t sdk_model_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_model_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_model_compile(sdk_project_t       *project,
                              sdk_project_asset_t *asset);

nu_status_t        json_parse_f32(const JSON_Object *object,
                                  const nu_char_t   *name,
                                  nu_f32_t          *v);
nu_status_t        json_write_f32(JSON_Object     *object,
                                  const nu_char_t *name,
                                  nu_f32_t         value);
nu_status_t        json_parse_u32(const JSON_Object *object,
                                  const nu_char_t   *name,
                                  nu_u32_t          *v);
NU_API nu_status_t json_write_u32(JSON_Object     *object,
                                  const nu_char_t *name,
                                  nu_u32_t         value);

nu_status_t cart_write(sdk_project_t *proj, const void *p, nu_size_t n);
nu_status_t cart_write_u32(sdk_project_t *proj, nu_u32_t v);
nu_status_t cart_write_f32(sdk_project_t *proj, nu_f32_t v);
nu_status_t cart_write_v2(sdk_project_t *proj, nu_v2_t v);
nu_status_t cart_write_v3(sdk_project_t *proj, nu_v3_t v);
nu_status_t cart_write_m4(sdk_project_t *proj, nu_m4_t v);
nu_status_t cart_write_texture(sdk_project_t   *proj,
                               const nu_char_t *name,
                               nu_u32_t         size,
                               const nu_byte_t *data);

nu_status_t image_resize(nu_v2u_t         source_size,
                         const nu_byte_t *source_data,
                         nu_u32_t         target_size,
                         nu_byte_t       *target_data);

#endif
