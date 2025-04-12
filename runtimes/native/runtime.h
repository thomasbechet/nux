#ifndef RUNTIME_H
#define RUNTIME_H

#include <nux.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>
#include <glad/gl.h>
#define RGFW_EXPORT
#include <rgfw/RGFW.h>
#define NU_STDLIB
#include <nulib/nulib.h>

typedef void (*runtime_log_callback_t)(nu_log_level_t   level,
                                       const nu_char_t *fmt,
                                       va_list          args);

typedef struct
{
    nu_char_t          name[32];
    nux_u32_t          addr;
    nux_inspect_type_t type;
    nu_bool_t          override;
    union
    {
        nu_f32_t f32;
        nu_i32_t i32;
    } value;
} inspect_value_t;

typedef enum
{
    COMMAND_EXIT,
    COMMAND_SAVE_STATE,
    COMMAND_LOAD_STATE,
} runtime_command_t;

typedef enum
{
    VIEWPORT_HIDDEN,
    VIEWPORT_FIXED,
    VIEWPORT_FIXED_BEST_FIT,
    VIEWPORT_STRETCH_KEEP_ASPECT,
    VIEWPORT_STRETCH,
} viewport_mode_t;

typedef struct
{
    nu_char_t             path[NU_PATH_MAX];
    nu_bool_t             active;
    nux_instance_config_t config;
    nux_instance_t        instance;
    nu_byte_t            *save_state;
    nu_bool_t             pause;
    struct nk_rect        viewport;
    viewport_mode_t       viewport_mode;
    inspect_value_t       inspect_values[256];
    nu_size_t             inspect_value_count;
} runtime_instance_t;

typedef struct
{
    nu_bool_t debug;
    nu_sv_t   path;
} runtime_config_t;

NU_API nu_status_t         runtime_run(const runtime_config_t *config);
NU_API nu_status_t         runtime_open(nu_u32_t index, nu_sv_t path);
NU_API void                runtime_close(nu_u32_t index);
NU_API void                runtime_reset(nu_u32_t index);
NU_API runtime_instance_t *runtime_instance(void);
NU_API void                runtime_quit(void);
NU_API void runtime_set_logger_callback(runtime_log_callback_t callback);

#endif
