#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#ifdef NUX_BUILD_STDLIB
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
    if (!(check))                          \
    {                                      \
        NUX_ERROR("%s", (message));        \
        action;                            \
    }
#else
#define NUX_CHECKM(check, message, action) \
    if (!(check))                          \
    {                                      \
        action;                            \
    }
#endif

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
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b);

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
    type           nux_##name##_dot(nux_##name##_t a, nux_##name##_t b);

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

#define NUX_VEC_DEFINE(name, T)                           \
    typedef struct                                        \
    {                                                     \
        nux_arena_t *arena;                               \
        nux_u32_t    size;                                \
        nux_u32_t    capa;                                \
        T           *data;                                \
    } name##_t;                                           \
    nux_status_t name##_alloc(                            \
        nux_arena_t *arena, nux_u32_t capa, name##_t *v); \
    T        *name##_push(name##_t *v);                   \
    nux_b32_t name##_pushv(name##_t *v, T val);           \
    T        *name##_pop(name##_t *v);                    \
    void      name##_clear(name##_t *v);                  \
    T        *name##_get(name##_t *v, nux_u32_t i);       \
    void      name##_fill_reverse_indices(name##_t *v);
#define NUX_VEC_IMPL(name, T)                                                  \
    nux_status_t name##_alloc(nux_arena_t *arena, nux_u32_t capa, name##_t *v) \
    {                                                                          \
        NUX_ASSERT(capa);                                                      \
        (v)->data = nux_arena_alloc(arena, sizeof(*(v)->data) * capa);         \
        NUX_CHECK((v)->data, return NUX_FAILURE);                              \
        (v)->arena = arena;                                                    \
        (v)->capa  = capa;                                                     \
        (v)->size  = 0;                                                        \
        return NUX_SUCCESS;                                                    \
    }                                                                          \
    T *name##_push(name##_t *v)                                                \
    {                                                                          \
        if ((v)->size >= (v)->capa)                                            \
        {                                                                      \
            return NUX_NULL;                                                   \
        }                                                                      \
        T *ret = (v)->data + (v)->size;                                        \
        ++(v)->size;                                                           \
        return ret;                                                            \
    }                                                                          \
    nux_b32_t name##_pushv(name##_t *v, T val)                                 \
    {                                                                          \
        T *a = name##_push(v);                                                 \
        if (a)                                                                 \
        {                                                                      \
            *a = val;                                                          \
            return NUX_TRUE;                                                   \
        }                                                                      \
        return NUX_FALSE;                                                      \
    }                                                                          \
    T *name##_pop(name##_t *v)                                                 \
    {                                                                          \
        if (!(v)->size)                                                        \
            return NUX_NULL;                                                   \
        T *ret = &(v)->data[(v)->size - 1];                                    \
        --(v)->size;                                                           \
        return ret;                                                            \
    }                                                                          \
    void name##_clear(name##_t *v)                                             \
    {                                                                          \
        (v)->size = 0;                                                         \
    }                                                                          \
    T *name##_get(name##_t *v, nux_u32_t i)                                    \
    {                                                                          \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                       \
    }

#define NUX_POOL_DEFINE(name, T)                          \
    typedef struct                                        \
    {                                                     \
        nux_arena_t  *arena;                              \
        nux_u32_t     capa;                               \
        nux_u32_t     size;                               \
        nux_u32_vec_t freelist;                           \
        T            *data;                               \
    } name##_t;                                           \
    nux_status_t name##_alloc(                            \
        nux_arena_t *arena, nux_u32_t capa, name##_t *p); \
    T   *name##_add(name##_t *p);                         \
    void name##_remove(name##_t *p, T *i);
#define NUX_POOL_IMPL(name, T)                                                 \
    nux_status_t name##_alloc(nux_arena_t *arena, nux_u32_t capa, name##_t *p) \
    {                                                                          \
        NUX_ASSERT(capa);                                                      \
        p->data = nux_arena_alloc(arena, sizeof(*p->data) * capa);             \
        NUX_CHECK(p->data, return NUX_FAILURE);                                \
        nux_u32_vec_alloc(arena, capa, &p->freelist);                          \
        p->arena = arena;                                                      \
        p->capa  = capa;                                                       \
        p->size  = 0;                                                          \
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
                return NUX_NULL;                                               \
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
    nux_u32_t id;
    void     *data;
    nux_u32_t capa;
    nux_u32_t size;
    nux_u32_t first_object;
    nux_u32_t last_object;
} nux_arena_t;

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)

typedef struct
{
    nux_u32_t          slot;
    nux_u32_t          framebuffer_slot;
    nux_texture_type_t type;
    nux_u32_t          width;
    nux_u32_t          height;
    nux_u8_t          *data;
} nux_texture_t;

typedef struct
{
    nux_f32_t *data;
    nux_u32_t  first;
    nux_u32_t  count;
} nux_mesh_t;

typedef struct
{
    nux_b2i_t viewport;
    nux_f32_t fov;
    nux_f32_t near;
    nux_f32_t far;
} nux_camera_t;

typedef struct
{
    nux_v3_t  local_translation;
    nux_q4_t  local_rotation;
    nux_v3_t  local_scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

typedef struct
{
    nux_u32_t mesh;
    nux_u32_t texture;
} nux_staticmesh_t;

typedef enum
{
    NUX_TYPE_NULL       = 0,
    NUX_TYPE_ARENA      = 1,
    NUX_TYPE_LUA        = 2,
    NUX_TYPE_TEXTURE    = 3,
    NUX_TYPE_MESH       = 4,
    NUX_TYPE_SCENE      = 5,
    NUX_TYPE_NODE       = 6,
    NUX_TYPE_TRANSFORM  = 7,
    NUX_TYPE_CAMERA     = 8,
    NUX_TYPE_STATICMESH = 9,

    NUX_TYPE_MAX = 256,
} nux_type_base_t;

typedef enum
{
    NUX_COMPONENT_TRANSFORM  = 0,
    NUX_COMPONENT_CAMERA     = 1,
    NUX_COMPONENT_STATICMESH = 2,

    NUX_COMPONENT_MAX = 16,
} nux_component_type_base_t;

typedef struct
{
    nux_u32_t type;
} nux_component_type_t;

typedef struct
{
    nux_u32_t scene;
    nux_u32_t parent;
    nux_u32_t id;
    nux_u32_t components[NUX_COMPONENT_MAX];
} nux_node_t;

typedef union
{
    nux_transform_t  transform;
    nux_camera_t     camera;
    nux_staticmesh_t staticmesh;
} nux_component_t;

NUX_POOL_DEFINE(nux_node_pool, nux_node_t);
NUX_POOL_DEFINE(nux_component_pool, nux_component_t);

typedef struct
{
    nux_arena_t         *arena;
    nux_node_pool_t      nodes;
    nux_component_pool_t components;
} nux_scene_t;

typedef void (*nux_type_cleanup_t)(nux_ctx_t *ctx, void *data);
typedef nux_status_t (*nux_type_save_lua_t)(nux_ctx_t  *ctx,
                                            const void *data,
                                            lua_State  *L);
typedef nux_status_t (*nux_type_load_lua_t)(nux_ctx_t *ctx,
                                            void      *data,
                                            lua_State *L);

typedef struct
{
    const nux_c8_t     *name;
    nux_type_cleanup_t  cleanup;
    nux_type_save_lua_t save_lua;
    nux_type_load_lua_t load_lua;
    nux_u32_t           component_type;
} nux_type_t;

typedef struct
{
    nux_u32_t prev;
    nux_u32_t next;
    nux_u8_t  version;
    nux_u32_t type;
    void     *data;
} nux_object_t;

NUX_POOL_DEFINE(nux_object_pool, nux_object_t);

typedef struct
{
    nux_m4_t  view;
    nux_m4_t  proj;
    nux_v2u_t canvas_size;
    nux_v2u_t screen_size;
    nux_f32_t time;
    nux_f32_t _pad[3];
} nux_gpu_constants_buffer_t;

typedef struct
{
    nux_u32_t             slot;
    nux_gpu_buffer_type_t type;
    nux_u32_t             size;
} nux_gpu_buffer_t;

typedef struct
{
    nux_gpu_pipeline_type_t type;
    nux_u32_t               slot;
} nux_gpu_pipeline_t;

NUX_VEC_DEFINE(nux_gpu_command_vec, nux_gpu_command_t);
NUX_POOL_DEFINE(nux_arena_pool, nux_arena_t);

struct nux_context
{
    // Error handling
    nux_error_t error;
    nux_c8_t    error_message[256];

    // Arena info
    nux_arena_t *active_arena;

    void     *userdata;
    nux_b32_t running;
    nux_u64_t frame;
    nux_f32_t time;
    nux_u8_t  pal[NUX_PALETTE_SIZE];
    nux_u32_t colormap[NUX_COLORMAP_SIZE];
    nux_v2i_t cursor;
    nux_u8_t *canvas;

    nux_u32_t buttons[NUX_PLAYER_MAX];
    nux_u32_t buttons_prev[NUX_PLAYER_MAX];
    nux_f32_t axis[NUX_PLAYER_MAX * NUX_AXIS_MAX];
    nux_f32_t axis_prev[NUX_PLAYER_MAX * NUX_AXIS_MAX];

    nux_u32_t stats[NUX_STAT_MAX];

    nux_gpu_pipeline_t    main_pipeline;
    nux_gpu_pipeline_t    blit_pipeline;
    nux_gpu_command_vec_t gpu_commands;
    nux_gpu_buffer_t      vertices_buffer;
    nux_u32_t             vertices_buffer_head;
    nux_gpu_buffer_t      constants_buffer;
    nux_gpu_buffer_t      transforms_buffer;
    nux_u32_t             transforms_buffer_head;

    nux_arena_pool_t  arenas;
    nux_object_pool_t objects;
    nux_arena_t      *core_arena;

    nux_type_t types[NUX_TYPE_MAX];
    nux_u32_t  types_count;

    nux_component_type_t component_types[NUX_COMPONENT_MAX];
    nux_u32_t            component_types_count;

    nux_u32_vec_t free_texture_slots;
    nux_u32_vec_t free_buffer_slots;
    nux_u32_vec_t free_pipeline_slots;
    nux_u32_vec_t free_framebuffer_slots;
    nux_u32_vec_t free_file_slots;

    lua_State *lua_state;

    nux_callback_t init;
    nux_callback_t update;
};

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

// type.c

nux_type_t *nux_type_register(nux_ctx_t *ctx, const nux_c8_t *name);

// object.c

nux_u32_t nux_object_create(nux_ctx_t   *ctx,
                            nux_arena_t *arena,
                            nux_u32_t    type_index,
                            void        *data);
void      nux_object_delete(nux_ctx_t *ctx, nux_u32_t id);
void      nux_object_update(nux_ctx_t *ctx, nux_u32_t id, void *data);
void     *nux_object_get(nux_ctx_t *ctx, nux_u32_t type_index, nux_u32_t id);

void *nux_arena_alloc(nux_arena_t *arena, nux_u32_t size);
void  nux_arena_reset_to(nux_ctx_t *ctx, nux_arena_t *arena, nux_u32_t object);

// texture.c

void nux_texture_cleanup(nux_ctx_t *ctx, void *data);
void nux_texture_write(nux_ctx_t  *ctx,
                       nux_u32_t   id,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data);

// scene.c

void nux_component_register(nux_ctx_t *ctx, nux_u32_t type);

void  nux_scene_cleanup(nux_ctx_t *ctx, void *data);
void *nux_scene_add_component(nux_ctx_t *ctx, nux_u32_t node, nux_u32_t type);
void nux_scene_remove_component(nux_ctx_t *ctx, nux_u32_t node, nux_u32_t type);
void *nux_scene_get_component(nux_ctx_t *ctx, nux_u32_t node, nux_u32_t type);

// gltf.c

nux_u32_t nux_scene_load_gltf(nux_ctx_t *ctx, const nux_c8_t *url);

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

nux_status_t nux_graphics_init(nux_ctx_t *ctx);
nux_status_t nux_graphics_free(nux_ctx_t *ctx);
nux_status_t nux_graphics_render(nux_ctx_t *ctx);

nux_status_t nux_graphics_push_vertices(nux_ctx_t       *ctx,
                                        nux_u32_t        vcount,
                                        const nux_f32_t *data,
                                        nux_u32_t       *first);
nux_status_t nux_graphics_push_transforms(nux_ctx_t      *ctx,
                                          nux_u32_t       mcount,
                                          const nux_m4_t *data,
                                          nux_u32_t      *index);

// gpu.c

nux_status_t nux_gpu_buffer_init(nux_ctx_t *ctx, nux_gpu_buffer_t *buffer);
nux_status_t nux_gpu_texture_init(nux_ctx_t *ctx, nux_texture_t *texture);
nux_status_t nux_gpu_pipeline_init(nux_ctx_t          *ctx,
                                   nux_gpu_pipeline_t *pipeline);

void nux_gpu_draw(nux_ctx_t *ctx, nux_u32_t count);
void nux_gpu_push_u32(nux_ctx_t *ctx, nux_u32_t index, nux_u32_t value);
void nux_gpu_push_f32(nux_ctx_t *ctx, nux_u32_t index, nux_f32_t value);
void nux_gpu_bind_pipeline(nux_ctx_t *ctx, nux_u32_t slot);
void nux_gpu_bind_texture(nux_ctx_t *ctx, nux_u32_t index, nux_u32_t slot);
void nux_gpu_bind_buffer(nux_ctx_t *ctx, nux_u32_t index, nux_u32_t slot);

// io.c

nux_status_t nux_io_init(nux_ctx_t *ctx);
nux_status_t nux_io_free(nux_ctx_t *ctx);

// lua.c

nux_status_t nux_lua_load_conf(nux_ctx_t *ctx);
nux_status_t nux_lua_init(nux_ctx_t *ctx);
void         nux_lua_free(nux_ctx_t *ctx);
void         nux_lua_tick(nux_ctx_t *ctx);
nux_status_t nux_lua_register_ext(nux_ctx_t *ctx);

// lua_api.c.inc

nux_status_t nux_lua_register_base(nux_ctx_t *ctx);

// context.c

void nux_error(nux_ctx_t *ctx, nux_error_t error);

// logger.c

void nux_vlog(nux_ctx_t      *ctx,
              nux_log_level_t level,
              const nux_c8_t *fmt,
              va_list         args);
void nux_log(nux_ctx_t *ctx, nux_log_level_t level, const nux_c8_t *fmt, ...);

// input.c

void nux_input_pre_update(nux_ctx_t *ctx);

// transform.c

nux_b32_t nux_transform_update_matrix(nux_ctx_t *ctx, nux_u32_t node);

#endif
