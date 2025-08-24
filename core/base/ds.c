#include "internal.h"

NUX_VEC_IMPL(nux_u32_vec, nux_u32_t);
NUX_POOL_IMPL(nux_resource_pool, nux_resource_entry_t)

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v)
{
    for (nux_u32_t i = 0; i < (v)->capa; ++i)
    {
        *nux_u32_vec_push((v)) = (v)->capa - i - 1;
    }
}
