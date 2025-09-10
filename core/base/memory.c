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
nux_u32_t
nux_u32_le (nux_u32_t v)
{
    if (NUX_BIG_ENDIAN)
    {
        return ((v >> 24) & 0xff) |      // move byte 3 to byte 0
               ((v << 8) & 0xff0000) |   // move byte 1 to byte 2
               ((v >> 8) & 0xff00) |     // move byte 2 to byte 1
               ((v << 24) & 0xff000000); // byte 0 to byte 3
    }
    else
    {
        return v;
    }
}
nux_f32_t
nux_f32_le (nux_f32_t v)
{
    nux_u32_t a;
    nux_memcpy(&a, &v, sizeof(a));
    a = nux_u32_le(a);
    nux_memcpy(&v, &a, sizeof(a));
    return v;
}
nux_u32_t
nux_hash (const void *p, nux_u32_t s)
{
    const nux_u32_t FNV1A_HASH_32  = 0x811c9dc5;
    const nux_u32_t FNV1A_PRIME_32 = 0x01000193;
    const nux_u8_t *b              = p;
    nux_u32_t       hash           = FNV1A_HASH_32;
    nux_u32_t       i              = 0;
    while (i < s)
    {
        hash ^= b[i];
        hash *= FNV1A_PRIME_32;
        ++i;
    }
    return hash;
}
nux_c8_t *
nux_mem_human (double size, nux_c8_t buf[10])
{
    int         i = 0;
    const char *units[]
        = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
    while (size > 1024)
    {
        size /= 1024;
        i++;
    }
    nux_snprintf(buf, 10, "%.*f %s", i, size, units[i]);
    return buf;
}
