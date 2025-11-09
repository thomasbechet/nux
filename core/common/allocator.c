#include "common.h"

void *
nux_malloc (nux_allocator_t *a, nux_u32_t size)
{
    return a->alloc(a->userdata, NUX_NULL, 0, size);
}
void *
nux_realloc (nux_allocator_t *a, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    return a->alloc(a->userdata, optr, osize, nsize);
}
void
nux_free (nux_allocator_t *a, void *p, nux_u32_t osize)
{
    a->alloc(a->userdata, p, osize, 0);
}
