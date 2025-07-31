#include "internal.h"

NUX_VEC_IMPL(nux_u32_vec, nux_u32_t);
NUX_VEC_IMPL(nux_gpu_command_vec, nux_gpu_command_t)
NUX_VEC_IMPL(nux_ecs_bitset, nux_ecs_mask_t);
NUX_VEC_IMPL(nux_ecs_chunk_vec, void *);
NUX_VEC_IMPL(nux_ecs_container_vec, nux_ecs_container_t);
NUX_VEC_IMPL(nux_ecs_component_vec, nux_ecs_component_t);
NUX_POOL_IMPL(nux_resource_pool, nux_resource_t)
NUX_POOL_IMPL(nux_arena_pool, nux_arena_t)

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v)
{
    for (nux_u32_t i = 0; i < (v)->capa; ++i)
    {
        *nux_u32_vec_push((v)) = (v)->capa - i - 1;
    }
}
