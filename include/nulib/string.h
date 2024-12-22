#ifndef NU_STRING_H
#define NU_STRING_H

#include "types.h"
#include "platform.h"

#include <stdarg.h>

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#define NU_MATCH(a, b) (nu_str_eq(a, b))
#define NU_STR(str)                                          \
    (nu_str_t)                                               \
    {                                                        \
        (nu_byte_t *)str, (sizeof(str) / sizeof(str[0])) - 1 \
    }

typedef struct
{
    nu_byte_t *data;
    nu_size_t  size;
} nu_str_t;

NU_API nu_size_t nu_cstr_len(const nu_byte_t *str, nu_size_t maxlen);
NU_API nu_str_t  nu_str(nu_byte_t *bytes, nu_size_t n);
NU_API nu_str_t  nu_str_from_cstr(nu_byte_t *s);
NU_API void      nu_str_to_cstr(nu_str_t str, nu_byte_t *s, nu_size_t n);
NU_API nu_bool_t nu_str_eq(nu_str_t s1, nu_str_t s2);
NU_API nu_u32_t  nu_str_hash(nu_str_t s);
NU_API nu_bool_t nu_str_next(nu_str_t s, nu_size_t *it, nu_wchar_t *c);
NU_API nu_bool_t nu_str_to_u32(nu_str_t s, nu_u32_t *v);
NU_API nu_bool_t nu_str_to_i32(nu_str_t s, nu_i32_t *v);
NU_API nu_bool_t nu_str_to_f32(nu_str_t s, nu_f32_t *v);
NU_API nu_str_t  nu_str_fmt(nu_str_t buf, nu_str_t format, ...);
NU_API nu_str_t  nu_str_vfmt(nu_str_t buf, nu_str_t format, va_list args);

NU_API nu_str_t nu_path_basename(nu_str_t path);
NU_API nu_str_t nu_path_dirname(nu_str_t path);
NU_API nu_str_t nu_path_concat(nu_str_t buf, nu_str_t p1, nu_str_t p2);

#endif
