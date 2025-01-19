#include "window.h"

#include <core/vm.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define RGFW_EXPORT
#define RGFW_IMPLEMENTATION
#include <rgfw/RGFW.h>

typedef enum
{
    NU_VIEWPORT_FIXED,
    NU_VIEWPORT_FIXED_BEST_FIT,
    NU_VIEWPORT_STRETCH_KEEP_ASPECT,
    NU_VIEWPORT_STRETCH,
} nu_viewport_mode_t;

typedef struct
{
    nu_viewport_mode_t mode;
    nu_f32_t           scale_factor;
    nu_v2u_t           screen;
    nu_b2i_t           extent;
    nu_b2i_t           viewport;
} nu_viewport_t;

static struct
{
    nu_bool_t     close_requested;
    nu_bool_t     fullscreen;
    nu_bool_t     switch_fullscreen;
    nu_viewport_t viewport;
    RGFW_rect     previous_rect;
    RGFW_window  *win;
} window;

static void
update_viewport (nu_viewport_t *v)
{
    nu_v2_t global_pos  = nu_v2(v->extent.min.x, v->extent.min.y);
    nu_v2_t global_size = nu_v2_v2u(nu_b2i_size(v->extent));

    nu_f32_t aspect_ratio = (nu_f32_t)v->screen.x / (nu_f32_t)v->screen.y;

    nu_v2_t size = NU_V2_ZEROS;
    switch (v->mode)
    {
        case NU_VIEWPORT_FIXED: {
            size = nu_v2((nu_f32_t)v->screen.x * v->scale_factor,
                         (nu_f32_t)v->screen.y * v->scale_factor);
        };
        break;
        case NU_VIEWPORT_FIXED_BEST_FIT: {
            nu_f32_t w_factor = global_size.x / (nu_f32_t)v->screen.x;
            nu_f32_t h_factor = global_size.y / (nu_f32_t)v->screen.y;
            nu_f32_t min = NU_MAX(1.0f, nu_floor(NU_MIN(w_factor, h_factor)));
            size.x       = v->screen.x * min;
            size.y       = v->screen.y * min;
        }
        break;
        case NU_VIEWPORT_STRETCH_KEEP_ASPECT: {
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
        case NU_VIEWPORT_STRETCH:
            size = global_size;
            break;
    }

    nu_v2_t vpos = nu_v2_sub(global_size, size);
    vpos         = nu_v2_divs(vpos, 2);
    vpos         = nu_v2_add(vpos, global_pos);
    v->viewport  = nu_b2i_xywh(vpos.x, vpos.y, size.x, size.y);
}

nu_status_t
window_init (void)
{
    // Initialize surface (and inputs)
    const nu_int_t width  = WINDOW_WIDTH;
    const nu_int_t height = WINDOW_HEIGHT;

    // Initialize values
    window.close_requested   = NU_FALSE;
    window.fullscreen        = NU_FALSE;
    window.switch_fullscreen = NU_FALSE;

    // Create window
    window.win
        = RGFW_createWindow("nux", RGFW_RECT(0, 0, width, height), RGFW_CENTER);
    RGFW_window_swapInterval(window.win, 1);

    // Initialize viewport
    window.viewport.mode     = NU_VIEWPORT_STRETCH_KEEP_ASPECT;
    window.viewport.screen   = nu_v2u(VM_SCREEN_WIDTH, VM_SCREEN_HEIGHT);
    window.viewport.extent   = nu_b2i_xywh(0, 0, width, height);
    window.viewport.viewport = nu_b2i_xywh(0, 0, width, height);
    update_viewport(&window.viewport);

    // Bind callbacks
    RGFW_window_makeCurrent_OpenGL(window.win);
    RGFW_window_makeCurrent(window.win);
    // RGFW_setWindowResizeCallback(nu__window_size_callback);

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
                    // nu__mouse_position_callback(_ctx.platform.win,
                    //                             _ctx.platform.win->event.point);
                    break;
                case RGFW_keyPressed:
                    // nu__key_callback(_ctx.platform.win,
                    //                  _ctx.platform.win->event.keyCode,
                    //                  _ctx.platform.win->event.keyName,
                    //                  NU_FALSE,
                    //                  NU_TRUE);
                    break;
                case RGFW_keyReleased:
                    // nu__key_callback(_ctx.platform.win,
                    //                  _ctx.platform.win->event.keyCode,
                    //                  _ctx.platform.win->event.keyName,
                    //                  NU_FALSE,
                    //                  NU_FALSE);
                    if (window.win->event.keyCode == RGFW_Escape)
                    {
                        window.close_requested = NU_TRUE;
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
                    // nu__window_size_callback(_ctx.platform.win,
                    //                          _ctx.platform.win->r);
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
                // nu__window_size_callback(_ctx.platform.win,
                //                          _ctx.platform.win->r);
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
