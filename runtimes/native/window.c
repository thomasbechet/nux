#include "window.h"

#include "core/vm.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define RGFW_EXPORT
#define RGFW_IMPLEMENTATION
#include <rgfw/RGFW.h>

typedef enum
{
    VIEWPORT_FIXED,
    VIEWPORT_FIXED_BEST_FIT,
    VIEWPORT_STRETCH_KEEP_ASPECT,
    VIEWPORT_STRETCH,
} viewport_mode_t;

typedef struct
{
    viewport_mode_t mode;
    nu_f32_t        scale_factor;
    nu_v2u_t        screen;
    nu_b2i_t        extent;
    nu_b2i_t        viewport;
} viewport_t;

static struct
{
    nu_bool_t    close_requested;
    nu_bool_t    fullscreen;
    nu_bool_t    switch_fullscreen;
    viewport_t   viewport;
    RGFW_rect    previous_rect;
    RGFW_window *win;
    nu_u32_t     button;
    nu_f32_t     axis[CONTROLLER_AXIS_MAX];
} window;

static void
update_viewport (viewport_t *v)
{
    nu_v2_t global_pos  = nu_v2(v->extent.min.x, v->extent.min.y);
    nu_v2_t global_size = nu_v2_v2u(nu_b2i_size(v->extent));

    nu_f32_t aspect_ratio = (nu_f32_t)v->screen.x / (nu_f32_t)v->screen.y;

    nu_v2_t size = NU_V2_ZEROS;
    switch (v->mode)
    {
        case VIEWPORT_FIXED: {
            size = nu_v2((nu_f32_t)v->screen.x * v->scale_factor,
                         (nu_f32_t)v->screen.y * v->scale_factor);
        };
        break;
        case VIEWPORT_FIXED_BEST_FIT: {
            nu_f32_t w_factor = global_size.x / (nu_f32_t)v->screen.x;
            nu_f32_t h_factor = global_size.y / (nu_f32_t)v->screen.y;
            nu_f32_t min = NU_MAX(1.0f, nu_floor(NU_MIN(w_factor, h_factor)));
            size.x       = v->screen.x * min;
            size.y       = v->screen.y * min;
        }
        break;
        case VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                size.x = nu_floor(global_size.y * aspect_ratio);
                size.y = nu_floor(global_size.y);
            }
            else
            {
                size.x = nu_floor(global_size.x);
                size.y = nu_floor(global_size.x / aspect_ratio);
            }
        }
        break;
        case VIEWPORT_STRETCH:
            size = global_size;
            break;
    }

    nu_v2_t vpos = nu_v2_sub(global_size, size);
    vpos         = nu_v2_divs(vpos, 2);
    vpos         = nu_v2_add(vpos, global_pos);
    v->viewport  = nu_b2i_xywh(vpos.x, vpos.y, size.x, size.y);
}
static void
resize_callback (RGFW_window *w, RGFW_rect r)
{
    window.viewport.extent
        = nu_b2i_resize(window.viewport.extent, nu_v2u(r.w, r.h));
    update_viewport(&window.viewport);
}

nu_status_t
window_init (void)
{
    // Initialize surface (and inputs)
    const nu_int_t width  = 1600;
    const nu_int_t height = 900;

    // Initialize values
    window.close_requested   = NU_FALSE;
    window.fullscreen        = NU_FALSE;
    window.switch_fullscreen = NU_FALSE;

    // Create window
    window.win
        = RGFW_createWindow("nux", RGFW_RECT(0, 0, width, height), RGFW_CENTER);
    RGFW_window_swapInterval(window.win, 1);

    // Initialize viewport
    window.viewport.mode     = VIEWPORT_STRETCH_KEEP_ASPECT;
    window.viewport.screen   = nu_v2u(GPU_SCREEN_WIDTH, GPU_SCREEN_HEIGHT);
    window.viewport.extent   = nu_b2i_xywh(0, 0, width, height);
    window.viewport.viewport = nu_b2i_xywh(0, 0, width, height);
    update_viewport(&window.viewport);

    // Bind callbacks
    RGFW_window_makeCurrent_OpenGL(window.win);
    RGFW_window_makeCurrent(window.win);
    RGFW_setWindowResizeCallback(resize_callback);

    return NU_SUCCESS;
}
void
window_free (void)
{
    RGFW_window_close(window.win);
}
void
window_poll_events (void)
{
    if (window.win)
    {
        // Check close requested
        if (RGFW_window_shouldClose(window.win))
        {
            window.close_requested = NU_TRUE;
        }

        // Process events
        while (RGFW_window_checkEvent(window.win))
        {
            switch (window.win->event.type)
            {
                case RGFW_mousePosChanged:
                    break;
                case RGFW_keyPressed:
                    switch (window.win->event.keyCode)
                    {
                        case RGFW_w:
                            window.button |= CONTROLLER_BUTTON_Y;
                            break;
                        case RGFW_s:
                            window.button |= CONTROLLER_BUTTON_A;
                            break;
                        case RGFW_a:
                            window.button |= CONTROLLER_BUTTON_X;
                            break;
                        case RGFW_d:
                            window.button |= CONTROLLER_BUTTON_B;
                            break;
                        case RGFW_z:
                            window.button |= CONTROLLER_BUTTON_LB;
                            break;
                        case RGFW_x:
                            window.button |= CONTROLLER_BUTTON_RB;
                            break;
                        case RGFW_h:
                            window.axis[CONTROLLER_AXIS_RIGHTX] = -1;
                            break;
                        case RGFW_l:
                            window.axis[CONTROLLER_AXIS_RIGHTX] = 1;
                            break;
                        case RGFW_j:
                            window.axis[CONTROLLER_AXIS_RIGHTY] = 1;
                            break;
                        case RGFW_k:
                            window.axis[CONTROLLER_AXIS_RIGHTY] = -1;
                            break;
                        default:
                            break;
                    }
                    break;
                case RGFW_keyReleased:
                    if (window.win->event.keyCode == RGFW_Escape)
                    {
                        window.close_requested = NU_TRUE;
                    }
                    switch (window.win->event.keyCode)
                    {
                        case RGFW_w:
                            window.button &= ~CONTROLLER_BUTTON_Y;
                            break;
                        case RGFW_s:
                            window.button &= ~CONTROLLER_BUTTON_A;
                            break;
                        case RGFW_a:
                            window.button &= ~CONTROLLER_BUTTON_X;
                            break;
                        case RGFW_d:
                            window.button &= ~CONTROLLER_BUTTON_B;
                            break;
                        case RGFW_z:
                            window.button &= ~CONTROLLER_BUTTON_LB;
                            break;
                        case RGFW_x:
                            window.button &= ~CONTROLLER_BUTTON_RB;
                            break;
                        case RGFW_h:
                        case RGFW_l:
                            window.axis[CONTROLLER_AXIS_RIGHTX] = 0;
                            break;
                        case RGFW_j:
                        case RGFW_k:
                            window.axis[CONTROLLER_AXIS_RIGHTY] = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                case RGFW_mouseButtonPressed:
                    // nu__mouse_button_callback(_ctx.platform.win,
                    //                           _ctx.platform.win->event.button,
                    //                           _ctx.platform.win->event.scroll,
                    //                           NU_TRUE);
                    break;
                case RGFW_mouseButtonReleased:
                    // nu__mouse_button_callback(_ctx.platform.win,
                    //                           _ctx.platform.win->event.button,
                    //                           _ctx.platform.win->event.scroll,
                    //                           NU_FALSE);
                    break;
                case RGFW_windowResized:
                    resize_callback(window.win, window.win->r);
                    break;
            }
        }

        // Check fullscreen button
        if (window.switch_fullscreen)
        {
            if (window.fullscreen)
            {
                RGFW_window_resize(
                    window.win,
                    RGFW_AREA(window.previous_rect.w, window.previous_rect.h));
                RGFW_window_move(
                    window.win,
                    RGFW_POINT(window.previous_rect.x, window.previous_rect.y));
                RGFW_window_setBorder(window.win, RGFW_TRUE);
                resize_callback(window.win, window.win->r);
            }
            else
            {
                window.previous_rect = window.win->r;
                RGFW_monitor mon     = RGFW_window_getMonitor(window.win);
                RGFW_window_setBorder(window.win, RGFW_FALSE);
                RGFW_window_resize(window.win,
                                   RGFW_AREA(mon.rect.w, mon.rect.h));
                RGFW_window_move(window.win,
                                 RGFW_POINT(mon.rect.x, mon.rect.y));
            }
            window.switch_fullscreen = NU_FALSE;
            window.fullscreen        = !window.fullscreen;
        }
    }
}
void
window_swap_buffers (void)
{
    RGFW_window_swapBuffers(window.win);
}
nu_bool_t
window_close_requested (void)
{
    return window.close_requested;
}
nu_b2i_t
window_get_render_viewport (void)
{
    return window.viewport.viewport;
}

void
os_iou_update_controllers (vm_t *vm)
{
    vm->iou.buttons[0] = window.button;
    nu_memcpy(vm->iou.axis[0], window.axis, sizeof(window.axis));
}
