#include "internal.h"

void
nux_dsa_init (nux_dsa_t *a, nux_u32_t capacity)
{
    a->capacity = capacity;
    a->top      = capacity;
    a->bottom   = 0;
}
nux_status_t
nux_dsa_push_bottom (nux_dsa_t *a, nux_u32_t count, nux_u32_t *index)
{
    nux_check(a->bottom + count < a->top, return NUX_FAILURE);
    if (index)
    {
        *index = a->bottom;
    }
    a->bottom += count;
    return NUX_SUCCESS;
}
nux_status_t
nux_dsa_push_top (nux_dsa_t *a, nux_u32_t count, nux_u32_t *index)
{
    nux_check(a->top - count > a->bottom, return NUX_FAILURE);
    if (index)
    {
        *index = a->top - count;
    }
    a->top -= count;
    return NUX_SUCCESS;
}
void
nux_dsa_reset_bottom (nux_dsa_t *a)
{
    a->bottom = 0;
}
void
nux_dsa_reset_top (nux_dsa_t *a)
{
    a->top = a->capacity;
}
