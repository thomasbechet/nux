#ifndef NUX_BASE_API_H
#define NUX_BASE_API_H

#include "nux_config.h"

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
typedef nux_u32_t nux_res_t;
typedef nux_u32_t nux_ent_t;

typedef struct nux_context nux_ctx_t;

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
} nux_q4_t;

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

nux_u32_t nux_stat(nux_ctx_t *ctx, nux_stat_t info);
nux_f32_t nux_time_elapsed(nux_ctx_t *ctx);
nux_f32_t nux_time_delta(nux_ctx_t *ctx);
nux_u32_t nux_time_frame(nux_ctx_t *ctx);
nux_u64_t nux_time_timestamp(nux_ctx_t *ctx);
// nux_u32_t nux_time_years(nux_ctx_t *ctx);
// nux_u32_t nux_time_months(nux_ctx_t *ctx);
// nux_u32_t nux_time_days(nux_ctx_t *ctx);
// nux_u32_t nux_time_hour(nux_ctx_t *ctx);
// nux_u32_t nux_time_minutes(nux_ctx_t *ctx);
// nux_u32_t nux_time_seconds(nux_ctx_t *ctx);

// Random

nux_u32_t nux_random(nux_ctx_t *ctx);

#ifdef NUX_BUILD_VARARGS
void nux_textfmt(nux_ctx_t      *ctx,
                 nux_i32_t       x,
                 nux_i32_t       y,
                 nux_u8_t        c,
                 const nux_c8_t *fmt,
                 ...);
void nux_printfmt(nux_ctx_t *ctx, nux_u8_t c, const nux_c8_t *fmt, ...);
void nux_tracefmt(nux_ctx_t *ctx, const nux_c8_t *fmt, ...);
#endif

nux_res_t nux_arena_new(nux_ctx_t *ctx, nux_res_t arena, const nux_c8_t *name);
void      nux_arena_reset(nux_ctx_t *ctx, nux_res_t arena);
nux_res_t nux_arena_core(nux_ctx_t *ctx);
nux_res_t nux_arena_frame(nux_ctx_t *ctx);

#endif
