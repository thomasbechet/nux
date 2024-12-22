#ifndef NU_STRING_IMPL_H
#define NU_STRING_IMPL_H

#include <nulib/string.h>
#include <nulib/memory.h>
#include <nulib/assert.h>

#ifdef NU_STDLIB
#include <stdlib.h>
#include <stdio.h>
#endif

static nu_size_t
nu__cstr_len (const nu_byte_t *str)
{
    const nu_byte_t *p = str;
    while (*p)
    {
        p++;
    }
    return p - str;
}
nu_size_t
nu_cstr_len (const nu_byte_t *str, nu_size_t maxlen)
{
    const nu_byte_t *p = str;
    while (maxlen-- > 0 && *p)
    {
        p++;
    }
    return p - str;
}
nu_str_t
nu_str (nu_byte_t *bytes, nu_size_t n)
{
    nu_str_t str;
    str.data = bytes;
    str.size = n;
    return str;
}
nu_str_t
nu_str_from_cstr (nu_byte_t *s)
{
    return nu_str(s, nu__cstr_len(s));
}
void
nu_str_to_cstr (nu_str_t str, nu_byte_t *chars, nu_size_t n)
{
    NU_ASSERT(str.size < n);
    nu_memset(chars, 0, n);
    nu_memcpy(chars, str.data, NU_MIN(str.size, n - 1));
}
nu_bool_t
nu_str_eq (nu_str_t s1, nu_str_t s2)
{
    return (s1.size == s2.size && nu_memcmp(s1.data, s2.data, s1.size) == 0);
}
nu_u32_t
nu_str_hash (nu_str_t s)
{
    static const nu_u32_t FNV1A_HASH_32  = 0x811c9dc5;
    static const nu_u32_t FNV1A_PRIME_32 = 0x01000193;
    nu_u32_t              hash           = FNV1A_HASH_32;
    nu_size_t             i              = 0;
    while (i < s.size)
    {
        hash ^= s.data[i];
        hash *= FNV1A_PRIME_32;
        ++i;
    }
    return hash;
}
nu_bool_t
nu_str_next (nu_str_t s, nu_size_t *it, nu_wchar_t *c)
{
    if (*it >= s.size)
    {
        return NU_FALSE;
    }
    *c = s.data[*it];
    ++(*it);
    return NU_TRUE;
}
nu_bool_t
nu_str_to_u32 (nu_str_t s, nu_u32_t *v)
{
#ifdef NU_STDLIB
    nu_byte_t  buf[32];
    nu_byte_t *nptr = NU_NULL;
    nu_str_to_cstr(s, buf, 32);
    *v = strtoul((char *)buf, (char **)&nptr, 10);
    return !*nptr;
#endif
}
nu_bool_t
nu_str_to_i32 (nu_str_t s, nu_i32_t *v)
{
#ifdef NU_STDLIB
    nu_byte_t  buf[32];
    nu_byte_t *nptr = NU_NULL;
    nu_str_to_cstr(s, buf, 32);
    *v = strtol((char *)buf, (char **)&nptr, 10);
    return !*nptr;
#endif
}
nu_bool_t
nu_str_to_f32 (nu_str_t s, nu_f32_t *v)
{
#ifdef NU_STDLIB
    nu_byte_t  buf[32];
    nu_byte_t *nptr = NU_NULL;
    nu_str_to_cstr(s, buf, 32);
    *v = strtof((char *)buf, (char **)&nptr);
    return !*nptr;
#endif
}
nu_str_t
nu_str_fmt (nu_str_t buf, nu_str_t format, ...)
{
#ifdef NU_STDLIB
    va_list args;
    va_start(args, format);
    nu_str_t r = nu_str_vfmt(buf, format, args);
    va_end(args);
    return r;
#endif
}
nu_str_t
nu_str_vfmt (nu_str_t buf, nu_str_t format, va_list args)
{
#ifdef NU_STDLIB
    int r = vsnprintf((char *)buf.data, buf.size, (char *)format.data, args);
    return nu_str(buf.data, r);
#endif
}

#endif
