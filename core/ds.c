#include "internal.h"

NUX_VEC_IMPL(nux_u32_vec, nux_u32_t);
NUX_VEC_IMPL(nux_render_pass_vec, nux_render_pass_t)
NUX_VEC_IMPL(nux_render_command_vec, nux_render_command_t)
NUX_VEC_IMPL(nux_object_vec, nux_object_t);

void
nux_u32_vec_fill_reversed (nux_u32_vec_t *v)
{
    for (nux_u32_t i = 0; i < (v)->capa; ++i)
    {
        *nux_u32_vec_push((v)) = (v)->capa - i - 1;
    }
}
