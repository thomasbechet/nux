#include "internal.h"

void
nux_input_pre_update (nux_ctx_t *ctx)
{
    nux_memcpy(ctx->buttons_prev, ctx->buttons, sizeof(ctx->buttons));
    nux_memcpy(ctx->axis_prev, ctx->axis, sizeof(ctx->axis));
}

void
nux_instance_set_buttons (nux_ctx_t *ctx, nux_u32_t player, nux_u32_t state)
{
    // TODO: validate ?
    ctx->buttons[player] = state;
}
void
nux_instance_set_axis (nux_ctx_t *ctx,
                       nux_u32_t  player,
                       nux_axis_t axis,
                       nux_f32_t  value)
{
    // TODO: validate ?
    ctx->axis[player * NUX_AXIS_MAX + axis] = value;
}

nux_u32_t
nux_input_button (nux_ctx_t *ctx, nux_u32_t player)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(ctx->buttons), return 0);
    return ctx->buttons[player];
}
nux_f32_t
nux_input_axis (nux_ctx_t *ctx, nux_u32_t player, nux_axis_t axis)
{
    NUX_CHECK(player < NUX_AXIS_MAX, return 0);
    return ctx->axis[player * NUX_AXIS_MAX + axis];
}
nux_b32_t
nux_button_pressed (nux_ctx_t *ctx, nux_u32_t player, nux_button_t button)
{
    return (nux_input_button(ctx, player) & button) ? 1 : 0;
}
nux_b32_t
nux_button_released (nux_ctx_t *ctx, nux_u32_t player, nux_button_t button)
{
    return !nux_button_pressed(ctx, player, button);
}
nux_b32_t
nux_button_just_pressed (nux_ctx_t *ctx, nux_u32_t player, nux_button_t button)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(ctx->buttons_prev), return 0);
    nux_u32_t stat = ctx->buttons[player];
    nux_u32_t prev = ctx->buttons_prev[player];
    return (stat ^ prev) & button && nux_button_pressed(ctx, player, button);
}
nux_b32_t
nux_button_just_released (nux_ctx_t *ctx, nux_u32_t player, nux_button_t button)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(ctx->buttons_prev), return 0);
    nux_u32_t stat = ctx->buttons[player];
    nux_u32_t prev = ctx->buttons_prev[player];
    return (stat ^ prev) & button && nux_button_released(ctx, player, button);
}
