#include "internal.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define MAX_COMMAND 64

static struct
{
    nu_bool_t         fullscreen;
    nu_bool_t         switch_fullscreen;
    GLFWwindow       *win;
    nu_u32_t          buttons[NUX_PLAYER_MAX];
    nu_f32_t          axis[NUX_PLAYER_MAX][NUX_AXIS_MAX];
    runtime_command_t cmds[MAX_COMMAND];
    nu_size_t         cmds_count;
    nu_f32_t          scale_factor;
    nu_v2_t           mouse_scroll;
    nu_v2_t           mouse_position;
    nu_v2u_t          size;
    nu_f64_t          prev_time;
} window;

static nux_button_t
key_to_button (nu_u32_t code)
{
    switch (code)
    {
        // D-Pad
        case GLFW_KEY_LEFT:
            return NUX_BUTTON_LEFT;
        case GLFW_KEY_DOWN:
            return NUX_BUTTON_DOWN;
        case GLFW_KEY_UP:
            return NUX_BUTTON_UP;
        case GLFW_KEY_RIGHT:
            return NUX_BUTTON_RIGHT;

        // Triggers
        case GLFW_KEY_E:
            return NUX_BUTTON_RB;
        case GLFW_KEY_Q:
            return NUX_BUTTON_LB;

        // Action buttons
        case GLFW_KEY_F:
            return NUX_BUTTON_A;
        case GLFW_KEY_R:
            return NUX_BUTTON_B;
        case GLFW_KEY_X:
            return NUX_BUTTON_Y;
        case GLFW_KEY_Z:
            return NUX_BUTTON_X;
    }
    return -1;
}
static nux_axis_t
key_to_axis (nu_u32_t code, nu_f32_t *value)
{
    switch (code)
    {
        // Left Stick
        case GLFW_KEY_W:
            *value = 1;
            return NUX_AXIS_LEFTY;
        case GLFW_KEY_A:
            *value = -1;
            return NUX_AXIS_LEFTX;
        case GLFW_KEY_S:
            *value = -1;
            return NUX_AXIS_LEFTY;
        case GLFW_KEY_D:
            *value = 1;
            return NUX_AXIS_LEFTX;

        // Right Stick
        case GLFW_KEY_J:
            *value = -1;
            return NUX_AXIS_RIGHTY;
        case GLFW_KEY_H:
            *value = -1;
            return NUX_AXIS_RIGHTX;
        case GLFW_KEY_K:
            *value = 1;
            return NUX_AXIS_RIGHTY;
        case GLFW_KEY_L:
            *value = 1;
            return NUX_AXIS_RIGHTX;

        case GLFW_KEY_U:
            *value = 1;
            return NUX_AXIS_LT;
        case GLFW_KEY_O:
            *value = 1;
            return NUX_AXIS_RT;

        default:
            break;
    }
    return -1;
}
static nux_button_t
gamepad_button_to_button (nu_u32_t button)
{
    switch (button)
    {
        case GLFW_GAMEPAD_BUTTON_A:
            return NUX_BUTTON_A;
        case GLFW_GAMEPAD_BUTTON_X:
            return NUX_BUTTON_Y;
        case GLFW_GAMEPAD_BUTTON_Y:
            return NUX_BUTTON_B;
        case GLFW_GAMEPAD_BUTTON_B:
            return NUX_BUTTON_UP;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP:
            return NUX_BUTTON_DOWN;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
            return NUX_BUTTON_LEFT;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
            return NUX_BUTTON_RIGHT;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
            return NUX_BUTTON_LB;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
            return NUX_BUTTON_RB;
        default:
            return -1;
    }
}

static void
resize_callback (GLFWwindow *win, int w, int h)
{
    window.size = nu_v2u(w, h);
}
static void
key_callback (GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        nux_button_t button = key_to_button(key);
        nu_f32_t     axvalue;
        nux_axis_t   axis = key_to_axis(key, &axvalue);
        if (button != (nux_button_t)-1)
        {
            window.buttons[0] |= button;
        }
        if (axis != (nux_axis_t)-1)
        {
            window.axis[0][axis] = axvalue;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            window.cmds[window.cmds_count++] = COMMAND_EXIT;
        }
        if (key == GLFW_KEY_P)
        {
            window.switch_fullscreen = NU_TRUE;
        }
        if (key == GLFW_KEY_T)
        {
            window.cmds[window.cmds_count++] = COMMAND_SAVE_STATE;
        }
        if (key == GLFW_KEY_Y)
        {
            window.cmds[window.cmds_count++] = COMMAND_LOAD_STATE;
        }
        nux_button_t button = key_to_button(key);
        nu_f32_t     axvalue;
        nux_axis_t   axis = key_to_axis(key, &axvalue);
        if (button != (nux_button_t)-1)
        {
            window.buttons[0] &= ~button;
        }
        if (axis != (nux_axis_t)-1)
        {
            window.axis[0][axis] = 0;
        }
    }
}

nu_status_t
window_init (void)
{
    // Initialize surface (and inputs)
    const nu_int_t width  = 1200;
    const nu_int_t height = 700;

    // Initialize values
    window.fullscreen        = NU_FALSE;
    window.switch_fullscreen = NU_FALSE;
    window.cmds_count        = 0;

    // Initialized GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to init GLFW");
        return NU_FAILURE;
    }

    // Create window
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
    window.win = glfwCreateWindow(width, height, "nux", NU_NULL, NU_NULL);
    if (!window.win)
    {
        fprintf(stderr, "Failed to create GLFW window");
        return NU_FAILURE;
    }
    window.prev_time = glfwGetTime();

    // Bind callbacks
    glfwMakeContextCurrent(window.win);
    glfwSetFramebufferSizeCallback(window.win, resize_callback);
    glfwSetKeyCallback(window.win, key_callback);

    // Initialize viewport
    int w, h;
    glfwGetFramebufferSize(window.win, &w, &h);
    window.size         = nu_v2u(w, h);
    window.scale_factor = 1;

    return NU_SUCCESS;
}
void
window_free (void)
{
    glfwDestroyWindow(window.win);
    glfwTerminate();
}
void
window_poll_events (void)
{
    glfwPollEvents();

    if (window.win)
    {
        // Check close requested
        if (glfwWindowShouldClose(window.win))
        {
            window.cmds[window.cmds_count++] = COMMAND_EXIT;
        }

        // Process events
        // case RGFW_gamepadButtonPressed: {
        //     nux_button_t button
        //         = gamepad_button_to_button(window.win->event.button);
        //     if (button != (nux_button_t)-1)
        //     {
        //         window.buttons[1] |= button;
        //     }
        // }
        // break;
        // case RGFW_gamepadButtonReleased: {
        //     nux_button_t button
        //         = gamepad_button_to_button(window.win->event.button);
        //     if (button != (nux_button_t)-1)
        //     {
        //         window.buttons[1] &= ~button;
        //     }
        // }
        // break;

        // Check fullscreen button
        if (window.switch_fullscreen)
        {
            if (window.fullscreen)
            {
                glfwSetWindowMonitor(window.win, NU_NULL, 50, 50, 1200, 800, 0);
            }
            else
            {
                GLFWmonitor       *mon = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode
                    = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(window.win,
                                     mon,
                                     0,
                                     0,
                                     mode->width,
                                     mode->height,
                                     mode->refreshRate);
            }
            window.switch_fullscreen = NU_FALSE;
            window.fullscreen        = !window.fullscreen;
        }

        // Update gamepad related axis
        {
            // const nu_u32_t controller = 0;
            // const nu_u32_t player     = 1;
            // nu_v2_t        ax         = nu_v2(
            //     RGFW_getGamepadAxis(window.win, controller, 0).x / 100.0,
            //     RGFW_getGamepadAxis(window.win, controller, 0).y / 100.0);
            // if (nu_v2_norm(ax) < 0.3)
            // {
            //     ax = NU_V2_ZEROS;
            // }
            // window.axis[player][NUX_AXIS_LEFTX] = ax.x;
            // window.axis[player][NUX_AXIS_LEFTY] = -ax.y;
            // ax = nu_v2(RGFW_getGamepadAxis(window.win, controller, 1).x /
            // 100.0,
            //            RGFW_getGamepadAxis(window.win, controller, 1).y
            //                / 100.0);
            // if (nu_v2_norm(ax) < 0.1)
            // {
            //     ax = NU_V2_ZEROS;
            // }
            // window.axis[player][NUX_AXIS_RIGHTX] = ax.x;
            // window.axis[player][NUX_AXIS_RIGHTY] = -ax.y;
        }
    }
}
nu_u32_t
window_swap_buffers (void)
{
    glfwSwapBuffers(window.win);
    nu_f64_t time    = glfwGetTime();
    nu_f64_t delta   = time - window.prev_time;
    nu_u32_t fps     = (nu_u32_t)(1. / ((nu_f32_t)delta));
    window.prev_time = time;
    return fps;
}
nu_v2u_t
window_get_size (void)
{
    return window.size;
}
nu_f32_t
window_get_scale_factor (void)
{
    return window.scale_factor;
}
nu_v2_t
window_get_mouse_scroll (void)
{
    return window.mouse_scroll;
}
nu_v2_t
window_get_mouse_position (void)
{
    double x, y;
    float  sx, sy;
    glfwGetCursorPos(window.win, &x, &y);
    glfwGetWindowContentScale(window.win, &sx, &sy);
    return nu_v2(x * sx, y * sy);
}
nu_f64_t
window_get_time (void)
{
    return glfwGetTime();
}
nu_bool_t
window_is_mouse_pressed (int button)
{
    return glfwGetMouseButton(window.win, button) == GLFW_PRESS;
}
nu_bool_t
window_poll_command (runtime_command_t *cmd)
{
    if (!window.cmds_count)
    {
        return NU_FALSE;
    }
    *cmd = window.cmds[--window.cmds_count];
    return NU_TRUE;
}

void
window_update_inputs (nux_instance_t inst)
{
    nux_instance_set_buttons(inst, 0, window.buttons[0]);
    for (nu_size_t a = 0; a < NUX_AXIS_MAX; ++a)
    {
        nux_instance_set_axis(inst, 0, a, window.axis[0][a]);
    }
}
