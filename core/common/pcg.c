#include "common.h"

nux_pcg_t
nux_pcg (nux_u64_t state, nux_u64_t incr)
{
    nux_pcg_t pcg;
    pcg.state = state;
    pcg.incr  = incr;
    nux_pcg_u32(&pcg); // Initialize first sample
    return pcg;
}
nux_u32_t
nux_pcg_u32 (nux_pcg_t *pcg)
{
    nux_u64_t old_state  = pcg->state;
    pcg->state           = old_state * 6364136223846793005 + (pcg->incr | 1);
    nux_u64_t xorshifted = ((old_state >> 18) ^ old_state) >> 27;
    nux_u64_t rot        = old_state >> 59;
    return ((xorshifted >> rot) | (xorshifted << ((-rot & 31))));
}
nux_f32_t
nux_pcg_f32 (nux_pcg_t *pcg)
{
    // generate uniformly distributed single precision number in [1, 2)
    nux_u32_t next = (nux_pcg_u32(pcg) >> 9) | 0x3f800000;
    nux_f32_t flt;
    nux_memcpy(&flt, &next, sizeof(nux_f32_t));
    return flt - 1;
}
