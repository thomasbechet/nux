#ifndef NUX_BASE_API_H
#define NUX_BASE_API_H

#include <nux_config.h>

#include <stdint.h>

typedef _Bool     nux_b32_t;
typedef uint8_t   nux_u8_t;
typedef char      nux_c8_t;
typedef int16_t   nux_i16_t;
typedef uint16_t  nux_u16_t;
typedef int32_t   nux_i32_t;
typedef uint32_t  nux_u32_t;
typedef int64_t   nux_i64_t;
typedef uint64_t  nux_u64_t;
typedef float     nux_f32_t;
typedef double    nux_f64_t;
typedef intptr_t  nux_intptr_t;
typedef nux_u32_t nux_rid_t;
typedef nux_u32_t nux_nid_t;

typedef struct nux_arena_t nux_arena_t;

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

typedef enum
{
    NUX_ERROR_NONE                 = 0,
    NUX_ERROR_OUT_OF_MEMORY        = 1,
    NUX_ERROR_INVALID_TEXTURE_SIZE = 4,
    NUX_ERROR_WASM_RUNTIME         = 8,
    NUX_ERROR_CART_EOF             = 10,
    NUX_ERROR_CART_MOUNT           = 11,
} nux_error_t;

typedef enum
{
    NUX_SUCCESS = 1,
    NUX_FAILURE = 0
} nux_status_t;

typedef enum
{
    NUX_STAT_FPS           = 0,
    NUX_STAT_SCREEN_WIDTH  = 1,
    NUX_STAT_SCREEN_HEIGHT = 2,
    NUX_STAT_TIMESTAMP     = 3,
    NUX_STAT_MAX           = 4
} nux_stat_t;

typedef enum
{
    NUX_LOG_DEBUG   = 4,
    NUX_LOG_INFO    = 3,
    NUX_LOG_WARNING = 2,
    NUX_LOG_ERROR   = 1,
} nux_log_level_t;

nux_u32_t nux_stat(nux_stat_t info);
nux_f32_t nux_time_elapsed();
nux_f32_t nux_time_delta();
nux_u32_t nux_time_frame();
nux_u64_t nux_time_timestamp();

nux_u32_t nux_random();
nux_f32_t nux_random01();

nux_arena_t *nux_arena_new(nux_arena_t *arena);
void         nux_arena_reset(nux_arena_t *arena);
nux_u32_t    nux_arena_memory_usage(const nux_arena_t *arena);
nux_u32_t    nux_arena_memory_capacity(const nux_arena_t *arena);
nux_u32_t    nux_arena_block_count(const nux_arena_t *arena);
nux_arena_t *nux_arena_core(void);
nux_arena_t *nux_arena_frame(void);

void            nux_log_set_level(nux_log_level_t level);
nux_log_level_t nux_log_level(void);

nux_v4_t  nux_color_rgba(nux_u8_t r, nux_u8_t g, nux_u8_t b, nux_u8_t a);
nux_v4_t  nux_color_hex(nux_u32_t hex);
nux_u32_t nux_color_to_hex(nux_v4_t color);
nux_v4_t  nux_color_to_srgb(nux_v4_t color);

#endif
