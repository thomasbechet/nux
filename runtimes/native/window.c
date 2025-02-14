#include "window.h"

#include "core/vm.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define RGFW_EXPORT
#define RGFW_IMPLEMENTATION
#include <rgfw/RGFW.h>

#define MAX_COMMAND 64

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
    nu_bool_t        fullscreen;
    nu_bool_t        switch_fullscreen;
    viewport_t       viewport;
    RGFW_rect        previous_rect;
    RGFW_window     *win;
    nu_u32_t         buttons[MAX_PLAYER];
    nu_f32_t         axis[MAX_PLAYER][SYS_AXIS_ENUM_MAX];
    window_command_t cmds[MAX_COMMAND];
    nu_size_t        cmds_count;
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
    const nu_int_t width  = 1600 / 2;
    const nu_int_t height = 900 / 2;

    // Initialize values
    window.fullscreen        = NU_FALSE;
    window.switch_fullscreen = NU_FALSE;
    window.cmds_count        = 0;

    // Create window
    RGFW_windowFlags flags = RGFW_windowCenter;
    flags                  = 0;
    window.win
        = RGFW_createWindow("nux", RGFW_RECT(0, 0, width, height), flags);
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
static sys_button_t
key_to_button (nu_u32_t code)
{
    switch (code)
    {
        // D-Pad
        case RGFW_left:
            return SYS_BUTTON_LEFT;
        case RGFW_down:
            return SYS_BUTTON_DOWN;
        case RGFW_up:
            return SYS_BUTTON_UP;
        case RGFW_right:
            return SYS_BUTTON_RIGHT;

        // Triggers
        case RGFW_e:
            return SYS_BUTTON_RB;
        case RGFW_q:
            return SYS_BUTTON_LB;

        // Action buttons
        case RGFW_f:
            return SYS_BUTTON_A;
        case RGFW_r:
            return SYS_BUTTON_B;
        case RGFW_x:
            return SYS_BUTTON_Y;
        case RGFW_z:
            return SYS_BUTTON_X;
    }
    return -1;
}
static sys_axis_t
key_to_axis (nu_u32_t code, nu_f32_t *value)
{
    switch (code)
    {
        // Left Stick
        case RGFW_w:
            *value = 1;
            return SYS_AXIS_LEFTY;
        case RGFW_a:
            *value = -1;
            return SYS_AXIS_LEFTX;
        case RGFW_s:
            *value = -1;
            return SYS_AXIS_LEFTY;
        case RGFW_d:
            *value = 1;
            return SYS_AXIS_LEFTX;

        // Right Stick
        case RGFW_j:
            *value = -1;
            return SYS_AXIS_RIGHTY;
        case RGFW_h:
            *value = -1;
            return SYS_AXIS_RIGHTX;
        case RGFW_k:
            *value = 1;
            return SYS_AXIS_RIGHTY;
        case RGFW_l:
            *value = 1;
            return SYS_AXIS_RIGHTX;

        case RGFW_u:
            *value = 1;
            return SYS_AXIS_LT;
        case RGFW_o:
            *value = 1;
            return SYS_AXIS_RT;

        default:
            break;
    }
    return -1;
}
static sys_button_t
gamepad_button_to_button (nu_u32_t button)
{
    switch (button)
    {
        case RGFW_gamepadA:
            return SYS_BUTTON_A;
        case RGFW_gamepadX:
            return SYS_BUTTON_X;
        case RGFW_gamepadY:
            return SYS_BUTTON_Y;
        case RGFW_gamepadB:
            return SYS_BUTTON_B;
        case RGFW_gamepadUp:
            return SYS_BUTTON_UP;
        case RGFW_gamepadDown:
            return SYS_BUTTON_DOWN;
        case RGFW_gamepadLeft:
            return SYS_BUTTON_LEFT;
        case RGFW_gamepadRight:
            return SYS_BUTTON_RIGHT;
        case RGFW_gamepadL1:
            return SYS_BUTTON_LB;
        case RGFW_gamepadR1:
            return SYS_BUTTON_RB;
        default:
            return -1;
    }
}
void
window_poll_events (void)
{
    if (window.win)
    {
        // Check close requested
        if (RGFW_window_shouldClose(window.win))
        {
            window.cmds[window.cmds_count++] = COMMAND_EXIT;
        }

        // Process events
        while (RGFW_window_checkEvent(window.win))
        {
            switch (window.win->event.type)
            {
                case RGFW_mousePosChanged:
                    break;
                case RGFW_keyPressed: {
                    sys_button_t button = key_to_button(window.win->event.key);
                    nu_f32_t     axvalue;
                    sys_axis_t   axis
                        = key_to_axis(window.win->event.key, &axvalue);
                    if (button != (sys_button_t)-1)
                    {
                        window.buttons[0] |= button;
                    }
                    if (axis != (sys_axis_t)-1)
                    {
                        window.axis[0][axis] = axvalue;
                    }
                }
                break;
                case RGFW_keyReleased: {
                    if (window.win->event.key == RGFW_escape)
                    {
                        window.cmds[window.cmds_count++] = COMMAND_EXIT;
                    }
                    if (window.win->event.key == RGFW_p)
                    {
                        window.switch_fullscreen = NU_TRUE;
                    }
                    if (window.win->event.key == RGFW_t)
                    {
                        window.cmds[window.cmds_count++] = COMMAND_SAVE_STATE;
                    }
                    if (window.win->event.key == RGFW_y)
                    {
                        window.cmds[window.cmds_count++] = COMMAND_LOAD_STATE;
                    }
                    sys_button_t button = key_to_button(window.win->event.key);
                    nu_f32_t     axvalue;
                    sys_axis_t   axis
                        = key_to_axis(window.win->event.key, &axvalue);
                    if (button != (sys_button_t)-1)
                    {
                        window.buttons[0] &= ~button;
                    }
                    if (axis != (sys_axis_t)-1)
                    {
                        window.axis[0][axis] = 0;
                    }
                }
                break;
                case RGFW_gamepadButtonPressed: {
                    sys_button_t button
                        = gamepad_button_to_button(window.win->event.button);
                    if (button != (sys_button_t)-1)
                    {
                        window.buttons[1] |= button;
                    }
                }
                break;
                case RGFW_gamepadButtonReleased: {
                    sys_button_t button
                        = gamepad_button_to_button(window.win->event.button);
                    if (button != (sys_button_t)-1)
                    {
                        window.buttons[1] &= ~button;
                    }
                }
                break;
                case RGFW_gamepadConnected:
                    printf("Gamepad (%i) connected %s\n",
                           window.win->event.gamepad,
                           RGFW_getGamepadName(window.win,
                                               window.win->event.gamepad));
                    break;
                case RGFW_gamepadDisconnected:
                    printf("Gamepad (%i) disconnected %s\n",
                           window.win->event.gamepad,
                           RGFW_getGamepadName(window.win,
                                               window.win->event.gamepad));
                    break;

                case RGFW_mouseButtonPressed:
                    break;
                case RGFW_mouseButtonReleased:
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

        // Update gamepad related axis
        {
            const nu_u32_t controller = 0;
            const nu_u32_t player     = 1;
            nu_v2_t        ax         = nu_v2(
                RGFW_getGamepadAxis(window.win, controller, 0).x / 100.0,
                RGFW_getGamepadAxis(window.win, controller, 0).y / 100.0);
            if (nu_v2_norm(ax) < 0.3)
            {
                ax = NU_V2_ZEROS;
            }
            window.axis[player][SYS_AXIS_LEFTX] = ax.x;
            window.axis[player][SYS_AXIS_LEFTY] = -ax.y;
            ax = nu_v2(RGFW_getGamepadAxis(window.win, controller, 1).x / 100.0,
                       RGFW_getGamepadAxis(window.win, controller, 1).y
                           / 100.0);
            if (nu_v2_norm(ax) < 0.1)
            {
                ax = NU_V2_ZEROS;
            }
            window.axis[player][SYS_AXIS_RIGHTX] = ax.x;
            window.axis[player][SYS_AXIS_RIGHTY] = -ax.y;
        }
    }
}
void
window_swap_buffers (void)
{
    RGFW_window_swapBuffers(window.win);
}
nu_b2i_t
window_get_render_viewport (void)
{
    return window.viewport.viewport;
}
nu_bool_t
window_poll_command (window_command_t *cmd)
{
    if (!window.cmds_count)
    {
        return NU_FALSE;
    }
    *cmd = window.cmds[--window.cmds_count];
    return NU_TRUE;
}

void
os_gpad_update (vm_t *vm)
{
    nu_memcpy(vm->gamepad.buttons, window.buttons, sizeof(window.buttons));
    nu_memcpy(vm->gamepad.axis, window.axis, sizeof(window.axis));
}
