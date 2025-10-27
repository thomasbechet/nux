#include "internal.h"

#include "gamecontrollerdb.c.inc"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static nux_key_t key_map[] = {
    [GLFW_KEY_SPACE]         = NUX_KEY_SPACE,
    [GLFW_KEY_APOSTROPHE]    = NUX_KEY_APOSTROPHE,
    [GLFW_KEY_COMMA]         = NUX_KEY_COMMA,
    [GLFW_KEY_MINUS]         = NUX_KEY_MINUS,
    [GLFW_KEY_PERIOD]        = NUX_KEY_PERIOD,
    [GLFW_KEY_SLASH]         = NUX_KEY_SLASH,
    [GLFW_KEY_0]             = NUX_KEY_NUM0,
    [GLFW_KEY_1]             = NUX_KEY_NUM1,
    [GLFW_KEY_2]             = NUX_KEY_NUM2,
    [GLFW_KEY_3]             = NUX_KEY_NUM3,
    [GLFW_KEY_4]             = NUX_KEY_NUM4,
    [GLFW_KEY_5]             = NUX_KEY_NUM5,
    [GLFW_KEY_6]             = NUX_KEY_NUM6,
    [GLFW_KEY_7]             = NUX_KEY_NUM7,
    [GLFW_KEY_8]             = NUX_KEY_NUM8,
    [GLFW_KEY_9]             = NUX_KEY_NUM9,
    [GLFW_KEY_SEMICOLON]     = NUX_KEY_SEMICOLON,
    [GLFW_KEY_EQUAL]         = NUX_KEY_EQUAL,
    [GLFW_KEY_A]             = NUX_KEY_A,
    [GLFW_KEY_B]             = NUX_KEY_B,
    [GLFW_KEY_C]             = NUX_KEY_C,
    [GLFW_KEY_D]             = NUX_KEY_D,
    [GLFW_KEY_E]             = NUX_KEY_E,
    [GLFW_KEY_F]             = NUX_KEY_F,
    [GLFW_KEY_G]             = NUX_KEY_G,
    [GLFW_KEY_H]             = NUX_KEY_H,
    [GLFW_KEY_I]             = NUX_KEY_I,
    [GLFW_KEY_J]             = NUX_KEY_J,
    [GLFW_KEY_K]             = NUX_KEY_K,
    [GLFW_KEY_L]             = NUX_KEY_L,
    [GLFW_KEY_M]             = NUX_KEY_M,
    [GLFW_KEY_N]             = NUX_KEY_N,
    [GLFW_KEY_O]             = NUX_KEY_O,
    [GLFW_KEY_P]             = NUX_KEY_P,
    [GLFW_KEY_Q]             = NUX_KEY_Q,
    [GLFW_KEY_R]             = NUX_KEY_R,
    [GLFW_KEY_S]             = NUX_KEY_S,
    [GLFW_KEY_T]             = NUX_KEY_T,
    [GLFW_KEY_U]             = NUX_KEY_U,
    [GLFW_KEY_V]             = NUX_KEY_V,
    [GLFW_KEY_W]             = NUX_KEY_W,
    [GLFW_KEY_X]             = NUX_KEY_X,
    [GLFW_KEY_Y]             = NUX_KEY_Y,
    [GLFW_KEY_Z]             = NUX_KEY_Z,
    [GLFW_KEY_LEFT_BRACKET]  = NUX_KEY_LEFT_BRACKET,
    [GLFW_KEY_BACKSLASH]     = NUX_KEY_BACKSLASH,
    [GLFW_KEY_RIGHT_BRACKET] = NUX_KEY_RIGHT_BRACKET,
    [GLFW_KEY_GRAVE_ACCENT]  = NUX_KEY_GRAVE_ACCENT,
    [GLFW_KEY_ESCAPE]        = NUX_KEY_ESCAPE,
    [GLFW_KEY_ENTER]         = NUX_KEY_ENTER,
    [GLFW_KEY_TAB]           = NUX_KEY_TAB,
    [GLFW_KEY_BACKSPACE]     = NUX_KEY_BACKSPACE,
    [GLFW_KEY_INSERT]        = NUX_KEY_INSERT,
    [GLFW_KEY_DELETE]        = NUX_KEY_DELETE,
    [GLFW_KEY_RIGHT]         = NUX_KEY_RIGHT,
    [GLFW_KEY_LEFT]          = NUX_KEY_LEFT,
    [GLFW_KEY_DOWN]          = NUX_KEY_DOWN,
    [GLFW_KEY_UP]            = NUX_KEY_UP,
    [GLFW_KEY_PAGE_UP]       = NUX_KEY_PAGE_UP,
    [GLFW_KEY_PAGE_DOWN]     = NUX_KEY_PAGE_DOWN,
    [GLFW_KEY_HOME]          = NUX_KEY_HOME,
    [GLFW_KEY_END]           = NUX_KEY_END,
    [GLFW_KEY_CAPS_LOCK]     = NUX_KEY_CAPS_LOCK,
    [GLFW_KEY_SCROLL_LOCK]   = NUX_KEY_SCROLL_LOCK,
    [GLFW_KEY_NUM_LOCK]      = NUX_KEY_NUM_LOCK,
    [GLFW_KEY_PRINT_SCREEN]  = NUX_KEY_PRINT_SCREEN,
    [GLFW_KEY_PAUSE]         = NUX_KEY_PAUSE,
    [GLFW_KEY_F1]            = NUX_KEY_F1,
    [GLFW_KEY_F2]            = NUX_KEY_F2,
    [GLFW_KEY_F3]            = NUX_KEY_F3,
    [GLFW_KEY_F4]            = NUX_KEY_F4,
    [GLFW_KEY_F5]            = NUX_KEY_F5,
    [GLFW_KEY_F6]            = NUX_KEY_F6,
    [GLFW_KEY_F7]            = NUX_KEY_F7,
    [GLFW_KEY_F8]            = NUX_KEY_F8,
    [GLFW_KEY_F9]            = NUX_KEY_F9,
    [GLFW_KEY_F10]           = NUX_KEY_F10,
    [GLFW_KEY_F11]           = NUX_KEY_F11,
    [GLFW_KEY_F12]           = NUX_KEY_F12,
    [GLFW_KEY_F13]           = NUX_KEY_F13,
    [GLFW_KEY_F14]           = NUX_KEY_F14,
    [GLFW_KEY_F15]           = NUX_KEY_F15,
    [GLFW_KEY_F16]           = NUX_KEY_F16,
    [GLFW_KEY_F17]           = NUX_KEY_F17,
    [GLFW_KEY_F18]           = NUX_KEY_F18,
    [GLFW_KEY_F19]           = NUX_KEY_F19,
    [GLFW_KEY_F20]           = NUX_KEY_F20,
    [GLFW_KEY_F21]           = NUX_KEY_F21,
    [GLFW_KEY_F22]           = NUX_KEY_F22,
    [GLFW_KEY_F23]           = NUX_KEY_F23,
    [GLFW_KEY_F24]           = NUX_KEY_F24,
    [GLFW_KEY_F25]           = NUX_KEY_F25,
    [GLFW_KEY_KP_0]          = NUX_KEY_KP_0,
    [GLFW_KEY_KP_1]          = NUX_KEY_KP_1,
    [GLFW_KEY_KP_2]          = NUX_KEY_KP_2,
    [GLFW_KEY_KP_3]          = NUX_KEY_KP_3,
    [GLFW_KEY_KP_4]          = NUX_KEY_KP_4,
    [GLFW_KEY_KP_5]          = NUX_KEY_KP_5,
    [GLFW_KEY_KP_6]          = NUX_KEY_KP_6,
    [GLFW_KEY_KP_7]          = NUX_KEY_KP_7,
    [GLFW_KEY_KP_8]          = NUX_KEY_KP_8,
    [GLFW_KEY_KP_9]          = NUX_KEY_KP_9,
    [GLFW_KEY_KP_DECIMAL]    = NUX_KEY_KP_DECIMAL,
    [GLFW_KEY_KP_DIVIDE]     = NUX_KEY_KP_DIVIDE,
    [GLFW_KEY_KP_MULTIPLY]   = NUX_KEY_KP_MULTIPLY,
    [GLFW_KEY_KP_SUBTRACT]   = NUX_KEY_KP_SUBTRACT,
    [GLFW_KEY_KP_ADD]        = NUX_KEY_KP_ADD,
    [GLFW_KEY_KP_ENTER]      = NUX_KEY_KP_ENTER,
    [GLFW_KEY_KP_EQUAL]      = NUX_KEY_KP_EQUAL,
    [GLFW_KEY_LEFT_SHIFT]    = NUX_KEY_LEFT_SHIFT,
    [GLFW_KEY_LEFT_CONTROL]  = NUX_KEY_LEFT_CONTROL,
    [GLFW_KEY_LEFT_ALT]      = NUX_KEY_LEFT_ALT,
    [GLFW_KEY_LEFT_SUPER]    = NUX_KEY_LEFT_SUPER,
    [GLFW_KEY_RIGHT_SHIFT]   = NUX_KEY_RIGHT_SHIFT,
    [GLFW_KEY_RIGHT_CONTROL] = NUX_KEY_RIGHT_CONTROL,
    [GLFW_KEY_RIGHT_ALT]     = NUX_KEY_RIGHT_ALT,
    [GLFW_KEY_RIGHT_SUPER]   = NUX_KEY_RIGHT_SUPER,
    [GLFW_KEY_MENU]          = NUX_KEY_MENU,
};
static nux_gamepad_button_t gamepad_button_map[] = {
    [GLFW_GAMEPAD_BUTTON_A]            = 0,
    [GLFW_GAMEPAD_BUTTON_B]            = 0,
    [GLFW_GAMEPAD_BUTTON_X]            = 0,
    [GLFW_GAMEPAD_BUTTON_Y]            = 0,
    [GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]  = 0,
    [GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = 0,
    [GLFW_GAMEPAD_BUTTON_BACK]         = 0,
    [GLFW_GAMEPAD_BUTTON_START]        = 0,
    [GLFW_GAMEPAD_BUTTON_GUIDE]        = 0,
    [GLFW_GAMEPAD_BUTTON_LEFT_THUMB]   = 0,
    [GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]  = 0,
    [GLFW_GAMEPAD_BUTTON_DPAD_UP]      = 0,
    [GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]   = 0,
    [GLFW_GAMEPAD_BUTTON_DPAD_DOWN]    = 0,
    [GLFW_GAMEPAD_BUTTON_DPAD_LEFT]    = 0,
};
static nux_mouse_button_t mouse_button_map[] = {
    [GLFW_MOUSE_BUTTON_1] = NUX_MOUSE_BUTTON_LEFT,
    [GLFW_MOUSE_BUTTON_2] = NUX_MOUSE_BUTTON_RIGHT,
    [GLFW_MOUSE_BUTTON_3] = NUX_MOUSE_BUTTON_MIDDLE,
    [GLFW_MOUSE_BUTTON_4] = 0,
    [GLFW_MOUSE_BUTTON_5] = 0,
    [GLFW_MOUSE_BUTTON_6] = 0,
    [GLFW_MOUSE_BUTTON_7] = 0,
    [GLFW_MOUSE_BUTTON_8] = 0,
};

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
                    "GL: %s, message = %s\n",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            fprintf(stderr,
                    "GL: %s, message = %s\n",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            fprintf(stderr,
                    "GL: %s, message = %s\n",
                    gl_message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            fprintf(stderr,
                    "GL: %s, message = %s\n",
                    gl_message_type_string(type),
                    message);
            break;
    }
    assert(severity != GL_DEBUG_SEVERITY_HIGH);
}

static void
resize_callback (GLFWwindow *win, int w, int h)
{
    runtime.size.x = w;
    runtime.size.y = h;
    nux_os_event_t event;
}
static void
key_callback (GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        nux_os_event_t event;
        event.type       = NUX_OS_EVENT_INPUT;
        event.input.type = NUX_INPUT_KEY;
        event.input.key  = key_map[key];
        event.input.button_state
            = action == GLFW_PRESS ? NUX_BUTTON_PRESSED : NUX_BUTTON_RELEASED;
        nux_instance_push_event(runtime.instance, &event);
    }

    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            bool unfocus = true;
            if (runtime.fullscreen)
            {
                runtime.switch_fullscreen = true;
                unfocus                   = true;
            }
            else if (runtime.focused)
            {
                unfocus = true;
            }
            else
            {
                runtime.running = false;
            }
            if (unfocus)
            {
                glfwSetInputMode(runtime.win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                runtime.focused = false;
            }
        }
        if (mods == GLFW_MOD_CONTROL)
        {
            switch (key)
            {
                case GLFW_KEY_P:
                    runtime.switch_fullscreen = true;
                    break;
                case GLFW_KEY_R:
                    runtime.reload = true;
                    break;
            }
        }
    }
}
static void
mouse_button_callback (GLFWwindow *win, int button, int action, int mods)
{
    nux_os_event_t event;
    event.type               = NUX_OS_EVENT_INPUT;
    event.input.type         = NUX_INPUT_MOUSE_BUTTON;
    event.input.mouse_button = mouse_button_map[button];
    event.input.button_state
        = action == GLFW_PRESS ? NUX_BUTTON_PRESSED : NUX_BUTTON_RELEASED;
    nux_instance_push_event(runtime.instance, &event);

    if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            double time = glfwGetTime();
            if (time - runtime.prev_left_click < 0.3)
            {
                runtime.double_click = true;
            }
            runtime.prev_left_click = time;
        }
    }
}
void
scroll_callback (GLFWwindow *win, double xoff, double yoff)
{
    runtime.scroll = yoff;
}
static void
cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
    runtime.cursor_position.x = xpos;
    runtime.cursor_position.y = ypos;
}

nux_status_t
window_init (void)
{
    // Initialize surface (and inputs)
    const int width  = 1200;
    const int height = 700;

    // Initialize values
    runtime.fullscreen        = false;
    runtime.focused           = false;
    runtime.switch_fullscreen = false;
    runtime.reload            = false;

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
    // GL 4.3 required at least for glGetProgramResourceIndex
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    runtime.win = glfwCreateWindow(width, height, "nux", NULL, NULL);
    if (!runtime.win)
    {
        fprintf(stderr, "failed to create GLFW window");
        return NUX_FAILURE;
    }
    runtime.prev_time = glfwGetTime();

    // Load gamecontroller database
    glfwUpdateGamepadMappings(gamecontrollerdb_txt);

    // Configure vsync
    glfwSwapInterval(0);

    // Initialize GL functions
    glfwMakeContextCurrent(runtime.win);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        fprintf(stderr, "failed to load GL functions");
        return NUX_FAILURE;
    }

    // Show cursor by default
    glfwSetInputMode(runtime.win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Setup debug callbacks
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_message_callback, NULL);

    // Initialize viewport
    int w, h;
    glfwGetFramebufferSize(runtime.win, &w, &h);
    runtime.size.x = w;
    runtime.size.y = h;

    // Bind callbacks
    glfwSetFramebufferSizeCallback(runtime.win, resize_callback);
    glfwSetKeyCallback(runtime.win, key_callback);
    glfwSetMouseButtonCallback(runtime.win, mouse_button_callback);
    glfwSetScrollCallback(runtime.win, scroll_callback);
    // glfwSetCharCallback(runtime.win, nk_glfw3_char_callback);
    glfwSetCursorPosCallback(runtime.win, cursor_position_callback);

    return NUX_SUCCESS;
}
void
window_free (void)
{
    glfwDestroyWindow(runtime.win);
    glfwTerminate();
}
void
window_begin_frame (void)
{
    glfwPollEvents();

    // Mouse delta
    if (runtime.focused)
    {
        nux_v2_t delta;
        delta.x = runtime.cursor_position.x - runtime.prev_cursor_position.x;
        delta.y = runtime.cursor_position.y - runtime.prev_cursor_position.y;

        nux_os_event_t event;
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_MOTION_RIGHT;
        event.input.axis_value = delta.x > 0 ? delta.x : 0;
        nux_instance_push_event(runtime.instance, &event);
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_MOTION_LEFT;
        event.input.axis_value = delta.x < 0 ? fabsf(delta.x) : 0;
        nux_instance_push_event(runtime.instance, &event);
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_MOTION_DOWN;
        event.input.axis_value = delta.y > 0 ? delta.y : 0;
        nux_instance_push_event(runtime.instance, &event);
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_MOTION_UP;
        event.input.axis_value = delta.y < 0 ? fabsf(delta.y) : 0;
        nux_instance_push_event(runtime.instance, &event);

        // Scroll delta
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_SCROLL_UP;
        event.input.axis_value = runtime.scroll > 0 ? runtime.scroll : 0;
        nux_instance_push_event(runtime.instance, &event);
        event.type             = NUX_OS_EVENT_INPUT;
        event.input.type       = NUX_INPUT_MOUSE_AXIS;
        event.input.mouse_axis = NUX_MOUSE_SCROLL_DOWN;
        event.input.axis_value = runtime.scroll < 0 ? fabsf(runtime.scroll) : 0;
        nux_instance_push_event(runtime.instance, &event);
    }

    // Update previous cursor and scroll
    runtime.prev_cursor_position = runtime.cursor_position;
    runtime.scroll               = 0;

    if (runtime.win)
    {
        // Check close requested
        if (glfwWindowShouldClose(runtime.win))
        {
            runtime.running = false;
        }

        // Check focus actions
        bool focus = false;
        if (!runtime.focused && runtime.double_click)
        {
            focus                = true;
            runtime.double_click = false;
        }

        // Check fullscreen button
        if (runtime.switch_fullscreen)
        {
            if (runtime.fullscreen)
            {
                glfwSetWindowMonitor(runtime.win,
                                     NULL,
                                     runtime.prev_position.x,
                                     runtime.prev_position.y,
                                     runtime.prev_size.x,
                                     runtime.prev_size.y,
                                     0);
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
                int xpos, ypos;
                glfwGetWindowPos(runtime.win, &xpos, &ypos);
                runtime.prev_position = (v2i_t) { xpos, ypos };
                runtime.prev_size     = runtime.size;
                focus                 = true;
            }
            runtime.switch_fullscreen = false;
            runtime.fullscreen        = !runtime.fullscreen;
        }

        // Focus action
        if (focus)
        {
            glfwSetInputMode(runtime.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            runtime.focused = true;
        }

        // Acquire gamepads inputs
        for (int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; ++jid)
        {
            if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid))
            {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(jid, &state))
                {
                    for (int button = 0; button < GLFW_GAMEPAD_BUTTON_LAST;
                         ++button)
                    {
                        // nux_button_t mask = gamepad_button_to_button(button);
                        // if (mask != (nux_button_t)-1)
                        // {
                        //     if (state.buttons[button])
                        //     {
                        //         runtime.buttons |= mask;
                        //     }
                        //     else
                        //     {
                        //         runtime.buttons &= ~mask;
                        //     }
                        // }
                    }

                    for (int axis = 0; axis < GLFW_GAMEPAD_AXIS_LAST; ++axis)
                    {
                        float value = state.axes[axis];
                        if (fabsf(value) <= 0.3)
                        {
                            value = 0;
                        }
                        if (axis == GLFW_GAMEPAD_AXIS_RIGHT_Y
                            || axis == GLFW_GAMEPAD_AXIS_LEFT_Y)
                        {
                            value = -value;
                        }
                        // runtime.axis[gamepad_axis_to_axis(axis)] = value;
                    }
                }
            }
        }
    }
}
int
window_end_frame (void)
{
    // Swap buffers
    glfwSwapBuffers(runtime.win);
    double time       = glfwGetTime();
    double delta      = time - runtime.prev_time;
    int    fps        = (int)(1. / ((float)delta));
    runtime.prev_time = time;
    return fps;
}
