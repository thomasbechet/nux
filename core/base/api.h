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
        nux_f32_t z;
    };
    nux_f32_t data[3];
} nux_v3_t;

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
} nux_m4_t;

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

typedef enum
{
    NUX_CONTROLLER_MAX = 4,
    NUX_BUTTON_MAX     = 10,
    NUX_AXIS_MAX       = 6,
    NUX_NAME_MAX       = 64,
    NUX_DISK_MAX       = 8,
} nux_io_constants_t;

typedef enum
{
    NUX_BUTTON_A     = 1 << 0,
    NUX_BUTTON_X     = 1 << 1,
    NUX_BUTTON_Y     = 1 << 2,
    NUX_BUTTON_B     = 1 << 3,
    NUX_BUTTON_UP    = 1 << 4,
    NUX_BUTTON_DOWN  = 1 << 5,
    NUX_BUTTON_LEFT  = 1 << 6,
    NUX_BUTTON_RIGHT = 1 << 7,
    NUX_BUTTON_LB    = 1 << 8,
    NUX_BUTTON_RB    = 1 << 9,
} nux_button_t;

typedef enum
{
    NUX_AXIS_LEFTX  = 0,
    NUX_AXIS_LEFTY  = 1,
    NUX_AXIS_RIGHTX = 2,
    NUX_AXIS_RIGHTY = 3,
    NUX_AXIS_RT     = 4,
    NUX_AXIS_LT     = 5,
} nux_axis_t;

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

#ifdef NUX_BUILD_VARARGS
void nux_textfmt(
    nux_i32_t x, nux_i32_t y, nux_u8_t c, const nux_c8_t *fmt, ...);
void nux_printfmt(nux_u8_t c, const nux_c8_t *fmt, ...);
void nux_tracefmt(const nux_c8_t *fmt, ...);
#endif

nux_arena_t *nux_arena_new(nux_arena_t *arena);
void         nux_arena_reset(nux_arena_t *arena);
nux_u32_t    nux_arena_memory_usage(const nux_arena_t *arena);
nux_u32_t    nux_arena_memory_capacity(const nux_arena_t *arena);
nux_u32_t    nux_arena_block_count(const nux_arena_t *arena);
nux_arena_t *nux_arena_core(void);
nux_arena_t *nux_arena_frame(void);
nux_arena_t *nux_arena_scratch(void);

const nux_c8_t *nux_resource_path(nux_rid_t rid);
void            nux_resource_set_name(nux_rid_t rid, const nux_c8_t *name);
const nux_c8_t *nux_resource_name(nux_rid_t rid);
nux_arena_t    *nux_resource_arena(nux_rid_t rid);
nux_rid_t       nux_resource_find(const nux_c8_t *name);

void nux_log_set_level(nux_log_level_t level);

nux_u32_t nux_button_state(nux_u32_t controller);
nux_b32_t nux_button_pressed(nux_u32_t controller, nux_button_t button);
nux_b32_t nux_button_released(nux_u32_t controller, nux_button_t button);
nux_b32_t nux_button_just_pressed(nux_u32_t controller, nux_button_t button);
nux_b32_t nux_button_just_released(nux_u32_t controller, nux_button_t button);
nux_f32_t nux_axis_value(nux_u32_t controller, nux_axis_t axis);
nux_f32_t nux_cursor_x(nux_u32_t controller);
nux_f32_t nux_cursor_y(nux_u32_t controller);
void      nux_cursor_set(nux_u32_t controller, nux_f32_t x, nux_f32_t y);

nux_status_t nux_io_cart_begin(const nux_c8_t *path, nux_u32_t entry_count);
nux_status_t nux_io_cart_end(void);
nux_status_t nux_io_write_cart_file(const nux_c8_t *path);

#endif
