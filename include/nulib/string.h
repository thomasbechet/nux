#ifndef NU_STRING_H
#define NU_STRING_H

#include "types.h"
#include "platform.h"

#include <stdarg.h>

#define NU_STR(str)                                          \
    (nu_str_t)                                               \
    {                                                        \
        (nu_byte_t *)str, (sizeof(str) / sizeof(str[0])) - 1 \
    }

#define NU_STR_FMT       "%.*s"
#define NU_STR_ARGS(str) (int)str.size, str.data

typedef struct
{
    const nu_byte_t *data; // might not be null-terminated
    nu_u32_t         size;
} nu_str_t;

NU_API nu_str_t  nu_str_null(void);
NU_API nu_bool_t nu_str_is_null(nu_str_t str);
NU_API nu_str_t  nu_str_from_cstr(const nu_char_t *s);
NU_API nu_str_t  nu_str_from_bytes(const nu_byte_t *bytes, nu_size_t n);
NU_API void      nu_str_to_cstr(nu_str_t str, nu_char_t *s, nu_size_t n);
NU_API nu_bool_t nu_str_eq(nu_str_t s1, nu_str_t s2);
NU_API nu_u32_t  nu_str_hash(nu_str_t s);
NU_API nu_bool_t nu_str_next(nu_str_t s, nu_size_t *it, nu_wchar_t *c);
NU_API nu_bool_t nu_str_to_u32(nu_str_t s, nu_u32_t *v);
NU_API nu_bool_t nu_str_to_i32(nu_str_t s, nu_i32_t *v);
NU_API nu_bool_t nu_str_to_f32(nu_str_t s, nu_f32_t *v);
NU_API nu_str_t  nu_str_fmt(nu_char_t       *buf,
                            nu_size_t        n,
                            const nu_char_t *format,
                            ...);
NU_API nu_str_t  nu_str_vfmt(nu_char_t       *buf,
                             nu_size_t        n,
                             const nu_char_t *format,
                             va_list          args);

NU_API nu_str_t nu_path_basename(nu_str_t path);
NU_API nu_str_t nu_path_dirname(nu_str_t path);
NU_API nu_str_t nu_path_concat(nu_char_t *buf,
                               nu_size_t  n,
                               nu_str_t   p1,
                               nu_str_t   p2);

#endif
