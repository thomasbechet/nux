#include "internal.h"

void
nux_instance_set_buttons (nux_instance_t inst,
                          nux_u32_t      player,
                          nux_u32_t      state)
{
    // TODO: validate ?
    nux_u32_t *buttons = (nux_u32_t *)(inst->memory + NUX_MAP_GAMEPAD_BUTTONS);
    buttons[player]    = state;
}
void
nux_instance_set_axis (nux_instance_t inst,
                       nux_u32_t      player,
                       nux_axis_t     axis,
                       nux_f32_t      value)
{
    // TODO: validate ?
    nux_f32_t *ax = (nux_f32_t *)(inst->memory + NUX_MAP_GAMEPAD_AXIS);
    ax[player * NUX_AXIS_MAX + axis] = value;
}

nux_u32_t
nux_button (nux_env_t env, nux_u32_t player)
{
    const nux_u32_t *buttons
        = (const nux_u32_t *)(env->inst->memory + NUX_MAP_GAMEPAD_BUTTONS);
    return buttons[player];
}
nux_f32_t
nux_axis (nux_env_t env, nux_u32_t player, nux_axis_t axis)
{
    const nux_f32_t *ax
        = (const nux_f32_t *)(env->inst->memory + NUX_MAP_GAMEPAD_AXIS);
    return ax[player * NUX_AXIS_MAX + axis];
}
