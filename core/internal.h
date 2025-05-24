#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#ifdef NUX_STDLIB
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_TRUE  1
#define NUX_FALSE 0
#define NUX_NULL  0
#define NUX_NOOP

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

#if defined(NUX_DEBUG) && defined(NUX_STDLIB)
#define NUX_ASSERT(x) assert((x))
#else
#define NUX_ASSERT(x) (void)((x))
#endif

// math.c

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

#define NUX_PI      3.14159265359
#define NUX_FLT_MAX 3.402823E+38
#define NUX_FLT_MIN 1.175494e-38
#define NUX_I32_MAX 2147483647
#define NUX_I32_MIN -2147483648

#define NUX_V2_SIZE 2
#define NUX_V3_SIZE 3
#define NUX_V4_SIZE 4
#define NUX_Q4_SIZE 4
#define NUX_M3_SIZE 9
#define NUX_M4_SIZE 16

#define NUX_V2_DEFINE(name, type)                                        \
    typedef union                                                        \
    {                                                                    \
        struct                                                           \
        {                                                                \
            type x;                                                      \
            type y;                                                      \
        };                                                               \
        type data[NUX_V2_SIZE];                                          \
    } nux_##name##_t;                                                    \
    nux_##name##_t nux_##name(type x, type y);                           \
    nux_##name##_t nux_##name##s(type x);                                \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b); \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b);          \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);

#define NUX_V3_DEFINE(name, type)                                          \
    typedef union                                                          \
    {                                                                      \
        struct                                                             \
        {                                                                  \
            type x;                                                        \
            type y;                                                        \
            type z;                                                        \
        };                                                                 \
        type data[NUX_V3_SIZE];                                            \
    } nux_##name##_t;                                                      \
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
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);

#define NUX_V4_DEFINE(name, type)                                        \
    typedef union                                                        \
    {                                                                    \
        struct                                                           \
        {                                                                \
            type x;                                                      \
            type y;                                                      \
            type z;                                                      \
            type w;                                                      \
        };                                                               \
        type data[NUX_V4_SIZE];                                          \
    } nux_##name##_t;                                                    \
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

#define NUX_VEC(type)   \
    struct              \
    {                   \
        type     *data; \
        nux_u32_t capa; \
        nux_u32_t size; \
    }
#define NUX_VEC_INIT(v, ptr, cap) \
    {                             \
        (v)->data = (ptr);        \
        (v)->capa = (cap);        \
        (v)->size = 0;            \
    }
#define NUX_VEC_PUSH(v) \
    ((v)->size >= (v)->capa ? NUX_NULL : &(v)->data[(v)->size++])
#define NUX_VEC_PUSHN(v, n) \
    ((v)->size + n >= (v)->capa ? NUX_NULL : &(v)->data[(v)->size += n])
#define NUX_VEC_CLEAR(v) (v)->size = 0
#define NUX_VEC_POP(v)   (v)->size ? &(v)->data[(v)->size--] : NUX_NULL
#define NUX_VEC_GET(v, index) \
    ((index) >= (v)->size ? NUX_NULL : &(v)->data[index])

// Vector data structure should be enought to cover all engine cases.
// Try not to create too much abstraction (i.e. pools)

////////////////////////////
///        TYPES         ///
////////////////////////////

NUX_V2_DEFINE(v2i, nux_i32_t);
NUX_V2_DEFINE(v2u, nux_u32_t);
NUX_V2_DEFINE(v2, nux_f32_t);
NUX_V3_DEFINE(v3i, nux_i32_t);
NUX_V3_DEFINE(v3u, nux_u32_t);
NUX_V3_DEFINE(v3, nux_f32_t);
NUX_V4_DEFINE(v4i, nux_i32_t);
NUX_V4_DEFINE(v4u, nux_u32_t);
NUX_V4_DEFINE(v4, nux_f32_t);

typedef struct
{
    nux_v2i_t min;
    nux_v2i_t max;
} nux_b2i_t;

typedef union
{
    struct
    {
        nux_f32_t x;
        nux_f32_t y;
        nux_f32_t z;
        nux_f32_t w;
    };
    nux_f32_t data[NUX_Q4_SIZE];
} nux_q4_t;

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

struct nux_env
{
    nux_instance_t *inst;

    // Error handling
    nux_error_t error;
    nux_c8_t    error_message[256];

    // Stats
    nux_u32_t tricount;
};

typedef struct
{
    nux_u32_t width;
    nux_u32_t height;
    nux_u8_t *data;
} nux_texture_t;

typedef struct
{
    void     *data;
    nux_u32_t size;
    nux_u32_t first_object;
} nux_section_t;

typedef enum
{
    NUX_OBJECT_NULL,
    NUX_OBJECT_LUA,
    NUX_OBJECT_SECTION,
    NUX_OBJECT_TEXTURE,
} nux_object_type_t;

typedef struct
{
    nux_u32_t         next;
    nux_u32_t         prev;
    nux_object_type_t type;
    union
    {
        nux_section_t section;
        nux_texture_t texture;
    } data;
} nux_object_t;

struct nux_instance
{
    void     *userdata;
    nux_b32_t running;
    nux_u64_t frame;
    nux_f32_t time;
    nux_u8_t  pal[NUX_PALETTE_SIZE];
    nux_u32_t colormap[NUX_COLORMAP_SIZE];
    nux_v2i_t cursor;
    nux_v3_t  cam_eye;
    nux_v3_t  cam_center;
    nux_v3_t  cam_up;
    nux_b2i_t cam_viewport;
    nux_f32_t cam_fov;
    nux_u8_t *canvas;
    nux_b2i_t tex_view;

    nux_u32_t buttons[NUX_PLAYER_MAX];
    nux_f32_t axis[NUX_PLAYER_MAX][NUX_AXIS_MAX];

    nux_u32_t stats[NUX_STAT_MAX];

    NUX_VEC(nux_u8_t) memory;
    NUX_VEC(nux_object_t) objects;
    NUX_VEC(nux_u32_t) free_objects;

    lua_State *L;

    struct nux_env env;
    nux_callback_t init;
    nux_callback_t update;
};

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

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

nux_b2i_t nux_b2i(nux_v2i_t min, nux_v2i_t max);
nux_b2i_t nux_b2i_xywh(nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h);
nux_b2i_t nux_b2i_translate(nux_b2i_t b, nux_v2i_t t);
nux_b2i_t nux_b2i_moveto(nux_b2i_t b, nux_v2i_t p);

// string.c

nux_u32_t nux_strnlen(const nux_c8_t *s, nux_u32_t n);
void      nux_strncpy(nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n);
void      nux_snprintf(nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, ...);
void      nux_vsnprintf(nux_c8_t       *buf,
                        nux_u32_t       n,
                        const nux_c8_t *format,
                        va_list         args);

// memory.c

nux_i32_t nux_memcmp(const void *p0, const void *p1, nux_u32_t n);
void     *nux_memset(void *dst, nux_u32_t c, nux_u32_t n);
void      nux_memcpy(void *dst, const void *src, nux_u32_t n);
void      nux_memswp(void *a, void *b, nux_u32_t n);
void     *nux_memalign(void *ptr, nux_u32_t align);

// vector.c

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
nux_v3_t nux_m4_mulv3(nux_m4_t a, nux_v3_t v);
nux_m4_t nux_m4_trs(nux_v3_t t, nux_q4_t r, nux_v3_t s);

// quaternion.c

nux_q4_t  nux_q4(nux_f32_t x, nux_f32_t y, nux_f32_t z, nux_f32_t w);
nux_q4_t  nux_q4_identity(void);
nux_v4_t  nux_q4_vec4(nux_q4_t a);
nux_f32_t nux_q4_norm(nux_q4_t a);
nux_q4_t  nux_q4_axis(nux_v3_t axis, nux_f32_t angle);
nux_q4_t  nux_q4_mul(nux_q4_t a, nux_q4_t b);
nux_v3_t  nux_q4_mulv3(nux_q4_t a, nux_v3_t v);
nux_q4_t  nux_q4_mul_axis(nux_q4_t q, nux_v3_t axis, nux_f32_t angle);
nux_m4_t  nux_q4_mat4(nux_q4_t q);
nux_m4_t  nux_q4_mulm4(nux_q4_t a, nux_m4_t m);

// camera.c

nux_m4_t nux_perspective(nux_f32_t fov,
                         nux_f32_t aspect_ratio,
                         nux_f32_t z_near,
                         nux_f32_t z_far);
nux_m4_t nux_ortho(nux_f32_t left,
                   nux_f32_t right,
                   nux_f32_t bottom,
                   nux_f32_t top,
                   nux_f32_t near,
                   nux_f32_t far);
nux_m4_t nux_lookat(nux_v3_t eye, nux_v3_t center, nux_v3_t up);

// instance.c

/**
 * Allocate memory in the current section
 * @param size requested memory size
 * @return pointer to memory
 */
void *nux_malloc(nux_env_t env, nux_u32_t size);
void  nux_set_error(nux_env_t env, nux_error_t error);

void *nux_add_object(nux_env_t env, nux_object_type_t type, nux_u32_t *id);
nux_status_t nux_remove_object(nux_env_t env, nux_u32_t id);
void *nux_check_object(nux_env_t env, nux_u32_t id, nux_object_type_t type);

nux_status_t nux_register_lua(nux_instance_t *inst);

#endif
