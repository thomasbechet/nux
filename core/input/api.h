#ifndef NUX_INPUT_H
#define NUX_INPUT_H

#include <base/api.h>

typedef enum
{
    NUX_INPUT_UNMAPPED       = 0,
    NUX_INPUT_KEY            = 1,
    NUX_INPUT_MOUSE_BUTTON   = 2,
    NUX_INPUT_MOUSE_AXIS     = 3,
    NUX_INPUT_GAMEPAD_BUTTON = 4,
    NUX_INPUT_GAMEPAD_AXIS   = 5,
} nux_input_type_t;

typedef enum
{
    NUX_BUTTON_PRESSED  = 1,
    NUX_BUTTON_RELEASED = 0,
} nux_button_state_t;

typedef enum
{
    NUX_KEY_SPACE         = 0,
    NUX_KEY_APOSTROPHE    = 1,
    NUX_KEY_COMMA         = 2,
    NUX_KEY_MINUS         = 3,
    NUX_KEY_PERIOD        = 4,
    NUX_KEY_SLASH         = 5,
    NUX_KEY_NUM0          = 6,
    NUX_KEY_NUM1          = 7,
    NUX_KEY_NUM2          = 8,
    NUX_KEY_NUM3          = 9,
    NUX_KEY_NUM4          = 10,
    NUX_KEY_NUM5          = 11,
    NUX_KEY_NUM6          = 12,
    NUX_KEY_NUM7          = 13,
    NUX_KEY_NUM8          = 14,
    NUX_KEY_NUM9          = 15,
    NUX_KEY_SEMICOLON     = 16,
    NUX_KEY_EQUAL         = 17,
    NUX_KEY_A             = 18,
    NUX_KEY_B             = 19,
    NUX_KEY_C             = 20,
    NUX_KEY_D             = 21,
    NUX_KEY_E             = 22,
    NUX_KEY_F             = 23,
    NUX_KEY_G             = 24,
    NUX_KEY_H             = 25,
    NUX_KEY_I             = 26,
    NUX_KEY_J             = 27,
    NUX_KEY_K             = 29,
    NUX_KEY_L             = 30,
    NUX_KEY_M             = 31,
    NUX_KEY_N             = 32,
    NUX_KEY_O             = 33,
    NUX_KEY_P             = 34,
    NUX_KEY_Q             = 35,
    NUX_KEY_R             = 36,
    NUX_KEY_S             = 37,
    NUX_KEY_T             = 38,
    NUX_KEY_U             = 39,
    NUX_KEY_V             = 40,
    NUX_KEY_W             = 41,
    NUX_KEY_X             = 42,
    NUX_KEY_Y             = 43,
    NUX_KEY_Z             = 44,
    NUX_KEY_LEFT_BRACKET  = 45,
    NUX_KEY_BACKSLASH     = 46,
    NUX_KEY_RIGHT_BRACKET = 47,
    NUX_KEY_GRAVE_ACCENT  = 48,
    NUX_KEY_ESCAPE        = 49,
    NUX_KEY_ENTER         = 50,
    NUX_KEY_TAB           = 51,
    NUX_KEY_BACKSPACE     = 52,
    NUX_KEY_INSERT        = 53,
    NUX_KEY_DELETE        = 54,
    NUX_KEY_RIGHT         = 55,
    NUX_KEY_LEFT          = 56,
    NUX_KEY_DOWN          = 57,
    NUX_KEY_UP            = 58,
    NUX_KEY_PAGE_UP       = 59,
    NUX_KEY_PAGE_DOWN     = 60,
    NUX_KEY_HOME          = 61,
    NUX_KEY_END           = 62,
    NUX_KEY_CAPS_LOCK     = 63,
    NUX_KEY_SCROLL_LOCK   = 64,
    NUX_KEY_NUM_LOCK      = 65,
    NUX_KEY_PRINT_SCREEN  = 66,
    NUX_KEY_PAUSE         = 67,
    NUX_KEY_F1            = 68,
    NUX_KEY_F2            = 69,
    NUX_KEY_F3            = 70,
    NUX_KEY_F4            = 71,
    NUX_KEY_F5            = 72,
    NUX_KEY_F6            = 73,
    NUX_KEY_F7            = 74,
    NUX_KEY_F8            = 75,
    NUX_KEY_F9            = 76,
    NUX_KEY_F10           = 77,
    NUX_KEY_F11           = 78,
    NUX_KEY_F12           = 79,
    NUX_KEY_F13           = 80,
    NUX_KEY_F14           = 81,
    NUX_KEY_F15           = 82,
    NUX_KEY_F16           = 83,
    NUX_KEY_F17           = 84,
    NUX_KEY_F18           = 85,
    NUX_KEY_F19           = 86,
    NUX_KEY_F20           = 87,
    NUX_KEY_F21           = 88,
    NUX_KEY_F22           = 89,
    NUX_KEY_F23           = 90,
    NUX_KEY_F24           = 91,
    NUX_KEY_F25           = 92,
    NUX_KEY_KP_0          = 93,
    NUX_KEY_KP_1          = 94,
    NUX_KEY_KP_2          = 95,
    NUX_KEY_KP_3          = 96,
    NUX_KEY_KP_4          = 97,
    NUX_KEY_KP_5          = 98,
    NUX_KEY_KP_6          = 99,
    NUX_KEY_KP_7          = 100,
    NUX_KEY_KP_8          = 101,
    NUX_KEY_KP_9          = 102,
    NUX_KEY_KP_DECIMAL    = 103,
    NUX_KEY_KP_DIVIDE     = 104,
    NUX_KEY_KP_MULTIPLY   = 105,
    NUX_KEY_KP_SUBTRACT   = 106,
    NUX_KEY_KP_ADD        = 107,
    NUX_KEY_KP_ENTER      = 108,
    NUX_KEY_KP_EQUAL      = 109,
    NUX_KEY_LEFT_SHIFT    = 110,
    NUX_KEY_LEFT_CONTROL  = 111,
    NUX_KEY_LEFT_ALT      = 112,
    NUX_KEY_LEFT_SUPER    = 113,
    NUX_KEY_RIGHT_SHIFT   = 114,
    NUX_KEY_RIGHT_CONTROL = 115,
    NUX_KEY_RIGHT_ALT     = 116,
    NUX_KEY_RIGHT_SUPER   = 117,
    NUX_KEY_MENU          = 118,
} nux_key_t;

typedef enum
{
    NUX_MOUSE_BUTTON_LEFT   = 0,
    NUX_MOUSE_BUTTON_RIGHT  = 1,
    NUX_MOUSE_BUTTON_MIDDLE = 2,
    NUX_MOUSE_WHEEL_UP      = 3,
    NUX_MOUSE_WHEEL_DOWN    = 4,
} nux_mouse_button_t;

typedef enum
{
    NUX_MOUSE_MOTION_RIGHT = 0,
    NUX_MOUSE_MOTION_LEFT  = 1,
    NUX_MOUSE_MOTION_DOWN  = 2,
    NUX_MOUSE_MOTION_UP    = 3,
    NUX_MOUSE_SCROLL_UP    = 4,
    NUX_MOUSE_SCROLL_DOWN  = 5,
} nux_mouse_axis_t;

typedef enum
{
    NUX_GAMEPAD_A              = 0,
    NUX_GAMEPAD_X              = 1,
    NUX_GAMEPAD_Y              = 2,
    NUX_GAMEPAD_B              = 3,
    NUX_GAMEPAD_DPAD_UP        = 4,
    NUX_GAMEPAD_DPAD_DOWN      = 5,
    NUX_GAMEPAD_DPAD_LEFT      = 6,
    NUX_GAMEPAD_DPAD_RIGHT     = 7,
    NUX_GAMEPAD_SHOULDER_LEFT  = 8,
    NUX_GAMEPAD_SHOULDER_RIGHT = 9,
} nux_gamepad_button_t;

typedef enum
{
    NUX_GAMEPAD_LSTICK_LEFT  = 0,
    NUX_GAMEPAD_LSTICK_RIGHT = 1,
    NUX_GAMEPAD_LSTICK_UP    = 2,
    NUX_GAMEPAD_LSTICK_DOWN  = 3,
    NUX_GAMEPAD_RSTICK_LEFT  = 4,
    NUX_GAMEPAD_RSTICK_RIGHT = 5,
    NUX_GAMEPAD_RSTICK_UP    = 6,
    NUX_GAMEPAD_RSTICK_DOWN  = 7,
    NUX_GAMEPAD_LTRIGGER     = 8,
    NUX_GAMEPAD_RTRIGGER     = 9,
} nux_gamepad_axis_t;

typedef enum
{
    NUX_CURSOR_UP    = 0,
    NUX_CURSOR_DOWN  = 1,
    NUX_CURSOR_LEFT  = 2,
    NUX_CURSOR_RIGHT = 3,
} nux_cursor_mode_t;

typedef enum
{
    NUX_CONTROLLER_MAX = 4,
} nux_input_constants_t;

typedef struct
{
    nux_input_type_t type;
    union
    {
        nux_key_t            key;
        nux_mouse_button_t   mouse_button;
        nux_mouse_axis_t     mouse_axis;
        nux_gamepad_button_t gamepad_button;
        nux_gamepad_axis_t   gamepad_axis;
    };
    union
    {
        nux_f32_t          axis_value;
        nux_button_state_t button_state;
    };
} nux_input_event_t;

typedef struct nux_inputmap_t nux_inputmap_t;

nux_inputmap_t *nux_inputmap_new(nux_arena_t *arena);
void            nux_inputmap_bind_key(nux_inputmap_t *map,
                                      const nux_c8_t *name,
                                      nux_key_t       key);
void            nux_inputmap_bind_mouse_button(nux_inputmap_t    *map,
                                               const nux_c8_t    *name,
                                               nux_mouse_button_t button);
void            nux_inputmap_bind_mouse_axis(nux_inputmap_t  *map,
                                             const nux_c8_t  *name,
                                             nux_mouse_axis_t axis,
                                             nux_f32_t        sensivity);

void      nux_input_set_map(nux_u32_t controller, nux_inputmap_t *map);
nux_b32_t nux_input_pressed(nux_u32_t controller, const nux_c8_t *name);
nux_b32_t nux_input_released(nux_u32_t controller, const nux_c8_t *name);
nux_b32_t nux_input_just_pressed(nux_u32_t controller, const nux_c8_t *name);
nux_b32_t nux_input_just_released(nux_u32_t controller, const nux_c8_t *name);
nux_f32_t nux_input_value(nux_u32_t controller, const nux_c8_t *name);
nux_v2_t  nux_input_cursor(nux_u32_t controller);
void      nux_input_set_cursor(nux_u32_t controller, nux_f32_t x, nux_f32_t y);

nux_status_t nux_controller_resize_values(nux_inputmap_t *map);

nux_status_t nux_inputmap_find_index(const nux_inputmap_t *map,
                                     const nux_c8_t       *name,
                                     nux_u32_t            *index);

void nux_input_push_event(nux_input_event_t *event);

#endif
