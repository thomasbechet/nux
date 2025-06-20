#include "internal.h"

void
nux_input_pre_update (nux_env_t *env)
{
    nux_memcpy(env->inst->buttons_prev,
               env->inst->buttons,
               sizeof(env->inst->buttons));
    nux_memcpy(env->inst->axis_prev, env->inst->axis, sizeof(env->inst->axis));
}

void
nux_instance_set_buttons (nux_instance_t *inst,
                          nux_u32_t       player,
                          nux_u32_t       state)
{
    // TODO: validate ?
    inst->buttons[player] = state;
}
void
nux_instance_set_axis (nux_instance_t *inst,
                       nux_u32_t       player,
                       nux_axis_t      axis,
                       nux_f32_t       value)
{
    // TODO: validate ?
    inst->axis[player * NUX_AXIS_MAX + axis] = value;
}

nux_u32_t
nux_input_button (nux_env_t *env, nux_u32_t player)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(env->inst->buttons), return 0);
    return env->inst->buttons[player];
}
nux_f32_t
nux_input_axis (nux_env_t *env, nux_u32_t player, nux_axis_t axis)
{
    NUX_CHECK(player < NUX_AXIS_MAX, return 0);
    return env->inst->axis[player * NUX_AXIS_MAX + axis];
}
nux_b32_t
nux_button_pressed (nux_env_t *env, nux_u32_t player, nux_button_t button)
{
    return (nux_input_button(env, player) & button) ? 1 : 0;
}
nux_b32_t
nux_button_released (nux_env_t *env, nux_u32_t player, nux_button_t button)
{
    return !nux_button_pressed(env, player, button);
}
nux_b32_t
nux_button_just_pressed (nux_env_t *env, nux_u32_t player, nux_button_t button)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(env->inst->buttons_prev), return 0);
    nux_u32_t stat = env->inst->buttons[player];
    nux_u32_t prev = env->inst->buttons_prev[player];
    return (stat ^ prev) & button && nux_button_pressed(env, player, button);
}
nux_b32_t
nux_button_just_released (nux_env_t *env, nux_u32_t player, nux_button_t button)
{
    NUX_CHECK(player < NUX_ARRAY_SIZE(env->inst->buttons_prev), return 0);
    nux_u32_t stat = env->inst->buttons[player];
    nux_u32_t prev = env->inst->buttons_prev[player];
    return (stat ^ prev) & button && nux_button_released(env, player, button);
}
