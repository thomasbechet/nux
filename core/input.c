#include "internal.h"

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
    return env->inst->buttons[player];
}
nux_f32_t
nux_input_axis (nux_env_t *env, nux_u32_t player, nux_axis_t axis)
{
    return env->inst->axis[player * NUX_AXIS_MAX + axis];
}
