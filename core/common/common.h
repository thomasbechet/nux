#ifndef NUX_COMMON_H
#define NUX_COMMON_H

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

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef __FILE_NAME__
#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__
#else
#define __SOURCE__ ""
#endif

#define NUX_BIG_ENDIAN (!*(unsigned char *)&(uint16_t) { 1 })

#if defined(NUX_BUILD_DEBUG) && defined(NUX_BUILD_STDLIB)
#define nux_assert(x) assert((x))
#else
#define nux_assert(x) (void)((x))
#endif

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

#define nux_unused0()
#define nux_unused1(a)             (void)(a)
#define nux_unused2(a, b)          (void)(a), nux_unused1(b)
#define nux_unused3(a, b, c)       (void)(a), nux_unused2(b, c)
#define nux_unused4(a, b, c, d)    (void)(a), nux_unused3(b, c, d)
#define nux_unused5(a, b, c, d, e) (void)(a), nux_unused4(b, c, d, e)

#define nux_check(check, action) \
    if (!(check))                \
    {                            \
        action;                  \
    }

#define nux_array_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define nux_array_fill(arr, size, value) \
    for (nux_u32_t i = 0; i < size; ++i) \
    {                                    \
        (arr)[i] = (value);              \
    }

#define nux_match(v, k) (nux_strncmp((v), k, nux_array_size(k)) == 0)

#define nux_min(a, b)          (((a) < (b)) ? (a) : (b))
#define nux_max(a, b)          (((a) > (b)) ? (a) : (b))
#define nux_clamp(x, min, max) (nux_max(min, nux_min(max, x)))
#define nux_clamp01(x)         (nux_clamp(x, 0, 1))
#define nux_abs(x)             (((x) < 0) ? -(x) : (x))
#define nux_swap(x, y, T) \
    do                    \
    {                     \
        T SWAP = x;       \
        x      = y;       \
        y      = SWAP;    \
    } while (0)

#define nux_color_hex(hex)                   \
    nux_color_rgba(((hex & 0xFF0000) >> 16), \
                   ((hex & 0xFF00) >> 8),    \
                   ((hex & 0xFF) >> 0),      \
                   ((hex & 0xFF000000) >> 24))
#define nux_color_rgba(r, g, b, a) \
    (nux_v4_t) { r / 255., g / 255., b / 255., a / 255. }

#define nux_v2_define(name, type)                                         \
    nux_##name##_t nux_##name(type x, type y);                            \
    nux_##name##_t nux_##name##s(type x);                                 \
    nux_##name##_t nux_##name##_zero(void);                               \
    nux_##name##_t nux_##name##_one(void);                                \
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

#define nux_v3_define(name, type)                                          \
    nux_##name##_t nux_##name(type x, type y, type z);                     \
    nux_##name##_t nux_##name##s(type x);                                  \
    nux_##name##_t nux_##name##_zero(void);                                \
    nux_##name##_t nux_##name##_one(void);                                 \
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

#define nux_v4_define(name, type)                                         \
    nux_##name##_t nux_##name(type x, type y, type z, type w);            \
    nux_##name##_t nux_##name##s(type x);                                 \
    nux_##name##_t nux_##name##_zero(void);                               \
    nux_##name##_t nux_##name##_one(void);                                \
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

#define nux_b3_define(name, type) nux_##name##_t nux_##name(type min, type max);

#define nux_vec(T)              \
    union                       \
    {                           \
        nux_vec_t vec;          \
        struct                  \
        {                       \
            nux_arena_t *arena; \
            nux_u32_t    osize; \
            nux_u32_t    size;  \
            nux_u32_t    capa;  \
            T           *data;  \
        };                      \
    }

#define nux_vec_init_capa(v, a, c) \
    nux__vec_init_capa(&(v)->vec, a, sizeof(*(v)->data), c)
#define nux_vec_init(v, a)      nux_vec_init_capa(v, a, 0)
#define nux_vec_reserve(v, c)   nux__vec_reserve(&(v)->vec, c)
#define nux_vec_resize(v, s)    nux__vec_resize(&(v)->vec, s)
#define nux_vec_get(v, i)       (typeof((v)->data))nux__vec_get(&(v)->vec, i)
#define nux_vec_last(v)         (typeof((v)->data))nux__vec_last(&(v)->vec)
#define nux_vec_push(v)         (typeof((v)->data))nux__vec_push(&(v)->vec)
#define nux_vec_pushv(v, value) (*nux_vec_push(v) = (value))
#define nux_vec_pop(v)          (typeof((v)->data))nux__vec_pop(&(v)->vec)
#define nux_vec_clear(v)        ((v)->vec).size = 0
#define nux_vec_swap(v, a, b)   nux__vec_swap(&(v)->vec, a, b)
#define nux_vec_swap_pop(v, i) \
    (typeof((v)->data))nux__vec_swap_pop(&(v)->vec, i)

#define nux_pool(T)                      \
    union                                \
    {                                    \
        nux_pool_t pool;                 \
        struct                           \
        {                                \
            nux_vec(nux_u32_t) freelist; \
            nux_u32_t osize;             \
            nux_u32_t capa;              \
            nux_u32_t size;              \
            T        *data;              \
        };                               \
    }
#define nux_pool_init_capa(p, a, c) \
    nux__pool_init_capa(&(p)->pool, a, sizeof(*(p)->data), c)
#define nux_pool_init(p, a)   nux_pool_init_capa(p, a, 0)
#define nux_pool_add(p)       (typeof((p)->data))nux__pool_add(&(p)->pool)
#define nux_pool_remove(p, i) nux__pool_remove(&(p)->pool, i)

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_SUCCESS = 1,
    NUX_FAILURE = 0
} nux_status_t;

typedef enum
{
    NUX_NULL          = 0,
    NUX_PATH_MAX      = 256, // does not include '\0'
    NUX_PATH_BUF_SIZE = NUX_PATH_MAX + 1
} nux_constants_t;

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

typedef struct
{
    nux_u32_t block_count;
    nux_u32_t block_size;
    nux_u32_t memory_usage;
    nux_u32_t memory_capacity;
    nux_u32_t memory_waste;
} nux_arena_info_t;

typedef void (*nux_arena_finalizer_t)(void *p);
typedef void *(*nux_arena_alloc_callback_t)(void     *ud,
                                            void     *p,
                                            nux_u32_t o,
                                            nux_u32_t n);
typedef void (*nux_arena_panic_callback_t)(void *ud);

typedef struct nux_arena_object
{
    struct nux_arena_object *prev;
    nux_arena_finalizer_t    finalize;
} nux_arena_object_t;

typedef struct block
{
    struct block *prev;
    struct block *next;
    nux_u32_t     bc;
} nux_arena_header_t;

typedef struct
{
    void                      *userdata;
    nux_arena_alloc_callback_t alloc;
    nux_arena_panic_callback_t panic;
    nux_arena_object_t        *last_object;
    nux_arena_info_t           info;

    nux_arena_header_t *first_header;
    nux_arena_header_t *last_header;
    nux_u8_t           *head;
    nux_u8_t           *end;
} nux_arena_t;

typedef struct
{
    nux_arena_t *arena;
    nux_u32_t    osize;
    nux_u32_t    size;
    nux_u32_t    capa;
    void        *data;
} nux_vec_t;

typedef struct
{
    nux_vec(nux_u32_t) freelist;
    nux_u32_t osize;
    nux_u32_t capa;
    nux_u32_t size;
    void     *data;
} nux_pool_t;

nux_v2_define(v2i, nux_i32_t);
nux_v2_define(v2u, nux_u32_t);
nux_v2_define(v2, nux_f32_t);
nux_v3_define(v3i, nux_i32_t);
nux_v3_define(v3u, nux_u32_t);
nux_v3_define(v3, nux_f32_t);
nux_v4_define(v4i, nux_i32_t);
nux_v4_define(v4u, nux_u32_t);
nux_v4_define(v4, nux_f32_t);
nux_b3_define(b3, nux_v3_t);
nux_b3_define(b3i, nux_v3i_t);

typedef nux_vec(nux_u32_t) nux_u32_vec_t;

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
nux_b2i_t nux_b2i_empty(void);
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
nux_v3_t nux_v3_up(void);
nux_v3_t nux_v3_down(void);
nux_v3_t nux_v3_forward(void);
nux_v3_t nux_v3_backward(void);
nux_v3_t nux_v3_left(void);
nux_v3_t nux_v3_right(void);

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

void             nux_arena_init(nux_arena_t               *arena,
                                void                      *userdata,
                                nux_arena_alloc_callback_t alloc,
                                nux_arena_panic_callback_t panic);
void             nux_arena_free(nux_arena_t *a);
nux_arena_info_t nux_arena_info(nux_arena_t *a);
void             nux_arena_clear(nux_arena_t *a);

void *nux_malloc(nux_arena_t *a, nux_u32_t size);
void *nux_mallocf(nux_arena_t          *a,
                  nux_u32_t             size,
                  nux_arena_finalizer_t finalizer);
void *nux_realloc(nux_arena_t *a, void *p, nux_u32_t osize, nux_u32_t nsize);

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v, nux_u32_t count);

void  nux__vec_init_capa(nux_vec_t   *vec,
                         nux_arena_t *a,
                         nux_u32_t    osize,
                         nux_u32_t    capa);
void  nux__vec_reserve(nux_vec_t *vec, nux_u32_t capa);
void  nux__vec_resize(nux_vec_t *vec, nux_u32_t size);
void *nux__vec_get(nux_vec_t *vec, nux_u32_t i);
void *nux__vec_last(nux_vec_t *vec);
void *nux__vec_push(nux_vec_t *vec);
void *nux__vec_pop(nux_vec_t *vec);
void  nux__vec_swap(nux_vec_t *vec, nux_u32_t a, nux_u32_t b);
void *nux__vec_swap_pop(nux_vec_t *vec, nux_u32_t i);

void  nux__pool_init_capa(nux_pool_t  *pool,
                          nux_arena_t *arena,
                          nux_u32_t    osize,
                          nux_u32_t    capa);
void *nux__pool_add(nux_pool_t *pool);
void  nux__pool_remove(nux_pool_t *pool, void *i);

#endif
