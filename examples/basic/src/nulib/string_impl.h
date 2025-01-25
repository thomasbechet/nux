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
nu_strlen (const nu_char_t *s)
{
    const char *p = s;
    while (*p)
    {
        p++;
    }
    return p - s;
}

nu_sv_t
nu_sv (const nu_char_t *s, nu_size_t n)
{
    nu_sv_t str;
    str.data = s;
    str.size = n;
    return str;
}
nu_sv_t
nu_sv_cstr (const nu_char_t *s)
{
    return nu_sv(s, nu_strlen(s));
}
nu_sv_t
nu_sv_null (void)
{
    return nu_sv(NU_NULL, 0);
}
nu_bool_t
nu_sv_is_null (nu_sv_t str)
{
    return str.data == NU_NULL;
}
void
nu_sv_to_cstr (nu_sv_t str, nu_char_t *chars, nu_size_t n)
{
    NU_ASSERT(str.size < n);
    nu_memset(chars, 0, n);
    nu_memcpy(chars, str.data, NU_MIN(str.size, n - 1));
}
nu_bool_t
nu_sv_eq (nu_sv_t s1, nu_sv_t s2)
{
    return (s1.size == s2.size && nu_memcmp(s1.data, s2.data, s1.size) == 0);
}
nu_u32_t
nu_sv_hash (nu_sv_t s)
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
nu_sv_next (nu_sv_t s, nu_size_t *it, nu_wchar_t *c)
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
    return nu_sv(buf, k);
#endif
}
nu_sv_t
nu_sv_join (nu_char_t *buf, nu_size_t n, nu_sv_t a, nu_sv_t b)
{
    return nu_sv_fmt(buf, n, NU_SV_FMT NU_SV_FMT, NU_SV_ARGS(a), NU_SV_ARGS(b));
}

nu_sv_t
nu_path_basename (nu_sv_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_sv(path.data + n, path.size - n);
        }
    }
    return path;
}
nu_sv_t
nu_path_dirname (nu_sv_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_sv(path.data, n);
        }
    }
    return path;
}
nu_sv_t
nu_path_concat (nu_char_t *buf, nu_size_t n, nu_sv_t p1, nu_sv_t p2)
{
    return nu_sv_fmt(
        buf, n, NU_SV_FMT "/" NU_SV_FMT, NU_SV_ARGS(p1), NU_SV_ARGS(p2));
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
    while (map->s)
    {
        if (nu_sv_eq(nu_sv_cstr(current->s), sv))
        {
            *found = NU_TRUE;
            return current->v;
        }
        ++current;
    }
    return 0;
}

#endif
