#ifndef RUNTIME_H
#define RUNTIME_H

#include <nulib/nulib.h>

typedef void (*runtime_log_callback_t)(nu_log_level_t   level,
                                       const nu_char_t *fmt,
                                       va_list          args);

typedef enum
{
    COMMAND_EXIT,
    COMMAND_SAVE_STATE,
    COMMAND_LOAD_STATE,
} window_command_t;

NU_API nu_status_t runtime_run(nu_sv_t path, nu_bool_t debug);

void *native_malloc(nu_size_t n);
void  native_free(void *p);

void logger_set_callback(runtime_log_callback_t callback);
void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);

nu_status_t wamr_init(nu_bool_t debug);
void        wamr_free(void);

nu_status_t renderer_init(void);
void        renderer_free(void);
void        renderer_set_viewport(nu_b2i_t viewport);

struct RGFW_window;
typedef struct RGFW_window RGFW_window;

nu_status_t  window_init(void);
void         window_free(void);
RGFW_window *window_get_win(void);
void         window_poll_events(void);
void         window_swap_buffers(void);
nu_b2i_t     window_get_render_viewport(void);
nu_v2u_t     window_get_size(void);
nu_bool_t    window_poll_command(window_command_t *cmd);

#ifdef NUX_BUILD_GUI

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>

struct RGFW_window;

nu_status_t gui_init(struct RGFW_window *win);
void        gui_free(void);

void               gui_font_stash_begin(struct nk_font_atlas **atlas);
void               gui_font_stash_end(void);
struct nk_context *gui_new_frame(void);
void               gui_render(enum nk_anti_aliasing aa,
                              int                   max_vertex_buffer,
                              int                   max_element_buffer);

void gui_device_destroy(void);
void gui_device_create(void);

void gui_char_callback(struct RGFW_window *win, unsigned int codepoint);
void gui_scroll_callback(struct RGFW_window *win, double xoff, double yoff);
void gui_mouse_button_callback(struct RGFW_window *win,
                               int                 button,
                               double              scroll,
                               int                 pressed);

#endif

#endif
