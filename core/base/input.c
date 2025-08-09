#include "nux_internal.h"

void
nux_input_update (nux_ctx_t *ctx)
{
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        // Keep previous state
        nux_controller_t *controller = ctx->controllers + i;
        controller->buttons_prev     = controller->buttons;
        nux_memcpy(
            controller->axis_prev, controller->axis, sizeof(controller->axis));
        controller->cursor_prev = controller->cursor;

        // Aquire inputs from os
        nux_os_input_update(ctx->userdata,
                            i,
                            &controller->buttons,
                            controller->axis,
                            controller->cursor.data);

        // Integrate cursor motion
        if (controller->mode == NUX_CONTROLLER_MODE_CURSOR)
        {
            nux_f32_t speed
                = nux_time_delta(ctx) * controller->cursor_motion_speed;
            const nux_v2_t motion[] = {
                nux_v2(1, 0),
                nux_v2(-1, 0),
                nux_v2(0, 1),
                nux_v2(0, -1),
            };
            for (nux_u32_t j = 0;
                 j < NUX_ARRAY_SIZE(controller->cursor_motion_buttons);
                 ++j)
            {
                if (controller->buttons & controller->cursor_motion_buttons[j])
                {
                    controller->cursor = nux_v2_add(
                        controller->cursor, nux_v2_muls(motion[j], speed));
                }
            }
            nux_v2_t axis_motion
                = nux_v2(controller->axis[controller->cursor_motion_axis[0]],
                         controller->axis[controller->cursor_motion_axis[1]]);
            controller->cursor = nux_v2_add(controller->cursor,
                                            nux_v2_muls(axis_motion, speed));
        }
    }
}

nux_u32_t
nux_button_state (nux_ctx_t *ctx, nux_u32_t controller)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(ctx->controllers), return 0);
    return ctx->controllers[controller].buttons;
}
nux_b32_t
nux_button_pressed (nux_ctx_t *ctx, nux_u32_t controller, nux_button_t button)
{
    return (nux_button_state(ctx, controller) & button) ? NUX_TRUE : NUX_FALSE;
}
nux_b32_t
nux_button_released (nux_ctx_t *ctx, nux_u32_t controller, nux_button_t button)
{
    return !nux_button_pressed(ctx, controller, button);
}
nux_b32_t
nux_button_just_pressed (nux_ctx_t   *ctx,
                         nux_u32_t    controller,
                         nux_button_t button)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(ctx->controllers), return 0);
    nux_u32_t state = ctx->controllers[controller].buttons;
    nux_u32_t prev  = ctx->controllers[controller].buttons_prev;
    return (state ^ prev) & button
           && nux_button_pressed(ctx, controller, button);
}
nux_b32_t
nux_button_just_released (nux_ctx_t   *ctx,
                          nux_u32_t    controller,
                          nux_button_t button)
{
    NUX_CHECK(controller < NUX_ARRAY_SIZE(ctx->controllers), return 0);
    nux_u32_t stat = ctx->controllers[controller].buttons;
    nux_u32_t prev = ctx->controllers[controller].buttons_prev;
    return (stat ^ prev) & button
           && nux_button_released(ctx, controller, button);
}
nux_f32_t
nux_axis (nux_ctx_t *ctx, nux_u32_t controller, nux_axis_t axis)
{
    NUX_CHECK(controller < NUX_AXIS_MAX, return 0);
    return ctx->controllers[controller].axis[axis];
}
nux_f32_t
nux_cursor_x (nux_ctx_t *ctx, nux_u32_t controller)
{
    return ctx->controllers[controller].cursor.x;
}
nux_f32_t
nux_cursor_y (nux_ctx_t *ctx, nux_u32_t controller)
{
    return ctx->controllers[controller].cursor.y;
}
void
nux_cursor_set (nux_ctx_t *ctx, nux_u32_t controller, nux_f32_t x, nux_f32_t y)
{
    ctx->controllers[controller].cursor = nux_v2(x, y);
}
