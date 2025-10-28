#ifndef NUX_BASE_MODULE_H
#define NUX_BASE_MODULE_H

#include <nux.h>

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
#define NUX_ENSURE(check, action, format, ...)                 \
    if (!(check))                                              \
    {                                                          \
        nux_error(format " at %s", ##__VA_ARGS__, __SOURCE__); \
        action;                                                \
    }

#define NUX_DEBUG(format, ...)   nux_log(NUX_LOG_DEBUG, format, ##__VA_ARGS__)
#define NUX_INFO(format, ...)    nux_log(NUX_LOG_INFO, format, ##__VA_ARGS__)
#define NUX_WARNING(format, ...) nux_log(NUX_LOG_WARNING, format, ##__VA_ARGS__)
#define NUX_ERROR(format, ...)   nux_log(NUX_LOG_ERROR, format, ##__VA_ARGS__)

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
#define NUX_VEC_IMPL(name, T)                                                   \
    nux_status_t name##_init(nux_arena_t *a, name##_t *v)                       \
    {                                                                           \
        return name##_init_capa(a, 0, v);                                       \
    }                                                                           \
    nux_status_t name##_init_capa(nux_arena_t *a, nux_u32_t capa, name##_t *v)  \
    {                                                                           \
        (v)->arena = a;                                                         \
        (v)->capa  = capa;                                                      \
        (v)->size  = 0;                                                         \
        (v)->data                                                               \
            = nux_arena_alloc(a, NUX_NULL, 0, sizeof(*(v)->data) * capa);       \
        if (capa && !v->data)                                                   \
            return NUX_FAILURE;                                                 \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    T *name##_push(name##_t *v)                                                 \
    {                                                                           \
        if ((v)->size >= (v)->capa)                                             \
        {                                                                       \
            NUX_CHECK(name##_reserve(v, v->capa ? v->capa * 2 : 1),             \
                      return NUX_NULL);                                         \
        }                                                                       \
        T *ret = (v)->data + (v)->size;                                         \
        ++(v)->size;                                                            \
        return ret;                                                             \
    }                                                                           \
    nux_status_t name##_pushv(name##_t *v, T val)                               \
    {                                                                           \
        T *a = name##_push(v);                                                  \
        if (a)                                                                  \
        {                                                                       \
            *a = val;                                                           \
            return NUX_SUCCESS;                                                 \
        }                                                                       \
        return NUX_FAILURE;                                                     \
    }                                                                           \
    nux_status_t name##_reserve(name##_t *v, nux_u32_t capa)                    \
    {                                                                           \
        if (capa > (v)->capa)                                                   \
        {                                                                       \
            nux_u32_t old_capa = (v)->capa;                                     \
            T        *old_data = (v)->data;                                     \
            (v)->capa          = capa;                                          \
            (v)->data          = nux_arena_alloc((v)->arena,                    \
                                        old_data,                      \
                                        sizeof(*(v)->data) * old_capa, \
                                        sizeof(*(v)->data) * capa);    \
            NUX_CHECK((v)->data, return NUX_NULL);                              \
        }                                                                       \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    nux_status_t name##_resize(name##_t *v, nux_u32_t size)                     \
    {                                                                           \
        NUX_CHECK(name##_reserve(v, size), return NUX_FAILURE);                 \
        v->size = size;                                                         \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    T *name##_pop(name##_t *v)                                                  \
    {                                                                           \
        if (!(v)->size)                                                         \
            return NUX_NULL;                                                    \
        T *ret = &(v)->data[(v)->size - 1];                                     \
        --(v)->size;                                                            \
        return ret;                                                             \
    }                                                                           \
    void name##_clear(name##_t *v)                                              \
    {                                                                           \
        (v)->size = 0;                                                          \
    }                                                                           \
    T *name##_get(name##_t *v, nux_u32_t i)                                     \
    {                                                                           \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                        \
    }                                                                           \
    T *name##_last(name##_t *v)                                                 \
    {                                                                           \
        return (v)->size ? (v)->data + (v)->size - 1 : NUX_NULL;                \
    }                                                                           \
    void name##_swap(name##_t *v, nux_u32_t a, nux_u32_t b)                     \
    {                                                                           \
        NUX_CHECK(a < v->size && b < v->size && a != b, return);                \
        T temp     = v->data[a];                                                \
        v->data[a] = v->data[b];                                                \
        v->data[b] = temp;                                                      \
    }                                                                           \
    T *name##_swap_pop(name##_t *v, nux_u32_t i)                                \
    {                                                                           \
        NUX_CHECK(i < v->size, return NUX_NULL);                                \
        name##_swap(v, i, v->size - 1);                                         \
        return name##_pop(v);                                                   \
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
        p->data = nux_arena_alloc(a, NUX_NULL, 0, sizeof(*p->data) * capa);    \
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
                (p)->data = nux_arena_alloc((p)->freelist.arena,               \
                                            old_data,                          \
                                            sizeof(*(p)->data) * old_capa,     \
                                            sizeof(*(p)->data) * new_capa);    \
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

typedef enum
{
    NUX_BASE_DEFAULT_EVENT_SIZE            = 32,
    NUX_BASE_DEFAULT_CONTROLLER_INPUT_SIZE = 16
} nux_base_defaults_t;

typedef enum
{
    NUX_RESOURCE_NULL       = 0,
    NUX_RESOURCE_ARENA      = 1,
    NUX_RESOURCE_LUA_MODULE = 2,
    NUX_RESOURCE_TEXTURE    = 3,
    NUX_RESOURCE_MESH       = 4,
    NUX_RESOURCE_PALETTE    = 5,
    NUX_RESOURCE_VIEWPORT   = 6,
    NUX_RESOURCE_CANVAS     = 7,
    NUX_RESOURCE_FONT       = 8,
    NUX_RESOURCE_FILE       = 9,
    NUX_RESOURCE_DISK       = 10,
    NUX_RESOURCE_SCENE      = 11,
    NUX_RESOURCE_QUERY      = 12,
    NUX_RESOURCE_EVENT      = 13,
    NUX_RESOURCE_INPUTMAP   = 14,
    NUX_RESOURCE_GUI        = 15,
    NUX_RESOURCE_STYLESHEET = 16,

    NUX_RESOURCE_MAX = 256,
} nux_resource_base_t;

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

typedef struct nux_resource_finalizer
{
    nux_rid_t                      rid;
    struct nux_resource_finalizer *prev;
    struct nux_resource_finalizer *next;
} nux_resource_header_t;

typedef struct nux_arena_block
{
    struct nux_arena_block *prev;
    struct nux_arena_block *next;
} nux_arena_block_t;

struct nux_arena_t
{
    nux_resource_header_t *first_header;
    nux_resource_header_t *last_header;
    nux_arena_block_t     *first_block;
    nux_arena_block_t     *last_block;
    nux_u32_t              block_size;
    nux_u8_t              *head;
    nux_u8_t              *end;
    void                  *stack;
    nux_u32_t              total_alloc;
    nux_u32_t              total_waste;
};

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_f32_vec, nux_f32_t)
NUX_VEC_DEFINE(nux_v4_vec, nux_v4_t)
NUX_VEC_DEFINE(nux_ptr_vec, void *);
typedef nux_u32_vec_t nux_rid_vec_t;

typedef void (*nux_resource_cleanup_t)(void *data);
typedef nux_status_t (*nux_resource_reload_t)(void *data, const nux_c8_t *path);

typedef struct
{
    nux_rid_t       self; // for validity check
    nux_arena_t    *arena;
    nux_u32_t       type;
    void           *data;
    const nux_c8_t *path;
    const nux_c8_t *name;
    nux_u32_t       prev_entry_index;
    nux_u32_t       next_entry_index;
} nux_resource_entry_t;

typedef struct
{
    const nux_c8_t        *name;
    nux_u32_t              size;
    nux_resource_cleanup_t cleanup;
    nux_resource_reload_t  reload;
    nux_u32_t              first_entry_index;
    nux_u32_t              last_entry_index;
} nux_resource_type_t;

NUX_POOL_DEFINE(nux_resource_pool, nux_resource_entry_t);

typedef struct
{
    struct
    {
        nux_b32_t enable;
        nux_u32_t width;
        nux_u32_t height;
    } window;

    struct
    {
        nux_b32_t enable;
    } scene;

    struct
    {
        nux_b32_t enable;
    } physics;

    struct
    {
        nux_log_level_t level;
    } log;

    struct
    {
        nux_u32_t transforms_buffer_size;
        nux_u32_t batches_buffer_size;
        nux_u32_t vertices_buffer_size;
    } graphics;

    struct
    {
        nux_b32_t enable;
        nux_b32_t console;
    } debug;

    nux_b32_t hotreload;

} nux_config_t;

typedef enum
{
    NUX_EVENT_LUA = 0,
} nux_event_type_t;

typedef void (*nux_event_callback_t)(void       *userdata,
                                     nux_rid_t   event,
                                     const void *data);
typedef void (*nux_event_cleanup_t)(void *data);

typedef struct nux_event_handler
{
    nux_rid_t                 event;
    struct nux_event_handler *next;
    struct nux_event_handler *prev;
    nux_event_callback_t      callback;
    void                     *userdata;
} nux_event_handler_t;

typedef struct nux_event_header
{
    struct nux_event_header *next;
    void                    *data;
} nux_event_header_t;

typedef struct
{
    nux_event_type_t     type;
    nux_arena_t         *arena;
    nux_event_handler_t *first_handler;
    nux_event_header_t  *first_event;
    nux_event_cleanup_t  cleanup;
} nux_event_t;

typedef enum
{
    NUX_CART_HEADER_SIZE = 4 * 3,
    NUX_CART_ENTRY_SIZE  = 4 * 6,
} nux_cart_layout_t;

typedef enum
{
    NUX_DISK_OS,
    NUX_DISK_CART,
} nux_disk_type_t;

typedef struct
{
    nux_disk_type_t type;
    nux_io_mode_t   mode;
    nux_b32_t       is_open;
    union
    {
        struct
        {
            nux_u32_t slot;
            nux_u32_t offset;
            nux_u32_t length;
            nux_u32_t cursor;
        } cart;
        struct
        {
            nux_u32_t slot;
        } os;
    };
} nux_file_t;

typedef struct
{
    nux_b32_t compressed;
    nux_u32_t data_type;
    nux_u32_t data_offset;
    nux_u32_t data_length;
    nux_u32_t path_hash;
    nux_u32_t path_offset;
    nux_u32_t path_length;
} nux_cart_entry_t;

typedef struct
{
    const nux_c8_t   *path;
    nux_file_t       *file;
    nux_cart_entry_t *entries;
    nux_u32_t         entries_count;
} nux_cart_t;

typedef struct
{
    nux_b32_t started;
    nux_rid_t file;
    nux_u32_t entry_count;
    nux_u32_t entry_index;
    nux_u32_t cursor;
} nux_cart_writer_t;

typedef struct nux_disk
{
    nux_disk_type_t  type;
    struct nux_disk *prev;
    struct nux_disk *next;
    union
    {
        nux_cart_t cart;
    };
} nux_disk_t;

typedef struct
{
    const nux_c8_t  *name;
    nux_input_type_t type;
    union
    {
        nux_key_t            key;
        nux_mouse_button_t   mouse_button;
        nux_mouse_axis_t     mouse_axis;
        nux_gamepad_button_t gamepad_button;
        nux_gamepad_axis_t   gamepad_axis;
    };
    nux_f32_t sensivity;
} nux_inputmap_entry_t;

NUX_VEC_DEFINE(nux_inputmap_entry_vec, nux_inputmap_entry_t);

struct nux_inputmap_t
{
    nux_inputmap_entry_vec_t entries;
    nux_u32_t                cursor_motions[4];
};

typedef struct
{
    nux_v2_t cursor;
    nux_v2_t cursor_prev;

    nux_rid_t     inputmap;
    nux_f32_vec_t inputs;
    nux_f32_vec_t prev_inputs;
} nux_controller_t;

typedef enum
{
    NUX_SERDE_OBJECT,
    NUX_SERDE_ARRAY,
    NUX_SERDE_END,
    NUX_SERDE_U32,
    NUX_SERDE_F32,
    NUX_SERDE_V3,
    NUX_SERDE_Q4,
    NUX_SERDE_BYTES,
    NUX_SERDE_STRING,
    NUX_SERDE_RID,
    NUX_SERDE_NID,
} nux_serde_type_t;

typedef struct
{
    const nux_c8_t  *key;
    nux_serde_type_t type;
    union
    {
        nux_u32_t *u32;
        nux_f32_t *f32;
        nux_v3_t  *v3;
        nux_q4_t  *q4;
        struct
        {
            const nux_u8_t **data;
            nux_u32_t        n;
        } bytes;
        struct
        {
            const nux_c8_t **s;
            nux_u32_t       *n;
        } str;
        nux_u32_t *size;
    };
} nux_serde_value_t;

struct nux_serde_writer;
struct nux_serde_reader;

typedef nux_status_t (*nux_serde_writer_callback_t)(void *userdata,
                                                    const nux_serde_value_t *v);
typedef nux_status_t (*nux_serde_reader_callback_t)(void              *userdata,
                                                    nux_serde_value_t *v);

typedef struct nux_serde_writer
{
    void                       *userdata;
    nux_serde_writer_callback_t callback;
    nux_status_t                status;
} nux_serde_writer_t;

typedef struct nux_serde_reader
{
    void                       *userdata;
    nux_serde_reader_callback_t callback;
    nux_status_t                status;
} nux_serde_reader_t;

typedef struct
{
    nux_file_t        *file;
    nux_b32_t          has_previous_value;
    nux_serde_writer_t writer;
    nux_serde_type_t   stack[256];
    nux_u32_t          depth;
} nux_json_writer_t;

struct jsmntok;

typedef struct
{
    nux_serde_reader_t reader;
    const nux_c8_t    *json;
    nux_u32_t          json_size;
    void              *tokens;
    nux_u32_t          tokens_capa;
    nux_u32_t          tokens_count;
    struct jsmntok    *it;
    nux_u32_t          depth;
    struct jsmntok    *iters[256];
} nux_json_reader_t;

NUX_VEC_DEFINE(nux_os_event_vec, nux_os_event_t);

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

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

// ds.c

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v);

// math.c

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

nux_b2i_t nux_b2i(nux_v2i_t min, nux_v2i_t max);
nux_b2i_t nux_b2i_xywh(nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h);
nux_b2i_t nux_b2i_translate(nux_b2i_t b, nux_v2i_t t);
nux_b2i_t nux_b2i_moveto(nux_b2i_t b, nux_v2i_t p);
nux_v2u_t nux_b2i_size(nux_b2i_t b);
nux_v2u_t nux_b2i_size(nux_b2i_t b);
nux_b32_t nux_b2i_containsi(nux_b2i_t b, nux_v2i_t p);
nux_b32_t nux_b2i_contains(nux_b2i_t b, nux_v2_t p);
nux_b2i_t nux_b2i_merge(nux_b2i_t a, nux_b2i_t b);

void nux_qsort(void     *base,
               nux_u32_t n,
               nux_u32_t size,
               nux_i32_t (*compare)(const void *a, const void *b));

// string.c

nux_u32_t nux_strnlen(const nux_c8_t *s, nux_u32_t n);
void      nux_strncpy(nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n);
nux_i32_t nux_strncmp(const nux_c8_t *a, const nux_c8_t *b, nux_u32_t n);
nux_f32_t nux_strtof(const nux_c8_t *s, nux_c8_t **end);
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

// memory.c

nux_i32_t nux_memcmp(const void *p0, const void *p1, nux_u32_t n);
void     *nux_memset(void *dst, nux_u32_t c, nux_u32_t n);
void      nux_memcpy(void *dst, const void *src, nux_u32_t n);
void      nux_memswp(void *a, void *b, nux_u32_t n);
void     *nux_memalign(void *ptr, nux_u32_t align);
nux_u32_t nux_u32_le(nux_u32_t v);
nux_f32_t nux_f32_le(nux_f32_t v);
nux_u32_t nux_hash(const void *p, nux_u32_t s);
nux_c8_t *nux_mem_human(double size, nux_c8_t buf[10]);

// resource.c

nux_resource_type_t *nux_resource_register(nux_u32_t       index,
                                           nux_u32_t       size,
                                           const nux_c8_t *name);
void                *nux_resource_new(nux_arena_t *arena, nux_u32_t type);
void                 nux_resource_delete(nux_rid_t rid);
void                 nux_resource_set_path(nux_rid_t rid, const nux_c8_t *path);
void                *nux_resource_get(nux_u32_t type, nux_rid_t rid);
void                *nux_resource_check(nux_u32_t type, nux_rid_t rid);
nux_status_t         nux_resource_reload(nux_rid_t rid);
// nux_rid_t            nux_resource_next(nux_u32_t type, nux_rid_t rid);
void                *nux_resource_nextp(nux_u32_t type, const void *p);
nux_rid_t            nux_resource_rid(const void *data);

// arena.c

void      nux_arena_init(nux_arena_t *arena);
void      nux_arena_init_stack(nux_arena_t *arena, void *data, nux_u32_t capa);
void      nux_arena_free(nux_arena_t *arena);
void     *nux_arena_alloc(nux_arena_t *arena,
                          void        *optr,
                          nux_u32_t    osize,
                          nux_u32_t    nsize);
nux_c8_t *nux_arena_alloc_string(nux_arena_t *arena, const nux_c8_t *s);
void     *nux_arena_malloc(nux_arena_t *arena, nux_u32_t size);
void      nux_arena_cleanup(void *data);

// random.c

nux_pcg_t nux_pcg(nux_u64_t state, nux_u64_t incr);
nux_u32_t nux_pcg_u32(nux_pcg_t *pcg);
nux_f32_t nux_pcg_f32(nux_pcg_t *pcg);

// logger.c

void nux_vlog(nux_log_level_t level, const nux_c8_t *fmt, va_list args);
void nux_log(nux_log_level_t level, const nux_c8_t *fmt, ...);

// vector.c

nux_v3_t nux_v3_normalize(nux_v3_t a);

// matrix.c

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

// quaternion.c

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

// error.c

void            nux_error(const nux_c8_t *fmt, ...);
void            nux_error_enable(void);
void            nux_error_disable(void);
void            nux_error_reset(void);
const nux_c8_t *nux_error_get_message(void);
nux_status_t    nux_error_get_status(void);

// base.c

nux_status_t        nux_base_init(void *userdata);
void                nux_base_free(void);
const nux_config_t *nux_config(void);
void               *nux_userdata(void);

// event.c

nux_event_t         *nux_event_new(nux_arena_t        *arena,
                                   nux_event_type_t    type,
                                   nux_event_cleanup_t cleanup);
nux_event_handler_t *nux_event_subscribe(nux_arena_t         *arena,
                                         nux_event_t         *event,
                                         void                *userdata,
                                         nux_event_callback_t callback);
void                 nux_event_unsubscribe(const nux_event_handler_t *handler);
void nux_event_emit(nux_event_t *event, nux_u32_t size, const void *data);
void nux_event_process(nux_event_t *event);
void nux_event_process_all(void);

// io.c

nux_status_t nux_io_init(void);
void         nux_io_free(void);

nux_status_t nux_disk_mount(const nux_c8_t *path);

nux_b32_t    nux_file_exists(const nux_c8_t *path);
nux_file_t  *nux_file_open(nux_arena_t    *arena,
                           const nux_c8_t *path,
                           nux_io_mode_t   mode);
void         nux_file_close(nux_file_t *file);
nux_u32_t    nux_file_read(nux_file_t *file, void *data, nux_u32_t n);
nux_u32_t    nux_file_write(nux_file_t *file, const void *data, nux_u32_t n);
nux_status_t nux_file_seek(nux_file_t *file, nux_u32_t cursor);
nux_status_t nux_file_stat(nux_file_t *file, nux_file_stat_t *stat);
void *nux_file_load(nux_arena_t *arena, const nux_c8_t *path, nux_u32_t *size);

nux_status_t nux_io_write_cart_data(const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

// file.c

void nux_file_cleanup(void *data);

// disk.c

void nux_disk_cleanup(void *data);

// input.c

void nux_input_update(void);

// inputmap.c

nux_status_t nux_inputmap_find_index(const nux_inputmap_t *map,
                                     const nux_c8_t       *name,
                                     nux_u32_t            *index);

// serde.c

void nux_serde_writer_init(nux_serde_writer_t         *s,
                           void                       *userdata,
                           nux_serde_writer_callback_t callback);
void nux_serde_reader_init(nux_serde_reader_t         *s,
                           void                       *userdata,
                           nux_serde_reader_callback_t callback);

nux_status_t nux_json_writer_init(nux_json_writer_t *j, const nux_c8_t *path);
void         nux_json_writer_close(nux_json_writer_t *j);
nux_status_t nux_json_reader_init(nux_json_reader_t *j, const nux_c8_t *path);

void nux_serde_write(nux_serde_writer_t *s, const nux_serde_value_t *value);
void nux_serde_write_object(nux_serde_writer_t *s, const nux_c8_t *key);
void nux_serde_write_array(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           nux_u32_t           size);
void nux_serde_write_end(nux_serde_writer_t *s);
void nux_serde_write_u32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_u32_t           v);
void nux_serde_write_f32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_f32_t           v);
void nux_serde_write_v3(nux_serde_writer_t *s, const nux_c8_t *key, nux_v3_t v);
void nux_serde_write_q4(nux_serde_writer_t *s, const nux_c8_t *key, nux_q4_t v);
void nux_serde_write_bytes(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           const nux_u8_t     *bytes,
                           nux_u32_t           size);
void nux_serde_write_string(nux_serde_writer_t *s,
                            const nux_c8_t     *key,
                            const nux_c8_t     *v);
void nux_serde_write_rid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_rid_t           rid);
void nux_serde_write_eid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_nid_t           v);

void nux_serde_read(nux_serde_reader_t *s, nux_serde_value_t *value);
void nux_serde_read_object(nux_serde_reader_t *s, const nux_c8_t *key);
void nux_serde_read_array(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          nux_u32_t          *size);
void nux_serde_read_end(nux_serde_reader_t *s);
void nux_serde_read_u32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_u32_t          *v);
void nux_serde_read_f32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_f32_t          *v);
void nux_serde_read_v3(nux_serde_reader_t *s, const nux_c8_t *key, nux_v3_t *v);
void nux_serde_read_q4(nux_serde_reader_t *s, const nux_c8_t *key, nux_q4_t *v);
void nux_serde_read_bytes(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          const nux_u8_t    **v,
                          nux_u32_t          *n);
void nux_serde_read_string(nux_serde_reader_t *s,
                           const nux_c8_t     *key,
                           const nux_c8_t    **v,
                           nux_u32_t          *n);
void nux_serde_read_rid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_rid_t          *rid);
void nux_serde_read_nid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_nid_t          *v);

#endif
