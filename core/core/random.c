#include "internal.h"

nux_u32_t
nux_random (void)
{
    return nux_pcg_u32(nux_base_pcg());
}
nux_f32_t
nux_random01 (void)
{
    return nux_pcg_f32(nux_base_pcg());
}
