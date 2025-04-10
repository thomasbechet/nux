#ifndef SDK_H
#define SDK_H

#include <nux.h>
#define NU_STDLIB
#include <nulib/nulib.h>

#define SDK_NAME_MAX        256
#define SDK_INIT_DATA_CAPA  NU_MEM_128M
#define SDK_INIT_ENTRY_CAPA 1024

typedef enum
{
    SDK_ASSET_WASM,
    SDK_ASSET_TEXTURE,
    SDK_ASSET_SCENE,
} sdk_asset_type_t;

typedef struct
{
    sdk_asset_type_t type;
    nux_id_t         id;
    nu_char_t        name[SDK_NAME_MAX];
    nu_char_t        source[NU_PATH_MAX];
    nu_bool_t        ignore;
    union
    {
        struct
        {
            nu_u32_t target_size;
        } texture;
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
    nux_cart_entry_t *entries;
    nu_u32_t          entries_capa;
    nu_u32_t          entries_size;
    nu_byte_t        *data;
    nu_u32_t          data_capa;
    nu_u32_t          data_size;
    nux_cart_entry_t *current_entry;
    nu_u32_t          next_id;
} sdk_project_t;

NU_API nu_status_t sdk_generate_template(nu_sv_t path, nu_sv_t lang);
NU_API nu_status_t sdk_compile(sdk_project_t *project);
NU_API nu_status_t sdk_dump(nu_sv_t   path,
                            nu_bool_t sort,
                            nu_bool_t display_table,
                            nu_u32_t  num);
NU_API nu_status_t sdk_project_load(sdk_project_t *project, nu_sv_t path);
NU_API nu_status_t sdk_project_save(const sdk_project_t *project, nu_sv_t path);
NU_API void        sdk_project_free(sdk_project_t *project);

#endif
