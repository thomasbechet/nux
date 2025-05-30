#include "internal.h"

nux_i32_t
nux_memcmp (const void *p0, const void *p1, nux_u32_t n)
{
    const nux_u8_t *b0 = (const nux_u8_t *)p0;
    const nux_u8_t *b1 = (const nux_u8_t *)p1;
    while (n-- > 0)
    {
        if (*b0++ != *b1++)
        {
            return b0[-1] < b1[-1] ? -1 : 1;
        }
    }
    return 0;
}
void *
nux_memset (void *dst, nux_u32_t c, nux_u32_t n)
{
    if (n != 0)
    {
        nux_u8_t *d = (nux_u8_t *)dst;
        do
        {
            *d++ = (nux_u8_t)c;
        } while (--n != 0);
    }
    return (dst);
}
void
nux_memcpy (void *dst, const void *src, nux_u32_t n)
{
    nux_u8_t       *u8_dst = (nux_u8_t *)dst;
    const nux_u8_t *u8_src = (const nux_u8_t *)src;
    for (nux_u32_t i = 0; i < n; ++i)
    {
        u8_dst[i] = u8_src[i];
    }
}
void
nux_memswp (void *a, void *b, nux_u32_t n)
{
    nux_u8_t *a_swap = (nux_u8_t *)a, *b_swap = (nux_u8_t *)b;
    nux_u8_t *a_end = (nux_u8_t *)a + n;

    while (a_swap < a_end)
    {
        nux_u8_t temp = *a_swap;
        *a_swap       = *b_swap;
        *b_swap       = temp;
        a_swap++, b_swap++;
    }
}
void *
nux_memalign (void *ptr, nux_u32_t align)
{
    NUX_ASSERT(align > 0);
    return (void *)(((nux_intptr_t)ptr + align - 1) & ~(align - 1));
}
