#include "internal.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static const GLchar *
gl_message_type_string (GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
    }
    return "";
}
static void GLAPIENTRY
gl_message_callback (GLenum        source,
                     GLenum        type,
                     GLuint        id,
                     GLenum        severity,
                     GLsizei       length,
                     const GLchar *message,
                     const void   *userParam)
{
    (void)source;
    (void)id;
    (void)length;
    if (type == GL_DEBUG_TYPE_OTHER) // Skip other messages
    {
        return;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    gl_message_type_string(type),
                    message);
            break;
    }
    // NU_ASSERT(severity != GL_DEBUG_SEVERITY_HIGH);
}

static nux_button_t
key_to_button (int code)
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
key_to_axis (int code, float *value)
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
gamepad_button_to_button (int button)
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
    runtime.size.x = w;
    runtime.size.y = h;
}
static void
key_callback (GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        nux_button_t button = key_to_button(key);
        float        axvalue;
        nux_axis_t   axis = key_to_axis(key, &axvalue);
        if (button != (nux_button_t)-1)
        {
            runtime.buttons[0] |= button;
        }
        if (axis != (nux_axis_t)-1)
        {
            runtime.axis[0][axis] = axvalue;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            runtime.running = false;
        }
        if (key == GLFW_KEY_P)
        {
            runtime.switch_fullscreen = true;
        }
        nux_button_t button = key_to_button(key);
        float        axvalue;
        nux_axis_t   axis = key_to_axis(key, &axvalue);
        if (button != (nux_button_t)-1)
        {
            runtime.buttons[0] &= ~button;
        }
        if (axis != (nux_axis_t)-1)
        {
            runtime.axis[0][axis] = 0;
        }
    }

    nk_glfw3_key_callback(win, key, scancode, action, mods);
}

nux_status_t
window_init (void)
{
    // Initialize surface (and inputs)
    const int width  = 1200;
    const int height = 700;

    // Initialize values
    runtime.fullscreen        = false;
    runtime.switch_fullscreen = false;

    // Initialized GLFW
#ifdef NUX_PLATFORM_UNIX
    glfwInitHint(GLFW_PLATFORM,
                 GLFW_PLATFORM_X11); // Wayland not supported by renderdoc
#endif
    if (!glfwInit())
    {
        fprintf(stderr, "failed to init GLFW");
        return NUX_FAILURE;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    runtime.win = glfwCreateWindow(width, height, "nux", NULL, NULL);
    if (!runtime.win)
    {
        fprintf(stderr, "failed to create GLFW window");
        return NUX_FAILURE;
    }
    runtime.prev_time = glfwGetTime();

    // Bind callbacks
    glfwMakeContextCurrent(runtime.win);
    glfwSetFramebufferSizeCallback(runtime.win, resize_callback);
    glfwSetKeyCallback(runtime.win, key_callback);

    // Configure vsync
    // glfwSwapInterval(0);

    // Initialize GL functions
    if (!gladLoadGL(glfwGetProcAddress))
    {
        fprintf(stderr, "failed to load GL functions");
        return NUX_FAILURE;
    }

    // Setup debug callbacks
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_message_callback, NULL);

    // Initialize viewport
    int w, h;
    glfwGetFramebufferSize(runtime.win, &w, &h);
    runtime.size.x = w;
    runtime.size.y = h;

    // Initialize nuklear context
    glfwSetScrollCallback(runtime.win, nk_gflw3_scroll_callback);
    glfwSetCharCallback(runtime.win, nk_glfw3_char_callback);
    glfwSetMouseButtonCallback(runtime.win, nk_glfw3_mouse_button_callback);
    nk_glfw3_init(&runtime.nk_glfw, runtime.win, NK_GLFW3_DEFAULT);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&runtime.nk_glfw, &atlas);
    nk_glfw3_font_stash_end(&runtime.nk_glfw);

    return NUX_SUCCESS;
}
void
window_free (void)
{
    nk_glfw3_shutdown(&runtime.nk_glfw);
    glfwDestroyWindow(runtime.win);
    glfwTerminate();
}
void
window_begin_frame (void)
{
    glfwPollEvents();

    if (runtime.win)
    {
        // Check close requested
        if (glfwWindowShouldClose(runtime.win))
        {
            runtime.running = false;
        }

        // Process events
        // case RGFW_gamepadButtonPressed: {
        //     nux_button_t button
        //         = gamepad_button_to_button(runtime.win->event.button);
        //     if (button != (nux_button_t)-1)
        //     {
        //         runtime.buttons[1] |= button;
        //     }
        // }
        // break;
        // case RGFW_gamepadButtonReleased: {
        //     nux_button_t button
        //         = gamepad_button_to_button(runtime.win->event.button);
        //     if (button != (nux_button_t)-1)
        //     {
        //         runtime.buttons[1] &= ~button;
        //     }
        // }
        // break;

        // Check fullscreen button
        if (runtime.switch_fullscreen)
        {
            if (runtime.fullscreen)
            {
                glfwSetWindowMonitor(runtime.win, NULL, 50, 50, 1200, 800, 0);
            }
            else
            {
                GLFWmonitor       *mon = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode
                    = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(runtime.win,
                                     mon,
                                     0,
                                     0,
                                     mode->width,
                                     mode->height,
                                     mode->refreshRate);
            }
            runtime.switch_fullscreen = false;
            runtime.fullscreen        = !runtime.fullscreen;
        }

        // Update gamepad related axis
        {
            // const int controller = 0;
            // const int player     = 1;
            // nu_v2_t        ax         = nu_v2(
            //     RGFW_getGamepadAxis(runtime.win, controller, 0).x / 100.0,
            //     RGFW_getGamepadAxis(runtime.win, controller, 0).y / 100.0);
            // if (nu_v2_norm(ax) < 0.3)
            // {
            //     ax = NU_V2_ZEROS;
            // }
            // runtime.axis[player][NUX_AXIS_LEFTX] = ax.x;
            // runtime.axis[player][NUX_AXIS_LEFTY] = -ax.y;
            // ax = nu_v2(RGFW_getGamepadAxis(runtime.win, controller, 1).x /
            // 100.0,
            //            RGFW_getGamepadAxis(runtime.win, controller, 1).y
            //                / 100.0);
            // if (nu_v2_norm(ax) < 0.1)
            // {
            //     ax = NU_V2_ZEROS;
            // }
            // runtime.axis[player][NUX_AXIS_RIGHTX] = ax.x;
            // runtime.axis[player][NUX_AXIS_RIGHTY] = -ax.y;
        }
    }

    // Begin nuklear context
    nk_glfw3_new_frame(&runtime.nk_glfw);
}
int
window_end_frame (void)
{
    // Render GUI
    nk_glfw3_render(&runtime.nk_glfw,
                    NK_ANTI_ALIASING_ON,
                    MAX_VERTEX_BUFFER,
                    MAX_ELEMENT_BUFFER);

    // Swap buffers
    glfwSwapBuffers(runtime.win);
    double time       = glfwGetTime();
    double delta      = time - runtime.prev_time;
    int    fps        = (int)(1. / ((float)delta));
    runtime.prev_time = time;
    return fps;
}

void
nux_os_input_update (void *userdata, nux_u32_t *buttons, nux_f32_t *axis)
{
    buttons[0] = runtime.buttons[0];
    for (int a = 0; a < NUX_AXIS_MAX; ++a)
    {
        axis[a] = runtime.axis[0][a];
    }
}
