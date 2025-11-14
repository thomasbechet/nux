#include "common.h"

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v)
{
    for (nux_u32_t i = 0; i < (v)->capa; ++i)
    {
        nux_vec_pushv(v, v->capa - i - 1);
    }
}
