#ifndef NUX_INPUT_INTERNAL_H
#define NUX_INPUT_INTERNAL_H

#include <input/input.h>

NUX_VEC_DEFINE(nux_input_event_vec, nux_input_event_t);

typedef enum
{
    DEFAULT_INPUT_EVENT_SIZE      = 32,
    DEFAULT_CONTROLLER_INPUT_SIZE = 16,
} nux_input_defaults_t;

typedef struct
{
    const nux_c8_t  *name;
    nux_input_type_t type;
    union
    {
        nux_key_t            key;
        nux_mouse_button_t   mouse_button;
        nux_mouse_axis_t     mouse_axis;
        nux_gamepad_button_t gamepad_button;
        nux_gamepad_axis_t   gamepad_axis;
    };
    nux_f32_t sensivity;
} nux_inputmap_entry_t;

NUX_VEC_DEFINE(nux_inputmap_entry_vec, nux_inputmap_entry_t);

struct nux_inputmap_t
{
    nux_inputmap_entry_vec_t entries;
    nux_u32_t                cursor_motions[4];
};

typedef struct
{
    nux_v2_t cursor;
    nux_v2_t cursor_prev;

    nux_rid_t     inputmap;
    nux_f32_vec_t inputs;
    nux_f32_vec_t prev_inputs;
} nux_controller_t;

typedef struct
{
    nux_controller_t      controllers[NUX_CONTROLLER_MAX];
    nux_input_event_vec_t input_events;
} nux_input_module_t;

void nux_input_module_register(void);

#endif
