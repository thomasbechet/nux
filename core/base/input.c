#include "internal.h"

static void
dispatch_key (nux_key_t key, nux_b32_t pressed)
{
    nux_base_module_t *module = nux_base_module();
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(module->controllers); ++i)
    {
        nux_controller_t *ctrl = module->controllers + i;
        nux_inputmap_t   *map
            = nux_resource_get(NUX_RESOURCE_INPUTMAP, ctrl->inputmap);
        if (map)
        {
            for (nux_u32_t j = 0; j < map->entries.size; ++j)
            {
                nux_inputmap_entry_t *entry = map->entries.data + j;
                if (entry->type == NUX_INPUT_KEY && entry->key == key)
                {
                    ctrl->inputs.data[j] = pressed ? entry->value : 0;
                }
            }
        }
    }
}

void
nux_input_update (void)
{
    nux_base_module_t *module = nux_base_module();

    // Keep previous state
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        // Keep previous state
        nux_controller_t *controller = module->controllers + i;
        nux_memcpy(controller->prev_inputs.data,
                   controller->inputs.data,
                   sizeof(*controller->inputs.data)
                       * controller->prev_inputs.size);
        controller->cursor_prev = controller->cursor;
    }

    // Dispatch input events
    for (nux_u32_t i = 0; i < module->events.size; ++i)
    {
        nux_os_event_t *event = module->events.data + i;
        switch (event->type)
        {
            case NUX_OS_EVENT_KEY_PRESSED:
                dispatch_key(event->key, NUX_INPUT_PRESSED);
                break;
            case NUX_OS_EVENT_KEY_RELEASED:
                dispatch_key(event->key, NUX_INPUT_RELEASED);
                break;
            default:
                break;
        }
    }

    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller = module->controllers + i;

        // Validate update
        controller->cursor = nux_v2_min(
            nux_v2_max(controller->cursor, NUX_V2_ZEROS), NUX_V2_ONES);

        // Integrate cursor motion
        if (controller->mode == NUX_CONTROLLER_MODE_CURSOR)
        {
            nux_f32_t speed
                = nux_time_delta() * controller->cursor_motion_speed;
            const nux_v2_t motion[] = {
                nux_v2(1, 0),
                nux_v2(-1, 0),
                nux_v2(0, 1),
                nux_v2(0, -1),
            };
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
        }
    }
}

nux_v2_t
nux_cursor_get (nux_u32_t controller)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(nux_base_module()->controllers),
              return NUX_V2_ZEROS);
    return nux_base_module()->controllers[controller].cursor;
}
void
nux_cursor_set (nux_u32_t controller, nux_f32_t x, nux_f32_t y)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(nux_base_module()->controllers),
              return);
    nux_base_module()->controllers[controller].cursor = nux_v2(x, y);
}
nux_f32_t
nux_cursor_x (nux_u32_t controller)
{
    return nux_cursor_get(controller).x;
}
nux_f32_t
nux_cursor_y (nux_u32_t controller)
{
    return nux_cursor_get(controller).y;
}

static void
controller_get_input_value (nux_u32_t       controller,
                            const nux_c8_t *name,
                            nux_f32_t      *value,
                            nux_f32_t      *prev_value,
                            nux_f32_t       default_value)
{
    nux_base_module_t *module = nux_base_module();
    NUX_CHECK(controller < NUX_ARRAY_SIZE(module->controllers), goto error);
    nux_controller_t *ctrl = module->controllers + controller;
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

void
nux_input_set_map (nux_u32_t controller, nux_inputmap_t *map)
{
    nux_base_module_t *module = nux_base_module();
    NUX_CHECK(controller < NUX_ARRAY_SIZE(module->controllers), return);
    nux_controller_t *ctrl = module->controllers + controller;
    ctrl->inputmap         = nux_resource_rid(map);
    nux_f32_vec_resize(&ctrl->inputs, map->entries.size);
    nux_f32_vec_resize(&ctrl->prev_inputs, map->entries.size);
}
nux_b32_t
nux_input_pressed (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(
        controller, name, &value, &prev, NUX_INPUT_RELEASED);
    return value > NUX_INPUT_RELEASED;
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
        controller, name, &value, &prev, NUX_INPUT_RELEASED);
    return value != prev;
}
nux_b32_t
nux_input_just_released (nux_u32_t controller, const nux_c8_t *name)
{
    return !nux_input_just_pressed(controller, name);
}
nux_f32_t
nux_input_value (nux_u32_t controller, const nux_c8_t *name)
{
    nux_f32_t value, prev;
    controller_get_input_value(controller, name, &value, &prev, 0);
    return value;
}
