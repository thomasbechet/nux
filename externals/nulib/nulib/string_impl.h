#ifndef NU_STRING_IMPL_H
#define NU_STRING_IMPL_H

#include "string.h"
#include "memory.h"
#include "assert.h"

#ifdef NU_STDLIB
#include <stdlib.h>
#include <stdio.h>
#endif

nu_size_t
nu_strnlen (const nu_char_t *s, nu_size_t n)
{
    size_t i = 0;
    for (; (i < n) && s[i]; ++i)
        ;
    return i;
}
void
nu_strncpy (nu_char_t *dst, const nu_char_t *src, nu_size_t n)
{
    size_t i = 0;
    while (i++ != n && (*dst++ = *src++))
        ;
}
nu_int_t
nu_strncmp (const nu_char_t *a, const nu_char_t *b, nu_size_t n)
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
nu_bool_t
nu_strneq (const nu_char_t *a, const nu_char_t *b, nu_size_t n)
{
    return nu_strncmp(a, b, n) == 0;
}

nu_sv_t
nu_sv (const nu_char_t *s, nu_size_t n)
{
    return nu_sv_slice(s, nu_strnlen(s, n));
}
nu_sv_t
nu_sv_slice (const nu_char_t *s, nu_size_t n)
{
    nu_sv_t str;
    str.ptr = s;
    str.len = n;
    return str;
}
nu_sv_t
nu_sv_empty (void)
{
    return nu_sv_slice(NU_NULL, 0);
}
nu_sv_t
nu_sv_to_cstr (nu_sv_t str, nu_char_t *chars, nu_size_t n)
{
    NU_ASSERT(str.len < n);
    nu_memset(chars, 0, n);
    nu_memcpy(chars, str.ptr, NU_MIN(str.len, n - 1));
    return nu_sv(chars, str.len);
}
nu_bool_t
nu_sv_eq (nu_sv_t s1, nu_sv_t s2)
{
    return (s1.len == s2.len && nu_memcmp(s1.ptr, s2.ptr, s1.len) == 0);
}
nu_u32_t
nu_sv_hash (nu_sv_t s)
{
    return nu_hash((const nu_byte_t *)s.ptr, s.len);
}
nu_bool_t
nu_sv_next (nu_sv_t s, nu_size_t *it, nu_wchar_t *c)
{
    if (*it >= s.len)
    {
        return NU_FALSE;
    }
    *c = s.ptr[*it];
    ++(*it);
    return NU_TRUE;
}
nu_bool_t
nu_sv_to_u32 (nu_sv_t s, nu_u32_t *v)
{
#ifdef NU_STDLIB
    char       buf[32];
    nu_byte_t *nptr = NU_NULL;
    nu_sv_to_cstr(s, buf, 32);
    *v = strtoul((char *)buf, (char **)&nptr, 10);
    return !*nptr;
#endif
}
nu_bool_t
nu_sv_to_i32 (nu_sv_t s, nu_i32_t *v)
{
#ifdef NU_STDLIB
    nu_char_t  buf[32];
    nu_char_t *nptr = NU_NULL;
    nu_sv_to_cstr(s, buf, 32);
    *v = strtol(buf, &nptr, 10);
    return !*nptr;
#endif
}
nu_bool_t
nu_sv_to_f32 (nu_sv_t s, nu_f32_t *v)
{
#ifdef NU_STDLIB
    nu_char_t  buf[32];
    nu_char_t *nptr = NU_NULL;
    nu_sv_to_cstr(s, buf, 32);
    *v = strtof(buf, &nptr);
    return !*nptr;
#endif
}
nu_sv_t
nu_sv_fmt (nu_char_t *buf, nu_size_t n, const nu_char_t *format, ...)
{
#ifdef NU_STDLIB
    va_list args;
    va_start(args, format);
    nu_sv_t r = nu_sv_vfmt(buf, n, format, args);
    va_end(args);
    return r;
#endif
}
nu_sv_t
nu_sv_vfmt (nu_char_t *buf, nu_size_t n, const nu_char_t *format, va_list args)
{
#ifdef NU_STDLIB
    nu_size_t k = vsnprintf(buf, n, format, args);
    return nu_sv_slice(buf, k);
#endif
}
nu_sv_t
nu_sv_join (nu_char_t *buf, nu_size_t n, nu_sv_t a, nu_sv_t b)
{
    return nu_sv_fmt(buf, n, NU_SV_FMT NU_SV_FMT, NU_SV_ARGS(a), NU_SV_ARGS(b));
}

const nu_char_t *
nu_enum_to_cstr (nu_u32_t v, const nu_enum_name_map_t *map)
{
    const nu_enum_name_map_t *current = map;
    while (current->s)
    {
        if (current->v == v)
        {
            return current->s;
        }
        ++current;
    }
    return NU_NULL;
}
nu_u32_t
nu_sv_to_enum (nu_sv_t sv, const nu_enum_name_map_t *map, nu_bool_t *found)
{
    *found                            = NU_FALSE;
    const nu_enum_name_map_t *current = map;
    while (current->s)
    {
        if (nu_sv_eq(nu_sv_slice(current->s, current->l), sv))
        {
            *found = NU_TRUE;
            return current->v;
        }
        ++current;
    }
    return 0;
}

#endif
