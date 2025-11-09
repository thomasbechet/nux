#ifndef NUX_STDLIB_H
#define NUX_STDLIB_H

#include <config.h>
#include <stdint.h>

#ifdef NUX_BUILD_STDLIB
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef NUX_BUILD_DEBUG
#include <assert.h>
#endif
#endif

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_TRUE  1
#define NUX_FALSE 0
#define NUX_NULL  0
#define NUX_NOOP

#define NUX_UNUSED0()
#define NUX_UNUSED1(a)             (void)(a)
#define NUX_UNUSED2(a, b)          (void)(a), NUX_UNUSED1(b)
#define NUX_UNUSED3(a, b, c)       (void)(a), NUX_UNUSED2(b, c)
#define NUX_UNUSED4(a, b, c, d)    (void)(a), NUX_UNUSED3(b, c, d)
#define NUX_UNUSED5(a, b, c, d, e) (void)(a), NUX_UNUSED4(b, c, d, e)

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef __FILE_NAME__
#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__
#else
#define __SOURCE__ ""
#endif

#define NUX_CHECK(check, action) \
    if (!(check))                \
    {                            \
        action;                  \
    }

#define NUX_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define NUX_ARRAY_FILL(arr, size, value) \
    for (nu_size_t i = 0; i < size; ++i) \
    {                                    \
        (arr)[i] = (value);              \
    }

#define NUX_MATCH(v, k) (nux_strncmp((v), k, NUX_ARRAY_SIZE(k)) == 0)

#define NUX_BIG_ENDIAN (!*(unsigned char *)&(uint16_t) { 1 })

#if defined(NUX_BUILD_DEBUG) && defined(NUX_BUILD_STDLIB)
#define NUX_ASSERT(x) assert((x))
#else
#define NUX_ASSERT(x) (void)((x))
#endif

#define NUX_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define NUX_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define NUX_CLAMP(x, min, max) (NUX_MAX(min, NUX_MIN(max, x)))
#define NUX_CLAMP01(x)         (NUX_CLAMP(x, 0, 1))
#define NUX_ABS(x)             (((x) < 0) ? -(x) : (x))
#define NUX_SWAP(x, y, T) \
    do                    \
    {                     \
        T SWAP = x;       \
        x      = y;       \
        y      = SWAP;    \
    } while (0)

#define NUX_PI          3.14159265359
#define NUX_FLT_MAX     3.402823E+38
#define NUX_FLT_MIN     1.175494e-38
#define NUX_I32_MAX     2147483647
#define NUX_I32_MIN     -2147483648
#define NUX_U32_MAX     4294967295
#define NUX_U32_MIN     0
#define NUX_F32_EPSILON 0.00001

#define NUX_MEM_1K   (1 << 10)
#define NUX_MEM_2K   (1 << 11)
#define NUX_MEM_4K   (1 << 12)
#define NUX_MEM_8K   (1 << 13)
#define NUX_MEM_16K  (1 << 14)
#define NUX_MEM_32K  (1 << 15)
#define NUX_MEM_64K  (1 << 16)
#define NUX_MEM_1M   (1 << 20)
#define NUX_MEM_2M   (1 << 21)
#define NUX_MEM_4M   (1 << 22)
#define NUX_MEM_8M   (1 << 23)
#define NUX_MEM_16M  (1 << 24)
#define NUX_MEM_32M  (1 << 25)
#define NUX_MEM_64M  (1 << 26)
#define NUX_MEM_128M (1 << 27)
#define NUX_MEM_256M (1 << 28)
#define NUX_MEM_512M (1 << 29)
#define NUX_MEM_1G   (1 << 30)
#define NUX_MEM_2G   (1 << 31)
#define NUX_MEM_4G   (1 << 32)

#define NUX_V2_SIZE 2
#define NUX_V3_SIZE 3
#define NUX_V4_SIZE 4
#define NUX_Q4_SIZE 4
#define NUX_M3_SIZE 9
#define NUX_M4_SIZE 16

#define NUX_V2_ONES  nux_v2s(1)
#define NUX_V2_ZEROS nux_v2s(0)

#define NUX_V3_ONES     nux_v3s(1)
#define NUX_V3_ZEROS    nux_v3s(0)
#define NUX_V3_UP       nux_v3(0, 1, 0)
#define NUX_V3_DOWN     nux_v3(0, -1, 0)
#define NUX_V3_FORWARD  nux_v3(0, 0, -1)
#define NUX_V3_BACKWARD nux_v3(0, 0, 1)
#define NUX_V3_LEFT     nux_v3(-1, 0, 0)
#define NUX_V3_RIGHT    nux_v3(1, 0, 0)

#define NUX_V4_ONES  nux_v4s(1)
#define NUX_V4_ZEROS nux_v4s(0)

#define NUX_COLOR_RGBA(r, g, b, a) \
    (nux_v4_t) { r / 255., g / 255., b / 255., a / 255. }
#define NUX_COLOR_HEX(hex)                   \
    NUX_COLOR_RGBA(((hex & 0xFF0000) >> 16), \
                   ((hex & 0xFF00) >> 8),    \
                   ((hex & 0xFF) >> 0),      \
                   ((hex & 0xFF000000) >> 24))

#define NUX_V2_DEFINE(name, type)                                         \
    nux_##name##_t nux_##name(type x, type y);                            \
    nux_##name##_t nux_##name##s(type x);                                 \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b);  \
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);  \
    nux_f32_t      nux_##name##_norm(nux_##name##_t a);                   \
    nux_f32_t      nux_##name##_dist(nux_##name##_t a, nux_##name##_t b); \
    type           nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b);

#define NUX_V3_DEFINE(name, type)                                          \
    nux_##name##_t nux_##name(type x, type y, type z);                     \
    nux_##name##_t nux_##name##s(type x);                                  \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_inv(nux_##name##_t a);                     \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_cross(nux_##name##_t a, nux_##name##_t b); \
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b);   \
    nux_f32_t      nux_##name##_norm(nux_##name##_t a);                    \
    nux_f32_t      nux_##name##_dist(nux_##name##_t a, nux_##name##_t b);  \
    type           nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b);

#define NUX_V4_DEFINE(name, type)                                         \
    nux_##name##_t nux_##name(type x, type y, type z, type w);            \
    nux_##name##_t nux_##name##s(type x);                                 \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b);  \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);           \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);           \
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);  \
    nux_f32_t      nux_##name##_norm(nux_##name##_t a);                   \
    nux_f32_t      nux_##name##_dist(nux_##name##_t a, nux_##name##_t b); \
    type           nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b);

#define NUX_B3_DEFINE(name, type) nux_##name##_t nux_##name(type min, type max);

#define NUX_VEC_DEFINE(name, T)                                      \
    typedef struct                                                   \
    {                                                                \
        nux_arena_t *arena;                                          \
        nux_u32_t    size;                                           \
        nux_u32_t    capa;                                           \
        T           *data;                                           \
    } name##_t;                                                      \
    nux_status_t name##_init(nux_arena_t *a, name##_t *v);           \
    nux_status_t name##_init_capa(                                   \
        nux_arena_t *a, nux_u32_t capa, name##_t *v);                \
    T           *name##_push(name##_t *v);                           \
    nux_status_t name##_pushv(name##_t *v, T val);                   \
    nux_status_t name##_reserve(name##_t *v, nux_u32_t capa);        \
    nux_status_t name##_resize(name##_t *v, nux_u32_t size);         \
    T           *name##_pop(name##_t *v);                            \
    void         name##_clear(name##_t *v);                          \
    T           *name##_get(name##_t *v, nux_u32_t i);               \
    T           *name##_last(name##_t *v);                           \
    void         name##_swap(name##_t *v, nux_u32_t a, nux_u32_t b); \
    T           *name##_swap_pop(name##_t *v, nux_u32_t i);
#define NUX_VEC_IMPL(name, T)                                                     \
    nux_status_t name##_init(nux_arena_t *a, name##_t *v)                         \
    {                                                                             \
        return name##_init_capa(a, 0, v);                                         \
    }                                                                             \
    nux_status_t name##_init_capa(nux_arena_t *a, nux_u32_t capa, name##_t *v)    \
    {                                                                             \
        (v)->arena = a;                                                           \
        (v)->capa  = capa;                                                        \
        (v)->size  = 0;                                                           \
        (v)->data  = nux_arena_malloc(a, sizeof(*(v)->data) * capa);              \
        if (capa && !v->data)                                                     \
            return NUX_FAILURE;                                                   \
        return NUX_SUCCESS;                                                       \
    }                                                                             \
    T *name##_push(name##_t *v)                                                   \
    {                                                                             \
        if ((v)->size >= (v)->capa)                                               \
        {                                                                         \
            NUX_CHECK(name##_reserve(v, v->capa ? v->capa * 2 : 1),               \
                      return NUX_NULL);                                           \
        }                                                                         \
        T *ret = (v)->data + (v)->size;                                           \
        ++(v)->size;                                                              \
        return ret;                                                               \
    }                                                                             \
    nux_status_t name##_pushv(name##_t *v, T val)                                 \
    {                                                                             \
        T *a = name##_push(v);                                                    \
        if (a)                                                                    \
        {                                                                         \
            *a = val;                                                             \
            return NUX_SUCCESS;                                                   \
        }                                                                         \
        return NUX_FAILURE;                                                       \
    }                                                                             \
    nux_status_t name##_reserve(name##_t *v, nux_u32_t capa)                      \
    {                                                                             \
        if (capa > (v)->capa)                                                     \
        {                                                                         \
            nux_u32_t old_capa = (v)->capa;                                       \
            T        *old_data = (v)->data;                                       \
            (v)->capa          = capa;                                            \
            (v)->data          = nux_arena_realloc((v)->arena,                    \
                                          old_data,                      \
                                          sizeof(*(v)->data) * old_capa, \
                                          sizeof(*(v)->data) * capa);    \
            NUX_CHECK((v)->data, return NUX_NULL);                                \
        }                                                                         \
        return NUX_SUCCESS;                                                       \
    }                                                                             \
    nux_status_t name##_resize(name##_t *v, nux_u32_t size)                       \
    {                                                                             \
        NUX_CHECK(name##_reserve(v, size), return NUX_FAILURE);                   \
        v->size = size;                                                           \
        return NUX_SUCCESS;                                                       \
    }                                                                             \
    T *name##_pop(name##_t *v)                                                    \
    {                                                                             \
        if (!(v)->size)                                                           \
            return NUX_NULL;                                                      \
        T *ret = &(v)->data[(v)->size - 1];                                       \
        --(v)->size;                                                              \
        return ret;                                                               \
    }                                                                             \
    void name##_clear(name##_t *v)                                                \
    {                                                                             \
        (v)->size = 0;                                                            \
    }                                                                             \
    T *name##_get(name##_t *v, nux_u32_t i)                                       \
    {                                                                             \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                          \
    }                                                                             \
    T *name##_last(name##_t *v)                                                   \
    {                                                                             \
        return (v)->size ? (v)->data + (v)->size - 1 : NUX_NULL;                  \
    }                                                                             \
    void name##_swap(name##_t *v, nux_u32_t a, nux_u32_t b)                       \
    {                                                                             \
        NUX_CHECK(a < v->size && b < v->size && a != b, return);                  \
        T temp     = v->data[a];                                                  \
        v->data[a] = v->data[b];                                                  \
        v->data[b] = temp;                                                        \
    }                                                                             \
    T *name##_swap_pop(name##_t *v, nux_u32_t i)                                  \
    {                                                                             \
        NUX_CHECK(i < v->size, return NUX_NULL);                                  \
        name##_swap(v, i, v->size - 1);                                           \
        return name##_pop(v);                                                     \
    }

#define NUX_POOL_DEFINE(name, T)                           \
    typedef struct                                         \
    {                                                      \
        nux_u32_vec_t freelist;                            \
        nux_u32_t     capa;                                \
        nux_u32_t     size;                                \
        T            *data;                                \
    } name##_t;                                            \
    nux_status_t name##_init(nux_arena_t *a, name##_t *p); \
    nux_status_t name##_init_capa(                         \
        nux_arena_t *a, nux_u32_t capa, name##_t *p);      \
    T   *name##_add(name##_t *p);                          \
    void name##_remove(name##_t *p, T *i);
#define NUX_POOL_IMPL(name, T)                                                 \
    nux_status_t name##_init(nux_arena_t *a, name##_t *p)                      \
    {                                                                          \
        return name##_init_capa(a, 0, p);                                      \
    }                                                                          \
    nux_status_t name##_init_capa(nux_arena_t *a, nux_u32_t capa, name##_t *p) \
    {                                                                          \
        p->capa = capa;                                                        \
        p->size = 0;                                                           \
        NUX_CHECK(nux_u32_vec_init_capa(a, capa, &p->freelist),                \
                  return NUX_FAILURE);                                         \
        p->data = nux_arena_malloc(a, sizeof(*p->data) * capa);                \
        if (capa && !p->data)                                                  \
            return NUX_FAILURE;                                                \
        return NUX_SUCCESS;                                                    \
    }                                                                          \
    T *name##_add(name##_t *p)                                                 \
    {                                                                          \
        if (p->freelist.size)                                                  \
        {                                                                      \
            nux_u32_t free = *nux_u32_vec_pop(&p->freelist);                   \
            return &p->data[free];                                             \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            if (p->size >= p->capa)                                            \
            {                                                                  \
                nux_u32_t old_capa = (p)->capa;                                \
                T        *old_data = (p)->data;                                \
                nux_u32_t new_capa = old_capa * 2;                             \
                if (!new_capa)                                                 \
                {                                                              \
                    new_capa = 1;                                              \
                }                                                              \
                (p)->capa = new_capa;                                          \
                (p)->data = nux_arena_realloc((p)->freelist.arena,             \
                                              old_data,                        \
                                              sizeof(*(p)->data) * old_capa,   \
                                              sizeof(*(p)->data) * new_capa);  \
                NUX_CHECK((p)->data, return NUX_NULL);                         \
            }                                                                  \
            T *data = p->data + p->size;                                       \
            ++p->size;                                                         \
            return data;                                                       \
        }                                                                      \
    }                                                                          \
    void name##_remove(name##_t *p, T *i)                                      \
    {                                                                          \
        nux_u32_t index = i - p->data;                                         \
        NUX_ASSERT(index < p->size);                                           \
        nux_u32_vec_pushv(&p->freelist, index);                                \
    }

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef _Bool    nux_b32_t;
typedef uint8_t  nux_u8_t;
typedef char     nux_c8_t;
typedef int16_t  nux_i16_t;
typedef uint16_t nux_u16_t;
typedef int32_t  nux_i32_t;
typedef uint32_t nux_u32_t;
typedef int64_t  nux_i64_t;
typedef uint64_t nux_u64_t;
typedef float    nux_f32_t;
typedef double   nux_f64_t;
typedef intptr_t nux_intptr_t;

typedef union
{
    struct
    {
        nux_f32_t x;
        nux_f32_t y;
    };
    nux_f32_t data[2];
} nux_v2_t;

typedef union
{
    struct
    {
        nux_i32_t x;
        nux_i32_t y;
    };
    nux_i32_t data[2];
} nux_v2i_t;

typedef union
{
    struct
    {
        nux_f32_t x;
        nux_f32_t y;
        nux_f32_t z;
    };
    nux_f32_t data[3];
} nux_v3_t;

typedef union
{
    struct
    {
        nux_f32_t x;
        nux_f32_t y;
        nux_f32_t z;
        nux_f32_t w;
    };
    nux_f32_t data[4];
} nux_v4_t;

typedef union
{
    struct
    {
        nux_f32_t x;
        nux_f32_t y;
        nux_f32_t z;
        nux_f32_t w;
    };
    nux_f32_t data[4];
} nux_q4_t;

typedef union
{
    struct
    {
        nux_f32_t x1;
        nux_f32_t x2;
        nux_f32_t x3;
        nux_f32_t x4;
        nux_f32_t y1;
        nux_f32_t y2;
        nux_f32_t y3;
        nux_f32_t y4;
        nux_f32_t z1;
        nux_f32_t z2;
        nux_f32_t z3;
        nux_f32_t z4;
        nux_f32_t w1;
        nux_f32_t w2;
        nux_f32_t w3;
        nux_f32_t w4;
    };
    nux_f32_t data[16];
    nux_f32_t data2d[4][4];
} nux_m4_t;

typedef struct
{
    nux_i32_t x;
    nux_i32_t y;
    nux_u32_t w;
    nux_u32_t h;
} nux_b2i_t;

typedef union
{
    struct
    {
        nux_u32_t x;
        nux_u32_t y;
    };
    nux_u32_t data[2];
} nux_v2u_t;

typedef union
{
    struct
    {
        nux_i32_t x;
        nux_i32_t y;
        nux_i32_t z;
    };
    nux_i32_t data[3];
} nux_v3i_t;

typedef union
{
    struct
    {
        nux_u32_t x;
        nux_u32_t y;
        nux_u32_t z;
    };
    nux_u32_t data[3];
} nux_v3u_t;

typedef union
{
    struct
    {
        nux_i32_t x;
        nux_i32_t y;
        nux_i32_t z;
        nux_i32_t w;
    };
    nux_i32_t data[4];
} nux_v4i_t;

typedef union
{
    struct
    {
        nux_u32_t x;
        nux_u32_t y;
        nux_u32_t z;
        nux_u32_t w;
    };
    nux_u32_t data[4];
} nux_v4u_t;

typedef union
{
    struct
    {
        nux_f32_t x1;
        nux_f32_t x2;
        nux_f32_t x3;
        nux_f32_t y1;
        nux_f32_t y2;
        nux_f32_t y3;
        nux_f32_t z1;
        nux_f32_t z2;
        nux_f32_t z3;
    };
    nux_f32_t data[NUX_M3_SIZE];
} nux_m3_t;

typedef struct
{
    nux_v3_t min;
    nux_v3_t max;
} nux_b3_t;

typedef struct
{
    nux_v3i_t min;
    nux_v3i_t max;
} nux_b3i_t;

typedef struct
{
    nux_u64_t state;
    nux_u64_t incr;
} nux_pcg_t;

typedef enum
{
    NUX_SUCCESS = 1,
    NUX_FAILURE = 0
} nux_status_t;

typedef enum
{
    NUX_PATH_MAX      = 256, // does not include '\0'
    NUX_PATH_BUF_SIZE = NUX_PATH_MAX + 1
} nux_constants_t;

typedef struct
{
    void *userdata;
    void *(*alloc)(void *userdata, void *p, nux_u32_t osize, nux_u32_t nsize);
} nux_allocator_t;

typedef struct
{
    nux_u32_t block_count;
    nux_u32_t memory_usage;
    nux_u32_t memory_capacity;
    nux_u32_t memory_waste;
} nux_arena_info_t;

typedef void (*nux_arena_finalizer_t)(void *p);

typedef struct nux_arena_object
{
    struct nux_arena_object *prev;
    nux_arena_finalizer_t    finalize;
} nux_arena_object_t;

typedef struct
{
    void *userdata;
    void *(*alloc)(void *userdata, void *p, nux_u32_t osize, nux_u32_t nsize);
    void (*clear)(void *userdata);
    void (*free)(void *userdata);
    void (*info)(void *userdata, nux_arena_info_t *info);
    nux_arena_object_t *chain;
} nux_arena_t;

typedef struct block
{
    struct block *prev;
    struct block *next;
    nux_u32_t     bc;
} nux_block_arena_header_t;

typedef struct
{
    nux_arena_t               arena;
    nux_allocator_t          *allocator;
    nux_block_arena_header_t *first_header;
    nux_block_arena_header_t *last_header;
    nux_u32_t                 block_size;
    nux_u8_t                 *head;
    nux_u8_t                 *end;
    nux_u32_t                 total_alloc;
    nux_u32_t                 total_waste;
} nux_block_arena_t;

NUX_V2_DEFINE(v2i, nux_i32_t)
NUX_V2_DEFINE(v2u, nux_u32_t)
NUX_V2_DEFINE(v2, nux_f32_t)
NUX_V3_DEFINE(v3i, nux_i32_t)
NUX_V3_DEFINE(v3u, nux_u32_t)
NUX_V3_DEFINE(v3, nux_f32_t)
NUX_V4_DEFINE(v4i, nux_i32_t)
NUX_V4_DEFINE(v4u, nux_u32_t)
NUX_V4_DEFINE(v4, nux_f32_t)
NUX_B3_DEFINE(b3, nux_v3_t)
NUX_B3_DEFINE(b3i, nux_v3i_t)

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_f32_vec, nux_f32_t)
NUX_VEC_DEFINE(nux_v4_vec, nux_v4_t)
NUX_VEC_DEFINE(nux_ptr_vec, void *);

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

nux_u32_t nux_strnlen(const nux_c8_t *s, nux_u32_t n);
void      nux_strncpy(nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n);
nux_i32_t nux_strncmp(const nux_c8_t *a, const nux_c8_t *b, nux_u32_t n);
nux_f32_t nux_strtof(const nux_c8_t *s, nux_c8_t **end);
nux_c8_t *nux_strdup(nux_arena_t *a, const nux_c8_t *s);
nux_u32_t nux_snprintf(nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, ...);
nux_u32_t nux_vsnprintf(nux_c8_t       *buf,
                        nux_u32_t       n,
                        const nux_c8_t *format,
                        va_list         args);
nux_b32_t nux_path_isdir(const nux_c8_t *path);
nux_status_t nux_path_concat(nux_c8_t       *dst,
                             const nux_c8_t *a,
                             const nux_c8_t *b);
nux_u32_t    nux_path_basename(nux_c8_t *dst, const nux_c8_t *path);
nux_u32_t    nux_path_normalize(nux_c8_t *dst, const nux_c8_t *path);
nux_b32_t    nux_path_endswith(const nux_c8_t *path, const nux_c8_t *end);
nux_status_t nux_path_set_extension(nux_c8_t *path, const nux_c8_t *extension);
void         nux_path_copy(nux_c8_t *dst, const nux_c8_t *src);

nux_u32_t nux_log2(nux_u32_t n);
nux_f32_t nux_log10(nux_f32_t x);
nux_f32_t nux_fabs(nux_f32_t f);
nux_f32_t nux_floor(nux_f32_t f);
nux_f32_t nux_radian(nux_f32_t d);
nux_f32_t nux_sqrt(nux_f32_t x);
nux_f32_t nux_pow(nux_f32_t b, nux_f32_t e);
nux_f32_t nux_cos(nux_f32_t x);
nux_f32_t nux_sin(nux_f32_t x);
nux_f32_t nux_tan(nux_f32_t x);
nux_f32_t nux_exp(nux_f32_t x);
nux_f32_t nux_atan2(nux_f32_t x, nux_f32_t y);

nux_b2i_t nux_b2i_min_max(nux_v2i_t min, nux_v2i_t max);
nux_b2i_t nux_b2i(nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h);
nux_b2i_t nux_b2i_translate(nux_b2i_t b, nux_v2i_t t);
nux_b2i_t nux_b2i_moveto(nux_b2i_t b, nux_v2i_t p);
nux_v2u_t nux_b2i_size(nux_b2i_t b);
nux_v2u_t nux_b2i_size(nux_b2i_t b);
nux_b32_t nux_b2i_containsi(nux_b2i_t b, nux_v2i_t p);
nux_b32_t nux_b2i_contains(nux_b2i_t b, nux_v2_t p);
nux_b2i_t nux_b2i_merge(nux_b2i_t a, nux_b2i_t b);
nux_i32_t nux_b2i_top(nux_b2i_t b);
nux_i32_t nux_b2i_bottom(nux_b2i_t b);
nux_i32_t nux_b2i_left(nux_b2i_t b);
nux_i32_t nux_b2i_right(nux_b2i_t b);
nux_v2i_t nux_b2i_tl(nux_b2i_t b);
nux_v2i_t nux_b2i_br(nux_b2i_t b);

void nux_qsort(void     *base,
               nux_u32_t n,
               nux_u32_t size,
               nux_i32_t (*compare)(const void *a, const void *b));

nux_v4_t  nux_color_rgba(nux_u8_t r, nux_u8_t g, nux_u8_t b, nux_u8_t a);
nux_v4_t  nux_color_hex(nux_u32_t hex);
nux_u32_t nux_color_to_hex(nux_v4_t color);
nux_v4_t  nux_color_to_srgb(nux_v4_t color);

nux_i32_t nux_memcmp(const void *p0, const void *p1, nux_u32_t n);
void     *nux_memset(void *dst, nux_u32_t c, nux_u32_t n);
void      nux_memcpy(void *dst, const void *src, nux_u32_t n);
void      nux_memswp(void *a, void *b, nux_u32_t n);
void     *nux_memalign(void *ptr, nux_u32_t align);
nux_u32_t nux_u32_le(nux_u32_t v);
nux_f32_t nux_f32_le(nux_f32_t v);
nux_u32_t nux_hash(const void *p, nux_u32_t s);
nux_c8_t *nux_mem_human(double size, nux_c8_t buf[10]);

nux_pcg_t nux_pcg(nux_u64_t state, nux_u64_t incr);
nux_u32_t nux_pcg_u32(nux_pcg_t *pcg);
nux_f32_t nux_pcg_f32(nux_pcg_t *pcg);

nux_v3_t nux_v3_normalize(nux_v3_t a);

nux_m3_t nux_m3_zero(void);
nux_m3_t nux_m3_identity(void);
nux_m3_t nux_m3_axis(nux_v3_t x, nux_v3_t y, nux_v3_t z);
nux_m3_t nux_m3_translate(nux_v2_t v);
nux_m3_t nux_m3_scale(nux_v2_t v);
nux_m3_t nux_m3_mul(nux_m3_t a, nux_m3_t b);

nux_m4_t nux_m4(const nux_f32_t *p);
nux_m4_t nux_m4_zero(void);
nux_m4_t nux_m4_identity(void);
nux_m4_t nux_m4_inv(nux_m4_t v);
nux_m4_t nux_m4_translate(nux_v3_t v);
nux_m4_t nux_m4_scale(nux_v3_t v);
nux_m4_t nux_m4_rotate_y(nux_f32_t angle);
nux_m4_t nux_m4_mul(nux_m4_t a, nux_m4_t b);
nux_v4_t nux_m4_mulv(nux_m4_t a, nux_v4_t v);
nux_v3_t nux_m4_mulv3(nux_m4_t a, nux_v3_t v, nux_f32_t w);
nux_m4_t nux_m4_trs(nux_v3_t t, nux_q4_t r, nux_v3_t s);
void nux_m4_trs_decompose(nux_m4_t m, nux_v3_t *t, nux_q4_t *r, nux_v3_t *s);
nux_m4_t nux_m4_perspective(nux_f32_t fov,
                            nux_f32_t aspect_ratio,
                            nux_f32_t z_near,
                            nux_f32_t z_far);
nux_m4_t nux_m4_ortho(nux_f32_t left,
                      nux_f32_t right,
                      nux_f32_t bottom,
                      nux_f32_t top,
                      nux_f32_t near,
                      nux_f32_t far);
nux_m4_t nux_m4_lookat(nux_v3_t eye, nux_v3_t center, nux_v3_t up);

nux_q4_t  nux_q4(nux_f32_t x, nux_f32_t y, nux_f32_t z, nux_f32_t w);
nux_q4_t  nux_q4_from_m3(nux_m3_t rot);
nux_q4_t  nux_q4_euler(nux_v3_t euler);
nux_v3_t  nux_q4_to_euler(nux_q4_t q);
nux_q4_t  nux_q4_identity(void);
nux_v4_t  nux_q4_vec4(nux_q4_t a);
nux_f32_t nux_q4_norm(nux_q4_t a);
nux_q4_t  nux_q4_axis(nux_v3_t axis, nux_f32_t angle);
nux_q4_t  nux_q4_mul(nux_q4_t a, nux_q4_t b);
nux_v3_t  nux_q4_mulv3(nux_q4_t a, nux_v3_t v);
nux_q4_t  nux_q4_mul_axis(nux_q4_t q, nux_v3_t axis, nux_f32_t angle);
nux_m4_t  nux_q4_mat4(nux_q4_t q);
nux_m4_t  nux_q4_mulm4(nux_q4_t a, nux_m4_t m);

void *nux_malloc(nux_allocator_t *a, nux_u32_t size);
void *nux_realloc(nux_allocator_t *a,
                  void            *optr,
                  nux_u32_t        osize,
                  nux_u32_t        nsize);
void  nux_free(nux_allocator_t *a, void *p, nux_u32_t osize);

void             nux_arena_init(nux_arena_t *a,
                                void        *userdata,
                                void *(*alloc)(void *ud, void *p, nux_u32_t o, nux_u32_t n),
                                void (*clear)(void *ud),
                                void (*free)(void *ud),
                                void (*info)(void *ud, nux_arena_info_t *info));
void             nux_arena_free(nux_arena_t *a);
nux_allocator_t  nux_arena_as_allocator(nux_arena_t *a);
nux_arena_info_t nux_arena_info(nux_arena_t *a);
void            *nux_arena_malloc(nux_arena_t *a, nux_u32_t size);
void            *nux_arena_realloc(nux_arena_t *a,
                                   void        *p,
                                   nux_u32_t    osize,
                                   nux_u32_t    nsize);
void            *nux_arena_new_object(nux_arena_t          *a,
                                      nux_u32_t             size,
                                      nux_arena_finalizer_t finalizer);
void             nux_arena_clear(nux_arena_t *a);

void nux_block_arena_init(nux_arena_t       *a,
                          nux_block_arena_t *ba,
                          nux_allocator_t   *allocator);

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v);

#endif
