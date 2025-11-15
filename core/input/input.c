#include "internal.h"

static nux_input_module_t _module;

static void
dispatch_event (const nux_input_event_t *event)
{
    for (nux_u32_t i = 0; i < nux_array_size(_module.controllers); ++i)
    {
        nux_controller_t *ctrl = _module.controllers + i;
        nux_inputmap_t   *map
            = nux_resource_get(NUX_RESOURCE_INPUTMAP, ctrl->inputmap);
        nux_check(map, continue);
        for (nux_u32_t j = 0; j < map->entries.size; ++j)
        {
            nux_inputmap_entry_t *entry = map->entries.data + j;
            if (entry->type == event->type)
            {
                switch (entry->type)
                {
                    case NUX_INPUT_UNMAPPED:
                        break;
                    case NUX_INPUT_KEY: {
                        if (entry->key == event->key)
                        {
                            ctrl->inputs.data[j] = event->button_state;
                        }
                    }
                    break;
                    case NUX_INPUT_MOUSE_BUTTON: {
                        if (entry->mouse_button == event->mouse_button)
                        {
                            ctrl->inputs.data[j] = event->button_state;
                        }
                    }
                    break;
                    case NUX_INPUT_MOUSE_AXIS: {
                        if (entry->mouse_axis == event->mouse_axis)
                        {
                            ctrl->inputs.data[j]
                                = event->axis_value * entry->sensivity;
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
    nux_check(controller < nux_array_size(_module.controllers), goto error);
    nux_controller_t *ctrl = _module.controllers + controller;
    nux_inputmap_t   *map
        = nux_resource_get(NUX_RESOURCE_INPUTMAP, ctrl->inputmap);
    nux_check(map, goto error);
    nux_u32_t index;
    nux_check(nux_inputmap_find_index(map, name, &index), goto error);
    nux_assert(index < ctrl->inputs.size);
    *value      = ctrl->inputs.data[index];
    *prev_value = ctrl->prev_inputs.data[index];
    return;
error:
    *value      = default_value;
    *prev_value = default_value;
}

static void
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
        dispatch_event(_module.input_events.data + i);
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
        //      j < nux_array_size(controller->cursor_motion_buttons);
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
}
static void
module_post_update (void)
{
    nux_vec_clear(&_module.input_events);
}
static nux_status_t
module_init (void)
{
    nux_arena_t *a = nux_arena_core();

    nux_system_register(NUX_SYSTEM_PRE_UPDATE, module_update);
    nux_system_register(NUX_SYSTEM_POST_UPDATE, module_post_update);

    // Register resources
    nux_resource_register(
        NUX_RESOURCE_INPUTMAP,
        (nux_resource_info_t) { .name = "inputmap",
                                .size = sizeof(nux_inputmap_t) });

    // Allocate events queue
    nux_vec_init_capa(&_module.input_events, a, DEFAULT_INPUT_EVENT_SIZE);

    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller = _module.controllers + i;
        controller->inputmap         = NUX_NULL;
        nux_vec_init_capa(
            &controller->inputs, a, DEFAULT_CONTROLLER_INPUT_SIZE);
        nux_vec_init_capa(
            &controller->prev_inputs, a, DEFAULT_CONTROLLER_INPUT_SIZE);
    }

    return NUX_SUCCESS;
}
void
nux_input_module_register (void)
{
    nux_module_register((nux_module_info_t) { .name = "input",
                                              .data = &_module,
                                              .init = module_init,
                                              .free = nullptr });
}

void
nux_core_push_event (nux_os_event_t *event)
{
    if (event->type == NUX_OS_EVENT_INPUT)
    {
        nux_vec_pushv(&_module.input_events, event->input);
    }
}

void
nux_controller_resize_values (nux_inputmap_t *map)
{
    for (nux_u32_t i = 0; i < nux_array_size(_module.controllers); ++i)
    {
        nux_controller_t *controller = _module.controllers + i;
        if (controller->inputmap == nux_resource_rid(map))
        {
            nux_vec_resize(&controller->inputs, map->entries.size);
        }
    }
}

nux_status_t
nux_input_set_inputmap (nux_u32_t controller, nux_inputmap_t *map)
{
    nux_check(controller < nux_array_size(_module.controllers),
              return NUX_FAILURE);
    nux_controller_t *ctrl = _module.controllers + controller;
    ctrl->inputmap         = nux_resource_rid(map);
    nux_vec_resize(&ctrl->inputs, map->entries.size);
    nux_vec_resize(&ctrl->prev_inputs, map->entries.size);
    return NUX_SUCCESS;
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
    nux_check(controller < nux_array_size(_module.controllers),
              return NUX_V2_ZEROS);
    return _module.controllers[controller].cursor;
}
void
nux_input_wrap_cursor (nux_u32_t controller, nux_f32_t x, nux_f32_t y)
{
    nux_check(controller < nux_array_size(_module.controllers), return);
    _module.controllers[controller].cursor = nux_v2(x, y);
}
