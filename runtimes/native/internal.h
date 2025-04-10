#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "runtime.h"

void *native_malloc(nu_size_t n);
void  native_free(void *p);

void logger_set_callback(runtime_log_callback_t callback);
void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);

nu_status_t renderer_init(void);
void        renderer_free(void);
void        renderer_clear(nu_b2i_t viewport, nu_v2u_t window_size);
void        renderer_render_instance(nux_instance_t inst,
                                     nu_b2i_t       viewport,
                                     nu_v2u_t       window_size);

nu_status_t  window_init(void);
void         window_free(void);
RGFW_window *window_get_win(void);
void         window_poll_events(void);
void         window_swap_buffers(void);
nu_v2u_t     window_get_size(void);
nu_f32_t     window_get_scale_factor(void);
nu_v2_t      window_get_mouse_scroll(void);
nu_bool_t    window_poll_command(runtime_command_t *cmd);
void         window_update_inputs(nux_instance_t inst);

nu_status_t gui_init(const runtime_config_t *config);
void        gui_free(void);
void        gui_update(void);
void        gui_render(void);

void      gui_char_event(struct RGFW_window *win, unsigned int codepoint);
void      gui_key_event(RGFW_key key, nu_bool_t pressed);
void      gui_mouse_button_callback(struct RGFW_window *win,
                                    int                 button,
                                    double              scroll,
                                    int                 pressed);
nu_bool_t gui_is_double_click(void);

void view_home(struct nk_context *ctx, struct nk_rect bounds);
void view_controls(struct nk_context *ctx, struct nk_rect bounds);
void view_settings(struct nk_context *ctx, struct nk_rect bounds);
void view_debug(struct nk_context *ctx, struct nk_rect bounds);

#endif
