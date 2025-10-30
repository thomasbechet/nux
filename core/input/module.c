#include "internal.h"

static nux_input_module_t _module;

NUX_VEC_IMPL(nux_inputmap_entry_vec, nux_inputmap_entry_t);

static void
dispatch_event (const nux_os_event_t *event)
{
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(_module.controllers); ++i)
    {
        nux_controller_t *ctrl = _module.controllers + i;
        nux_inputmap_t   *map
            = nux_resource_get(NUX_RESOURCE_INPUTMAP, ctrl->inputmap);
        NUX_CHECK(map, continue);
        for (nux_u32_t j = 0; j < map->entries.size; ++j)
        {
            nux_inputmap_entry_t *entry = map->entries.data + j;
            if (entry->type == event->input.type)
            {
                switch (entry->type)
                {
                    case NUX_INPUT_UNMAPPED:
                        break;
                    case NUX_INPUT_KEY: {
                        if (entry->key == event->input.key)
                        {
                            ctrl->inputs.data[j] = event->input.button_state;
                        }
                    }
                    break;
                    case NUX_INPUT_MOUSE_BUTTON: {
                        if (entry->mouse_button == event->input.mouse_button)
                        {
                            ctrl->inputs.data[j] = event->input.button_state;
                        }
                    }
                    break;
                    case NUX_INPUT_MOUSE_AXIS: {
                        if (entry->mouse_axis == event->input.mouse_axis)
                        {
                            ctrl->inputs.data[j]
                                = event->input.axis_value * entry->sensivity;
                        }
                    }
                    break;
                    case NUX_INPUT_GAMEPAD_BUTTON:
                    case NUX_INPUT_GAMEPAD_AXIS:
                        break;
                }
            }
        }
    }
}
static void
controller_get_input_value (nux_u32_t       controller,
                            const nux_c8_t *name,
                            nux_f32_t      *value,
                            nux_f32_t      *prev_value,
                            nux_f32_t       default_value)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(_module.controllers), goto error);
    nux_controller_t *ctrl = _module.controllers + controller;
    nux_inputmap_t   *map
        = nux_resource_get(NUX_RESOURCE_INPUTMAP, ctrl->inputmap);
    NUX_CHECK(map, goto error);
    nux_u32_t index;
    NUX_CHECK(nux_inputmap_find_index(map, name, &index), goto error);
    NUX_ASSERT(index < ctrl->inputs.size);
    *value      = ctrl->inputs.data[index];
    *prev_value = ctrl->prev_inputs.data[index];
    return;
error:
    *value      = default_value;
    *prev_value = default_value;
}

static nux_status_t
module_init (void)
{
    nux_resource_type_t *type;
    type = nux_resource_register(
        NUX_RESOURCE_INPUTMAP, sizeof(nux_inputmap_t), "inputmap");

    // Allocate events queue
    NUX_CHECK(nux_os_event_vec_init_capa(nux_arena_core(),
                                         DEFAULT_INPUT_EVENT_SIZE,
                                         &_module.input_events),
              return NUX_FAILURE);

    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller = _module.controllers + i;
        controller->inputmap         = NUX_NULL;
        nux_f32_vec_init_capa(nux_arena_core(),
                              DEFAULT_CONTROLLER_INPUT_SIZE,
                              &controller->inputs);
        nux_f32_vec_init_capa(nux_arena_core(),
                              DEFAULT_CONTROLLER_INPUT_SIZE,
                              &controller->prev_inputs);
    }

    return NUX_SUCCESS;
}
static nux_status_t
module_update (void)
{
    // Keep previous state
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        // Keep previous state
        nux_controller_t *controller = _module.controllers + i;
        nux_memcpy(controller->prev_inputs.data,
                   controller->inputs.data,
                   sizeof(*controller->inputs.data)
                       * controller->prev_inputs.size);
        controller->cursor_prev = controller->cursor;
    }

    // Dispatch input events
    for (nux_u32_t i = 0; i < _module.input_events.size; ++i)
    {
        nux_os_event_t *event = _module.input_events.data + i;
        if (event->type == NUX_OS_EVENT_INPUT)
        {
            dispatch_event(event);
        }
    }

    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller = _module.controllers + i;

        // Validate update
        controller->cursor = nux_v2_min(
            nux_v2_max(controller->cursor, NUX_V2_ZEROS), NUX_V2_ONES);

        // Integrate cursor motion
        // if (controller->mode == NUX_CONTROLLER_MODE_CURSOR)
        // {
        // nux_f32_t speed
        //     = nux_time_delta() * controller->cursor_motion_speed;
        // const nux_v2_t motion[] = {
        //     nux_v2(1, 0),
        //     nux_v2(-1, 0),
        //     nux_v2(0, 1),
        //     nux_v2(0, -1),
        // };
        // for (nux_u32_t j = 0;
        //      j < NUX_ARRAY_SIZE(controller->cursor_motion_buttons);
        //      ++j)
        // {
        //     if (controller->buttons &
        //     controller->cursor_motion_buttons[j])
        //     {
        //         controller->cursor = nux_v2_add(
        //             controller->cursor, nux_v2_muls(motion[j], speed));
        //     }
        // }
        // nux_v2_t axis_motion
        //     = nux_v2(controller->axis[controller->cursor_motion_axis[0]],
        //              controller->axis[controller->cursor_motion_axis[1]]);
        // controller->cursor = nux_v2_add(controller->cursor,
        //                                 nux_v2_muls(axis_motion, speed));
        // }
    }

    return NUX_SUCCESS;
}
static nux_status_t
module_post_update (void)
{
    nux_os_event_vec_clear(&_module.input_events);
    return NUX_SUCCESS;
}
static nux_status_t
module_on_event (nux_os_event_t *event)
{
    if (event->type == NUX_OS_EVENT_INPUT)
    {
        nux_os_event_vec_pushv(&_module.input_events, *event);
    }
    return NUX_SUCCESS;
}
const nux_module_t *
nux_input_module_info (void)
{
    static nux_module_t info = {
        .name        = "input",
        .size        = sizeof(_module),
        .data        = &_module,
        .init        = module_init,
        .update      = module_update,
        .post_update = module_post_update,
        .on_event    = module_on_event,
    };
    return &info;
}

nux_status_t
nux_controller_resize_values (nux_inputmap_t *map)
{
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(_module.controllers); ++i)
    {
        nux_controller_t *controller = _module.controllers + i;
        if (controller->inputmap == nux_resource_rid(map))
        {
            NUX_CHECK(
                nux_f32_vec_resize(&controller->inputs, map->entries.size),
                return NUX_FAILURE);
        }
    }
    return NUX_SUCCESS;
}

void
nux_input_set_map (nux_u32_t controller, nux_inputmap_t *map)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(_module.controllers), return);
    nux_controller_t *ctrl = _module.controllers + controller;
    ctrl->inputmap         = nux_resource_rid(map);
    nux_f32_vec_resize(&ctrl->inputs, map->entries.size);
    nux_f32_vec_resize(&ctrl->prev_inputs, map->entries.size);
}
nux_b32_t
nux_input_pressed (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(
        controller, name, &value, &prev, NUX_BUTTON_RELEASED);
    return value > NUX_BUTTON_RELEASED;
}
nux_b32_t
nux_input_released (nux_u32_t controller, const nux_c8_t *name)
{
    return !nux_input_pressed(controller, name);
}
nux_b32_t
nux_input_just_pressed (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(
        controller, name, &value, &prev, NUX_BUTTON_RELEASED);
    return value > NUX_BUTTON_RELEASED && prev <= NUX_BUTTON_RELEASED;
}
nux_b32_t
nux_input_just_released (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(
        controller, name, &value, &prev, NUX_BUTTON_RELEASED);
    return value <= NUX_BUTTON_RELEASED && prev > NUX_BUTTON_RELEASED;
}
nux_f32_t
nux_input_value (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(controller, name, &value, &prev, 0);
    return value;
}
nux_v2_t
nux_input_cursor (nux_u32_t controller)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(_module.controllers),
              return NUX_V2_ZEROS);
    return _module.controllers[controller].cursor;
}
void
nux_input_set_cursor (nux_u32_t controller, nux_f32_t x, nux_f32_t y)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(_module.controllers), return);
    _module.controllers[controller].cursor = nux_v2(x, y);
}
