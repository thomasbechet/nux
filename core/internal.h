#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#ifdef NUX_STDLIB
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

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

#define NUX_CHECK(check, action) \
    if (!(check))                \
    {                            \
        action;                  \
    }
#ifdef NUX_DEBUG
#define NUX_CHECKM(check, message, action) \
    NUX_ASSERT((check));                   \
    if (!(check))                          \
    {                                      \
        fprintf(stderr, "%s", (message));  \
        action;                            \
    }
#else
#define NUX_CHECKM(check, message, action) \
    if (!(check))                          \
    {                                      \
        action;                            \
    }
#endif

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

#define NUX_B3_DEFINE(name, type) \
    typedef struct                \
    {                             \
        type min;                 \
        type max;                 \
    } nux_##name##_t;             \
    nux_##name##_t nux_##name(type min, type max);

#define NUX_VEC_DEFINE(name, T)                                             \
    typedef struct                                                          \
    {                                                                       \
        nux_u32_t size;                                                     \
        nux_u32_t capa;                                                     \
        T        *data;                                                     \
    } name##_t;                                                             \
    nux_status_t name##_alloc(nux_env_t *env, nux_u32_t capa, name##_t *v); \
    void         name##_init(void *p, nux_u32_t capa, name##_t *v);         \
    T           *name##_push(name##_t *v);                                  \
    nux_b32_t    name##_pushv(name##_t *v, T val);                          \
    T           *name##_pop(name##_t *v);                                   \
    void         name##_clear(name##_t *v);                                 \
    T           *name##_get(name##_t *v, nux_u32_t i);                      \
    void         name##_fill_reverse_indices(name##_t *v);
#define NUX_VEC_IMPL(name, T)                                              \
    nux_status_t name##_alloc(nux_env_t *env, nux_u32_t capa, name##_t *v) \
    {                                                                      \
        void *p = nux_alloc(env, sizeof(*(v)->data) * capa);               \
        NUX_CHECK(p, return NUX_FAILURE);                                  \
        name##_init(p, capa, v);                                           \
        return NUX_SUCCESS;                                                \
    }                                                                      \
    void name##_init(void *p, nux_u32_t capa, name##_t *v)                 \
    {                                                                      \
        NUX_ASSERT(capa);                                                  \
        (v)->capa = capa;                                                  \
        (v)->size = 0;                                                     \
        (v)->data = p;                                                     \
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
        return ((v)->size) ? (v)->data + (v)->size++ : NUX_NULL;           \
    }                                                                      \
    void name##_clear(name##_t *v)                                         \
    {                                                                      \
        (v)->size = 0;                                                     \
    }                                                                      \
    T *name##_get(name##_t *v, nux_u32_t i)                                \
    {                                                                      \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                   \
    }

#define NUX_NEW(env, type, struct, id) \
    (struct *)nux_new((env), type, sizeof(struct), (id))

////////////////////////////
///        TYPES         ///
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

typedef struct
{
    nux_u32_t arena;
    nux_u32_t size;
    nux_u32_t object;
} nux_frame_t;

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

    // Arena
    nux_u32_t arena;
};

typedef struct
{
    nux_u32_t            slot;
    nux_texture_format_t format;
    nux_u32_t            width;
    nux_u32_t            height;
    nux_u8_t            *data;
} nux_texture_t;

typedef struct
{
    nux_u32_t texture; // owned object
    nux_u32_t slot;    // framebuffer slot
} nux_render_target_t;

typedef struct
{
    nux_f32_t *data;
    nux_u32_t  first;
    nux_u32_t  count;
} nux_mesh_t;

typedef struct
{
    void     *data;
    nux_u32_t capa;
    nux_u32_t size;
    nux_u32_t last_object;
} nux_arena_t;

typedef enum
{
    NUX_OBJECT_NULL = 0,
    NUX_OBJECT_ARENA,
    NUX_OBJECT_LUA,
    NUX_OBJECT_TEXTURE,
    NUX_OBJECT_RENDER_TARGET,
    NUX_OBJECT_MESH,
} nux_object_type_t;

typedef struct
{
    nux_u32_t         prev;
    nux_u32_t         arena;
    nux_object_type_t type;
    void             *data;
} nux_object_t;

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_object_vec, nux_object_t)

typedef struct
{
    nux_m4_t  view;
    nux_m4_t  proj;
    nux_m4_t  model;
    nux_v2u_t canvas_size;
    nux_v2u_t screen_size;
    nux_f32_t time;
    nux_u32_t _pad[3];
} nux_gpu_constants_buffer_t;

typedef struct
{
    nux_u32_t texture_type;
} nux_gpu_draw_constants_t;

typedef struct
{
    nux_gpu_pass_type_t type;
    nux_u32_t           render_target;
    nux_u32_t           command_first;
    nux_u32_t           commant_count;
} nux_render_pass_t;

typedef struct
{

} nux_render_command_t;

NUX_VEC_DEFINE(nux_render_pass_vec, nux_render_pass_t)
NUX_VEC_DEFINE(nux_render_command_vec, nux_render_command_t)

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

    nux_u32_t buttons[NUX_PLAYER_MAX];
    nux_f32_t axis[NUX_PLAYER_MAX * NUX_AXIS_MAX];

    nux_u32_t stats[NUX_STAT_MAX];

    nux_gpu_texture_info_t canvas_info;
    nux_gpu_texture_info_t colormap_info;
    nux_u32_t              vertex_storage_head;

    nux_u32_t test_cube;

    nux_arena_t      core_arena;
    nux_u32_t        core_arena_id;
    nux_object_vec_t objects;
    nux_u32_vec_t    objects_freelist;

    nux_u32_vec_t free_texture_slots;
    nux_u32_vec_t free_buffer_slots;
    nux_u32_vec_t free_framebuffer_slots;

    lua_State *L;

    struct nux_env env;
    nux_callback_t init;
    nux_callback_t update;
};

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

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

// arena.c

void *nux_new(nux_env_t        *env,
              nux_object_type_t type,
              nux_u32_t         ssize,
              nux_u32_t        *id);
void *nux_get(nux_env_t *env, nux_object_type_t type, nux_u32_t id);

void  nux_arena_cleanup(nux_env_t *env, void *data);
void *nux_arena_alloc(nux_arena_t *arena, nux_u32_t size);
void *nux_alloc(nux_env_t *env, nux_u32_t size);
void *nux_realloc(nux_env_t *env, void *p, nux_u32_t osize, nux_u32_t nsize);

nux_frame_t nux_begin_frame(nux_env_t *env);
void        nux_reset_frame(nux_env_t *env, nux_frame_t frame);

// texture.c

void nux_texture_cleanup(nux_env_t *env, void *data);
void nux_texture_write(nux_env_t      *env,
                       nux_u32_t       id,
                       nux_u32_t       x,
                       nux_u32_t       y,
                       nux_u32_t       w,
                       nux_u32_t       h,
                       const nux_u8_t *data);

// render_target.c

void nux_render_target_cleanup(nux_env_t *env, void *data);

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

// graphics.c

nux_status_t nux_graphics_init(nux_env_t *env);
nux_status_t nux_graphics_free(nux_env_t *env);
nux_status_t nux_graphics_render(nux_env_t *env);

nux_status_t nux_graphics_push_vertices(nux_env_t       *env,
                                        nux_u32_t        vcount,
                                        const nux_f32_t *data,
                                        nux_u32_t       *first);

nux_u32_t nux_generate_cube(nux_env_t *env,
                            nux_f32_t  sx,
                            nux_f32_t  sy,
                            nux_f32_t  sz);

// instance.c

void nux_set_error(nux_env_t *env, nux_error_t error);

nux_status_t nux_register_lua(nux_instance_t *inst);

#endif
