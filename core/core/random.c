#include "internal.h"

nux_u32_t
nux_random_next (void)
{
    return nux_pcg_u32(&nux_core()->pcg);
}
nux_f32_t
nux_random_nextf (void)
{
    return nux_pcg_f32(&nux_core()->pcg);
}
