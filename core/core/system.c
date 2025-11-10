#include "internal.h"

void
nux_system_register (nux_system_phase_t phase, nux_system_callback_t callback)
{
    nux_core_module_t *core = nux_core();
    nux_system_t      *s    = nux_system_vec_push(&core->systems);
    s->phase                = phase;
    s->callback             = callback;
}
void
nux_system_call (nux_system_phase_t phase)
{
    nux_core_module_t *core = nux_core();
    for (nux_u32_t i = 0; i < core->systems.size; ++i)
    {
        if (core->systems.data[i].phase == phase)
        {
            core->systems.data[i].callback();
        }
    }
}
