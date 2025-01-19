#ifndef SDK_H
#define SDK_H

#include <nulib.h>
#include <native/core/vm.h>

#define SDK_NAME_MAX 256

typedef struct
{
    vm_chunk_header_t header;
    nu_char_t         source_path[NU_PATH_MAX];
} sdk_project_chunk_t;

typedef struct
{
    nu_char_t            target_path[NU_PATH_MAX];
    sdk_project_chunk_t *chunks;
    nu_size_t            chunks_count;
    nu_char_t            prebuild[SDK_NAME_MAX];
} sdk_project_t;

typedef void (*sdk_log_callback_t)(nu_log_level_t   level,
                                   const nu_char_t *fmt,
                                   va_list          args);

NU_API void sdk_set_log_callback(sdk_log_callback_t callback);
NU_API void sdk_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);
NU_API void sdk_log(nu_log_level_t level, const nu_char_t *fmt, ...);

NU_API nu_status_t sdk_generate_template(nu_sv_t path, nu_sv_t lang);
NU_API nu_status_t sdk_build(const sdk_project_t *project);
NU_API nu_status_t sdk_project_load(sdk_project_t *project, nu_sv_t path);
NU_API nu_status_t sdk_project_save(const sdk_project_t *project, nu_sv_t path);
NU_API void        sdk_project_free(sdk_project_t *project);

NU_API nu_byte_t *sdk_load_image(nu_sv_t path, nu_v2u_t *size);
NU_API nu_f32_t  *sdk_load_gltf(nu_sv_t path, nu_size_t *size);

#endif
