#ifndef NUX_BASE_API_H
#define NUX_BASE_API_H

#include <stdlib/stdlib.h>

typedef nux_u32_t nux_rid_t;
typedef nux_u32_t nux_nid_t;

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
void         nux_arena_clear(nux_arena_t *arena);
nux_u32_t    nux_arena_block_count(nux_arena_t *arena);
nux_u32_t    nux_arena_memory_usage(nux_arena_t *arena);

nux_arena_t *nux_arena_core(void);
nux_arena_t *nux_arena_frame(void);

void            nux_log_set_level(nux_log_level_t level);
nux_log_level_t nux_log_level(void);

#endif
