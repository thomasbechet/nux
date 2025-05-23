#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include <nux.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>
#define NU_STDLIB
#include <nulib/nulib.h>

typedef enum
{
    VIEWPORT_HIDDEN,
    VIEWPORT_FIXED,
    VIEWPORT_FIXED_BEST_FIT,
    VIEWPORT_STRETCH_KEEP_ASPECT,
    VIEWPORT_STRETCH,
} viewport_mode_t;

typedef enum
{
    COMMAND_EXIT,
    COMMAND_SAVE_STATE,
    COMMAND_LOAD_STATE,
} runtime_command_t;

typedef struct
{
    nu_char_t        name[32];
    nux_u32_t        addr;
    nux_debug_type_t type;
    nu_bool_t        override;
    union
    {
        nu_f32_t f32;
        nu_i32_t i32;
    } value;
} debug_value_t;

typedef struct
{
    nu_char_t             path[NU_PATH_MAX];
    nu_bool_t             active;
    nux_instance_config_t config;
    nux_instance_t       *instance;
    nu_bool_t             pause;
    struct nk_rect        viewport;
    viewport_mode_t       viewport_mode;
    debug_value_t         debug_values[256];
    nu_size_t             debug_value_count;
} runtime_instance_t;

typedef struct
{
    nu_bool_t debug;
    nu_sv_t   path;
} runtime_config_t;

void *native_malloc(nu_size_t n);
void  native_free(void *p);
void *native_realloc(void *p, nu_size_t n);

void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);

nu_status_t         runtime_run(const runtime_config_t *config);
nu_status_t         runtime_open(nu_u32_t index, nu_sv_t path);
void                runtime_close(nu_u32_t index);
void                runtime_reset(nu_u32_t index);
runtime_instance_t *runtime_instance(void);
void                runtime_quit(void);

nu_status_t renderer_init(void);
void        renderer_free(void);
void        renderer_clear(nu_b2i_t viewport, nu_v2u_t window_size);
void        renderer_render_instance(nux_instance_t *inst,
                                     nu_b2i_t        viewport,
                                     nu_v2u_t        window_size);

nu_status_t        window_init(void);
void               window_free(void);
void               window_begin_frame(void);
nu_u32_t           window_end_frame(void);
nu_v2u_t           window_get_size(void);
struct nk_context *window_nk_context(void);
nu_bool_t          window_is_double_click(void);
nu_bool_t          window_poll_command(runtime_command_t *cmd);
void               window_set_instance_inputs(nux_instance_t *inst);

void view_home(struct nk_context *ctx, struct nk_rect bounds);
void view_controls(struct nk_context *ctx, struct nk_rect bounds);
void view_settings(struct nk_context *ctx, struct nk_rect bounds);
void view_debug(struct nk_context *ctx, struct nk_rect bounds);

void gui_update(void);

void loop_init(nux_env_t env);
void loop_update(nux_env_t env);

void load_blk_colormap(nux_env_t env);
void draw_red(nux_env_t env, const nux_f32_t *m, nux_u32_t wireframe);
void draw_krabe(nux_env_t env, const nux_f32_t *m, nux_u32_t wireframe);
void draw_lavalamp(nux_env_t env, const nux_f32_t *m, nux_u32_t wireframe);

#endif
