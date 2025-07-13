#include "internal.h"

NUX_VEC_IMPL(nux_u32_vec, nux_u32_t);
NUX_VEC_IMPL(nux_gpu_command_vec, nux_gpu_command_t)
NUX_POOL_IMPL(nux_ref_pool, nux_ref_t)
NUX_POOL_IMPL(nux_arena_pool, nux_arena_t)
NUX_POOL_IMPL(nux_node_pool, nux_node_t)
NUX_POOL_IMPL(nux_component_pool, nux_component_t)

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v)
{
    for (nux_u32_t i = 0; i < (v)->capa; ++i)
    {
        *nux_u32_vec_push((v)) = (v)->capa - i - 1;
    }
}
