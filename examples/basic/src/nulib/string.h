#ifndef NU_STRING_H
#define NU_STRING_H

#include "types.h"
#include "platform.h"

#include <stdarg.h>

#define NU_PATH_MAX 256

#define NU_SV(str)                                     \
    (nu_sv_t)                                          \
    {                                                  \
        .data = (const nu_char_t *)(str),              \
        .size = (sizeof((str)) / sizeof((str)[0])) - 1 \
    }
#define NU_SV_FMT       "%.*s"
#define NU_SV_ARGS(str) (int)str.size, str.data

#define NU_ENUM_MAP(mapname, ...) \
    nu_enum_name_map_t mapname[]  \
        = { __VA_ARGS__, { .v = NU_NULL, .s = NU_NULL } };
#define NU_ENUM_NAME(enumval, name) { .v = enumval, .s = name }

#define NU_SV_TO_ENUM(sv, penum, map, found)                 \
    {                                                        \
        (found) = NU_FALSE;                                  \
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE((map)); ++i) \
        {                                                    \
            if (nu_sv_eq(nu_sv_cstr((map)[i].s), sv))        \
            {                                                \
                *(penum) = (map)[i].e;                       \
                found    = NU_TRUE;                          \
            }                                                \
        }                                                    \
    }

typedef struct
{
    const nu_char_t *data; // might not be null-terminated
    nu_size_t        size;
} nu_sv_t;

typedef struct
{
    nu_u32_t         v;
    const nu_char_t *s;
} nu_enum_name_map_t;

NU_API nu_size_t nu_strlen(const nu_char_t *s);

NU_API nu_sv_t   nu_sv(const nu_char_t *s, nu_size_t n);
NU_API nu_sv_t   nu_sv_cstr(const nu_char_t *s);
NU_API nu_sv_t   nu_sv_null(void);
NU_API nu_bool_t nu_sv_is_null(nu_sv_t str);
NU_API void      nu_sv_to_cstr(nu_sv_t str, nu_char_t *s, nu_size_t n);
NU_API nu_bool_t nu_sv_eq(nu_sv_t s1, nu_sv_t s2);
NU_API nu_u32_t  nu_sv_hash(nu_sv_t s);
NU_API nu_bool_t nu_sv_next(nu_sv_t s, nu_size_t *it, nu_wchar_t *c);
NU_API nu_bool_t nu_sv_to_u32(nu_sv_t s, nu_u32_t *v);
NU_API nu_bool_t nu_sv_to_i32(nu_sv_t s, nu_i32_t *v);
NU_API nu_bool_t nu_sv_to_f32(nu_sv_t s, nu_f32_t *v);
NU_API nu_sv_t   nu_sv_fmt(nu_char_t       *buf,
                           nu_size_t        n,
                           const nu_char_t *format,
                           ...);
NU_API nu_sv_t   nu_sv_vfmt(nu_char_t       *buf,
                            nu_size_t        n,
                            const nu_char_t *format,
                            va_list          args);
NU_API nu_sv_t   nu_sv_join(nu_char_t *buf, nu_size_t n, nu_sv_t a, nu_sv_t b);

NU_API nu_sv_t nu_path_basename(nu_sv_t path);
NU_API nu_sv_t nu_path_dirname(nu_sv_t path);
NU_API nu_sv_t nu_path_concat(nu_char_t *buf,
                              nu_size_t  n,
                              nu_sv_t    p1,
                              nu_sv_t    p2);

NU_API const nu_char_t *nu_enum_to_cstr(nu_u32_t                  v,
                                        const nu_enum_name_map_t *map);
NU_API nu_u32_t         nu_sv_to_enum(nu_sv_t                   sv,
                                      const nu_enum_name_map_t *map,
                                      nu_bool_t                *found);

#endif
