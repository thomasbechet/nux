#include "common.h"

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v, nux_u32_t count)
{
    nux__vec_reserve(&v->vec, count);
    for (nux_u32_t i = 0; i < count; ++i)
    {
        nux_vec_pushv(v, count - i - 1);
    }
}
