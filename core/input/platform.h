#ifndef NUX_INPUT_PLATFORM_H
#define NUX_INPUT_PLATFORM_H

#include <input/input.h>

typedef enum
{
    NUX_OS_EVENT_INPUT,
} nux_os_event_type_t;

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

typedef struct
{
    nux_os_event_type_t type;
    union
    {
        nux_input_event_t input;
    };
} nux_os_event_t;

NUX_API void nux_core_push_event(nux_os_event_t *event);

#endif
