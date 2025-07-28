#ifndef NUX_BASE_H
#define NUX_BASE_H

#include "nux.h"

#ifdef NUX_BUILD_STDLIB
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
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
#define NUX_ENSURE(check, action, format, ...)                      \
    if (!(check))                                                   \
    {                                                               \
        nux_error(ctx, format " at %s", ##__VA_ARGS__, __SOURCE__); \
        action;                                                     \
    }

#define NUX_DEBUG(format, ...) \
    nux_log(ctx, NUX_LOG_DEBUG, format, ##__VA_ARGS__)
#define NUX_INFO(format, ...) nux_log(ctx, NUX_LOG_INFO, format, ##__VA_ARGS__)
#define NUX_WARNING(format, ...) \
    nux_log(ctx, NUX_LOG_WARNING, format, ##__VA_ARGS__)
#define NUX_ERROR(format, ...) \
    nux_log(ctx, NUX_LOG_ERROR, format, ##__VA_ARGS__)

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

#define NUX_V3_ONES     nux_v3s(1)
#define NUX_V3_ZEROES   nux_v3s(0)
#define NUX_V3_UP       nux_v3(0, 1, 0)
#define NUX_V3_DOWN     nux_v3(0, -1, 0)
#define NUX_V3_FORWARD  nux_v3(0, 0, -1)
#define NUX_V3_BACKWARD nux_v3(0, 0, 1)
#define NUX_V3_LEFT     nux_v3(-1, 0, 0)
#define NUX_V3_RIGHT    nux_v3(1, 0, 0)

#define NUX_V2_DEFINE(name, type)                                        \
    nux_##name##_t nux_##name(type x, type y);                           \
    nux_##name##_t nux_##name##s(type x);                                \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b);

#define NUX_V3_DEFINE(name, type)                                          \
    nux_##name##_t nux_##name(type x, type y, type z);                     \
    nux_##name##_t nux_##name##s(type x);                                  \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);            \
    nux_##name##_t nux_##name##_cross(nux_##name##_t a, nux_##name##_t b); \
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b);   \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b);

#define NUX_V4_DEFINE(name, type)                                        \
    nux_##name##_t nux_##name(type x, type y, type z, type w);           \
    nux_##name##_t nux_##name##s(type x);                                \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);          \
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);

#define NUX_B3_DEFINE(name, type) nux_##name##_t nux_##name(type min, type max);

#define NUX_VEC_DEFINE(name, T)                                             \
    typedef struct                                                          \
    {                                                                       \
        nux_arena_t *arena;                                                 \
        nux_u32_t    size;                                                  \
        nux_u32_t    capa;                                                  \
        T           *data;                                                  \
    } name##_t;                                                             \
    nux_status_t name##_alloc(nux_ctx_t *ctx, nux_u32_t capa, name##_t *v); \
    void         name##_init(T *data, nux_u32_t capa, name##_t *v);         \
    T           *name##_push(name##_t *v);                                  \
    nux_b32_t    name##_pushv(name##_t *v, T val);                          \
    T           *name##_pop(name##_t *v);                                   \
    void         name##_clear(name##_t *v);                                 \
    T           *name##_get(name##_t *v, nux_u32_t i);                      \
    void         name##_fill_reverse_indices(name##_t *v);
#define NUX_VEC_IMPL(name, T)                                              \
    nux_status_t name##_alloc(nux_ctx_t *ctx, nux_u32_t capa, name##_t *v) \
    {                                                                      \
        NUX_ASSERT(capa);                                                  \
        (v)->data = nux_arena_alloc(ctx, sizeof(*(v)->data) * capa);       \
        NUX_CHECK((v)->data, return NUX_FAILURE);                          \
        (v)->arena = ctx->active_arena;                                    \
        (v)->capa  = capa;                                                 \
        (v)->size  = 0;                                                    \
        return NUX_SUCCESS;                                                \
    }                                                                      \
    void name##_init(T *data, nux_u32_t capa, name##_t *v)                 \
    {                                                                      \
        NUX_ASSERT(capa);                                                  \
        (v)->data  = data;                                                 \
        (v)->arena = NUX_NULL;                                             \
        (v)->capa  = capa;                                                 \
        (v)->size  = 0;                                                    \
    }                                                                      \
    T *name##_push(name##_t *v)                                            \
    {                                                                      \
        if ((v)->size >= (v)->capa)                                        \
        {                                                                  \
            return NUX_NULL;                                               \
        }                                                                  \
        T *ret = (v)->data + (v)->size;                                    \
        ++(v)->size;                                                       \
        return ret;                                                        \
    }                                                                      \
    nux_b32_t name##_pushv(name##_t *v, T val)                             \
    {                                                                      \
        T *a = name##_push(v);                                             \
        if (a)                                                             \
        {                                                                  \
            *a = val;                                                      \
            return NUX_TRUE;                                               \
        }                                                                  \
        return NUX_FALSE;                                                  \
    }                                                                      \
    T *name##_pop(name##_t *v)                                             \
    {                                                                      \
        if (!(v)->size)                                                    \
            return NUX_NULL;                                               \
        T *ret = &(v)->data[(v)->size - 1];                                \
        --(v)->size;                                                       \
        return ret;                                                        \
    }                                                                      \
    void name##_clear(name##_t *v)                                         \
    {                                                                      \
        (v)->size = 0;                                                     \
    }                                                                      \
    T *name##_get(name##_t *v, nux_u32_t i)                                \
    {                                                                      \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                   \
    }

#define NUX_POOL_DEFINE(name, T)                                            \
    typedef struct                                                          \
    {                                                                       \
        nux_arena_t  *arena;                                                \
        nux_u32_t     capa;                                                 \
        nux_u32_t     size;                                                 \
        nux_u32_vec_t freelist;                                             \
        T            *data;                                                 \
    } name##_t;                                                             \
    nux_status_t name##_alloc(nux_ctx_t *ctx, nux_u32_t capa, name##_t *p); \
    T           *name##_add(name##_t *p);                                   \
    void         name##_remove(name##_t *p, T *i);
#define NUX_POOL_IMPL(name, T)                                             \
    nux_status_t name##_alloc(nux_ctx_t *ctx, nux_u32_t capa, name##_t *p) \
    {                                                                      \
        NUX_ASSERT(capa);                                                  \
        p->data = nux_arena_alloc(ctx, sizeof(*p->data) * capa);           \
        NUX_CHECK(p->data, return NUX_FAILURE);                            \
        nux_u32_vec_alloc(ctx, capa, &p->freelist);                        \
        p->arena = ctx->active_arena;                                      \
        p->capa  = capa;                                                   \
        p->size  = 0;                                                      \
        return NUX_SUCCESS;                                                \
    }                                                                      \
    T *name##_add(name##_t *p)                                             \
    {                                                                      \
        if (p->freelist.size)                                              \
        {                                                                  \
            nux_u32_t free = *nux_u32_vec_pop(&p->freelist);               \
            return &p->data[free];                                         \
        }                                                                  \
        else                                                               \
        {                                                                  \
            if (p->size >= p->capa)                                        \
            {                                                              \
                return NUX_NULL;                                           \
            }                                                              \
            T *data = p->data + p->size;                                   \
            ++p->size;                                                     \
            return data;                                                   \
        }                                                                  \
    }                                                                      \
    void name##_remove(name##_t *p, T *i)                                  \
    {                                                                      \
        nux_u32_t index = i - p->data;                                     \
        NUX_ASSERT(index < p->size);                                       \
        nux_u32_vec_pushv(&p->freelist, index);                            \
    }

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_TYPE_NULL       = 0,
    NUX_TYPE_ARENA      = 1,
    NUX_TYPE_LUA        = 2,
    NUX_TYPE_TEXTURE    = 3,
    NUX_TYPE_MESH       = 4,
    NUX_TYPE_CANVAS     = 5,
    NUX_TYPE_SCENE      = 6,
    NUX_TYPE_NODE       = 7,
    NUX_TYPE_TRANSFORM  = 8,
    NUX_TYPE_CAMERA     = 9,
    NUX_TYPE_STATICMESH = 10,
    NUX_TYPE_FILE       = 11,

    NUX_TYPE_MAX = 256,
} nux_type_base_t;

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
        nux_u32_t x;
        nux_u32_t y;
    };
    nux_u32_t data[2];
} nux_v2u_t;

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

typedef struct
{
    nux_v2i_t min;
    nux_v2i_t max;
} nux_b2i_t;

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
    nux_f32_t data[NUX_M4_SIZE];
} nux_m4_t;

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

typedef struct nux_resource_header
{
    nux_id_t                    id;
    nux_u32_t                   type;
    struct nux_resource_header *prev;
    struct nux_resource_header *next;
} nux_resource_header_t;

typedef struct
{
    nux_id_t               id;
    void                  *data;
    nux_u32_t              capa;
    nux_u32_t              size;
    nux_resource_header_t *first_resource;
    nux_resource_header_t *last_resource;
} nux_arena_t;

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)

typedef void (*nux_resource_cleanup_t)(nux_ctx_t *ctx, void *data);
typedef struct
{
    const nux_c8_t        *name;
    nux_resource_cleanup_t cleanup;
    nux_u32_t              component_type;
} nux_resource_t;

typedef struct
{
    nux_u8_t  version;
    nux_u32_t type;
    void     *data;
} nux_id_entry_t;

NUX_POOL_DEFINE(nux_id_pool, nux_id_entry_t);

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

// string.c

nux_u32_t nux_strnlen(const nux_c8_t *s, nux_u32_t n);
void      nux_strncpy(nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n);
nux_i32_t nux_strncmp(const nux_c8_t *a, const nux_c8_t *b, nux_u32_t n);
nux_u32_t nux_snprintf(nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, ...);
nux_u32_t nux_vsnprintf(nux_c8_t       *buf,
                        nux_u32_t       n,
                        const nux_c8_t *format,
                        va_list         args);

// memory.c

nux_i32_t nux_memcmp(const void *p0, const void *p1, nux_u32_t n);
void     *nux_memset(void *dst, nux_u32_t c, nux_u32_t n);
void      nux_memcpy(void *dst, const void *src, nux_u32_t n);
void      nux_memswp(void *a, void *b, nux_u32_t n);
void     *nux_memalign(void *ptr, nux_u32_t align);
nux_u32_t nux_u32_le(nux_u32_t v);
nux_f32_t nux_f32_le(nux_f32_t v);
nux_u32_t nux_hash(const void *p, nux_u32_t s);

// type.c

nux_resource_t *nux_resource_register(nux_ctx_t *ctx, const nux_c8_t *name);

// arena.c

void    *nux_arena_alloc_raw(nux_arena_t *arena, nux_u32_t size);
void    *nux_arena_alloc(nux_ctx_t *ctx, nux_u32_t size);
void    *nux_arena_alloc_resource(nux_ctx_t *ctx,
                                  nux_u32_t  type,
                                  nux_u32_t  size,
                                  nux_id_t  *id);
nux_id_t nux_arena_get_active(nux_ctx_t *ctx);
void     nux_arena_set_active(nux_ctx_t *ctx, nux_id_t id);

// id.c

nux_id_t nux_id_create(nux_ctx_t *ctx, nux_u32_t type, void *data);
void     nux_id_delete(nux_ctx_t *ctx, nux_id_t id);
void     nux_id_update(nux_ctx_t *ctx, nux_id_t id, void *data);
void    *nux_id_check(nux_ctx_t *ctx, nux_u32_t type, nux_id_t id);

// random.c

nux_pcg_t nux_pcg(nux_u64_t state, nux_u64_t incr);
nux_u32_t nux_pcg_u32(nux_pcg_t *pcg);
nux_f32_t nux_pcg_f32(nux_pcg_t *pcg);

// logger.c

void nux_vlog(nux_ctx_t      *ctx,
              nux_log_level_t level,
              const nux_c8_t *fmt,
              va_list         args);
void nux_log(nux_ctx_t *ctx, nux_log_level_t level, const nux_c8_t *fmt, ...);

// vector.c

nux_f32_t nux_v3_norm(nux_v3_t a);
nux_v3_t  nux_v3_normalize(nux_v3_t a);
nux_f32_t nux_v4_norm(nux_v4_t a);

// matrix.c

nux_m3_t nux_m3_zero(void);
nux_m3_t nux_m3_identity(void);
nux_m3_t nux_m3_translate(nux_v2_t v);
nux_m3_t nux_m3_scale(nux_v2_t v);
nux_m3_t nux_m3_mul(nux_m3_t a, nux_m3_t b);

nux_m4_t nux_m4(const nux_f32_t *p);
nux_m4_t nux_m4_zero(void);
nux_m4_t nux_m4_identity(void);
nux_m4_t nux_m4_translate(nux_v3_t v);
nux_m4_t nux_m4_scale(nux_v3_t v);
nux_m4_t nux_m4_rotate_y(nux_f32_t angle);
nux_m4_t nux_m4_mul(nux_m4_t a, nux_m4_t b);
nux_v4_t nux_m4_mulv(nux_m4_t a, nux_v4_t v);
nux_v3_t nux_m4_mulv3(nux_m4_t a, nux_v3_t v, nux_f32_t w);
nux_m4_t nux_m4_trs(nux_v3_t t, nux_q4_t r, nux_v3_t s);
void nux_m4_trs_decompose(nux_m4_t m, nux_v3_t *t, nux_q4_t *r, nux_v3_t *s);

// quaternion.c

nux_q4_t  nux_q4(nux_f32_t x, nux_f32_t y, nux_f32_t z, nux_f32_t w);
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

// context.c

void            nux_error(nux_ctx_t *ctx, const nux_c8_t *fmt, ...);
void            nux_error_reset(nux_ctx_t *ctx);
const nux_c8_t *nux_error_get_message(nux_ctx_t *ctx);
nux_status_t    nux_error_get_status(nux_ctx_t *ctx);

#endif
