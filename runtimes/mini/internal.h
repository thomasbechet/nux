#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include <nux.h>

#include <glad/gl.h>
#define RGFW_EXPORT
#include <rgfw/RGFW.h>
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
nu_u32_t     window_swap_buffers(void);
nu_v2u_t     window_get_size(void);
nu_f32_t     window_get_scale_factor(void);
nu_v2_t      window_get_mouse_scroll(void);
nu_bool_t    window_poll_command(runtime_command_t *cmd);
void         window_update_inputs(nux_instance_t inst);

void loop_init(nux_env_t env);
void loop_update(nux_env_t env);

void load_blk_colormap(nux_env_t env);
void draw_sibenik(nux_env_t env, const nux_f32_t *m);
void draw_sponza2(nux_env_t env, const nux_f32_t *m, nux_u32_t wireframe);

#endif
