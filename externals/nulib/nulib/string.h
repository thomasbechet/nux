#ifndef NU_STRING_H
#define NU_STRING_H

#include "types.h"
#include "platform.h"

#define NU_STRINGIFY(X)  NU_STRINGIFY_(X)
#define NU_STRINGIFY_(X) #X

#define NU_SV(str)                               \
    (nu_sv_t) { .ptr = (const nu_char_t *)(str), \
                .len = (sizeof((str)) / sizeof((str)[0])) - 1 }
#define NU_SV_FMT       "%.*s"
#define NU_SV_ARGS(str) (int)str.len, str.ptr

#define NU_ENUM_MAP(mapname, ...) \
    nu_enum_name_map_t mapname[]  \
        = { __VA_ARGS__, { .v = NU_NULL, .s = NU_NULL } };
#define NU_ENUM_NAME(enumval, name) \
    { .v = enumval, .s = name, .l = (sizeof((name)) / sizeof((name)[0])) - 1 }

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
    const nu_char_t *ptr; // might not be null-terminated
    nu_size_t        len;
} nu_sv_t;

typedef struct
{
    nu_u32_t         v;
    const nu_char_t *s;
    nu_size_t        l;
} nu_enum_name_map_t;

NU_API nu_size_t nu_strnlen(const nu_char_t *s, nu_size_t n);
NU_API void      nu_strncpy(nu_char_t *dst, const nu_char_t *src, nu_size_t n);
NU_API nu_int_t nu_strncmp(const nu_char_t *a, const nu_char_t *b, nu_size_t n);
NU_API nu_bool_t nu_strneq(const nu_char_t *a, const nu_char_t *b, nu_size_t n);

NU_API nu_sv_t nu_snprintf(nu_char_t       *buf,
                           nu_size_t        n,
                           const nu_char_t *format,
                           ...);
NU_API nu_sv_t nu_vsnprintf(nu_char_t       *buf,
                            nu_size_t        n,
                            const nu_char_t *format,
                            va_list          args);

NU_API nu_sv_t   nu_sv(const nu_char_t *s, nu_size_t n);
NU_API nu_sv_t   nu_sv_slice(const nu_char_t *s, nu_size_t n);
NU_API nu_sv_t   nu_sv_empty(void);
NU_API nu_sv_t   nu_sv_to_cstr(nu_sv_t str, nu_char_t *s, nu_size_t n);
NU_API nu_bool_t nu_sv_eq(nu_sv_t s1, nu_sv_t s2);
NU_API nu_u32_t  nu_sv_hash(nu_sv_t s);
NU_API nu_bool_t nu_sv_next(nu_sv_t s, nu_size_t *it, nu_wchar_t *c);
NU_API nu_bool_t nu_sv_to_u32(nu_sv_t s, nu_u32_t *v);
NU_API nu_bool_t nu_sv_to_i32(nu_sv_t s, nu_i32_t *v);
NU_API nu_bool_t nu_sv_to_f32(nu_sv_t s, nu_f32_t *v);
NU_API nu_sv_t   nu_sv_join(nu_char_t *buf, nu_size_t n, nu_sv_t a, nu_sv_t b);

NU_API const nu_char_t *nu_enum_to_cstr(nu_u32_t                  v,
                                        const nu_enum_name_map_t *map);
NU_API nu_u32_t         nu_sv_to_enum(nu_sv_t                   sv,
                                      const nu_enum_name_map_t *map,
                                      nu_bool_t                *found);

#endif
