#ifndef NUX_IO_API_H
#define NUX_IO_API_H

#include "base/api.h"

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

void nux_log_set_level(nux_ctx_t *ctx, nux_log_level_t level);

nux_u32_t nux_button_state(nux_ctx_t *ctx, nux_u32_t controller);
nux_b32_t nux_button_pressed(nux_ctx_t   *ctx,
                             nux_u32_t    controller,
                             nux_button_t button);
nux_b32_t nux_button_released(nux_ctx_t   *ctx,
                              nux_u32_t    controller,
                              nux_button_t button);
nux_b32_t nux_button_just_pressed(nux_ctx_t   *ctx,
                                  nux_u32_t    controller,
                                  nux_button_t button);
nux_b32_t nux_button_just_released(nux_ctx_t   *ctx,
                                   nux_u32_t    controller,
                                   nux_button_t button);
nux_f32_t nux_axis(nux_ctx_t *ctx, nux_u32_t controller, nux_axis_t axis);
nux_f32_t nux_cursor_x(nux_ctx_t *ctx, nux_u32_t controller);
nux_f32_t nux_cursor_y(nux_ctx_t *ctx, nux_u32_t controller);
void      nux_cursor_set(nux_ctx_t *ctx,
                         nux_u32_t  controller,
                         nux_f32_t  x,
                         nux_f32_t  y);

nux_status_t nux_io_cart_begin(nux_ctx_t      *ctx,
                               const nux_c8_t *path,
                               nux_u32_t       entry_count);
nux_status_t nux_io_cart_end(nux_ctx_t *ctx);
nux_status_t nux_io_write_cart_file(nux_ctx_t *ctx, const nux_c8_t *path);

#endif
