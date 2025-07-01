#include "internal.h"

nux_u32_t
nux_strnlen (const nux_c8_t *s, nux_u32_t n)
{
    nux_u32_t i = 0;
    for (; (i < n) && s[i]; ++i)
        ;
    return i;
}
void
nux_strncpy (nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n)
{
    nux_u32_t i = 0;
    while (i++ != n && (*dst++ = *src++))
        ;
}
nux_i32_t
nux_strncmp (const nux_c8_t *a, const nux_c8_t *b, nux_u32_t n)
{
    while (n && *a && (*a == *b))
    {
        ++a;
        ++b;
        --n;
    }
    if (n == 0)
    {
        return 0;
    }
    else
    {
        return (*(unsigned char *)a - *(unsigned char *)b);
    }
}
nux_u32_t
nux_snprintf (nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, ...)
{
#ifdef NUX_STDLIB
    va_list args;
    va_start(args, format);
    nux_u32_t cn = nux_vsnprintf(buf, n, format, args);
    va_end(args);
    return cn;
#endif
}
nux_u32_t
nux_vsnprintf (nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, va_list args)
{
#ifdef NUX_STDLIB
    return vsnprintf(buf, n, format, args);
#endif
}
