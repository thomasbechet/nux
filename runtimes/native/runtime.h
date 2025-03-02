#ifndef RUNTIME_H
#define RUNTIME_H

#include "core/syscall.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>

typedef void (*runtime_log_callback_t)(nu_log_level_t   level,
                                       const nu_char_t *fmt,
                                       va_list          args);

typedef struct
{
    nu_char_t          name[32];
    nu_u32_t           addr;
    sys_inspect_type_t type;
    union
    {
        nu_f32_t f32;
        nu_u32_t u32;
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
    VIEWPORT_FIXED,
    VIEWPORT_FIXED_BEST_FIT,
    VIEWPORT_STRETCH_KEEP_ASPECT,
    VIEWPORT_STRETCH,
} viewport_mode_t;

typedef struct
{
    void (*init)(void);
    void (*update)(struct nk_context *ctx);
} runtime_app_t;

NU_API runtime_app_t runtime_app_default(nu_sv_t path);
NU_API nu_status_t   runtime_run(runtime_app_t app, nu_bool_t debug);
NU_API nu_status_t   runtime_init_instance(nu_u32_t index, nu_sv_t path);
NU_API void          runtime_set_instance_viewport(nu_u32_t        index,
                                                   nu_b2i_t        viewport,
                                                   viewport_mode_t mode);

void *native_malloc(nu_size_t n);
void  native_free(void *p);

void logger_set_callback(runtime_log_callback_t callback);
void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);

nu_status_t wamr_init(nu_bool_t debug);
void        wamr_free(void);

nu_status_t renderer_init(void);
void        renderer_free(void);
void        renderer_render_instance(nu_b2i_t        viewport,
                                     viewport_mode_t mode,
                                     nu_v2u_t        window_size);

struct RGFW_window;
typedef struct RGFW_window RGFW_window;

nu_status_t  window_init(void);
void         window_free(void);
RGFW_window *window_get_win(void);
void         window_poll_events(void);
void         window_swap_buffers(void);
nu_v2u_t     window_get_size(void);
nu_f32_t     window_get_scale_factor(void);
nu_bool_t    window_poll_command(runtime_command_t *cmd);

typedef void (*gui_callback_t)(struct nk_context *ctx);

nu_status_t        gui_init(void);
void               gui_free(void);
struct nk_context *gui_new_frame(void);
void               gui_render(void);

void gui_char_callback(struct RGFW_window *win, unsigned int codepoint);
void gui_scroll_callback(struct RGFW_window *win, double xoff, double yoff);
void gui_mouse_button_callback(struct RGFW_window *win,
                               int                 button,
                               double              scroll,
                               int                 pressed);

#endif
