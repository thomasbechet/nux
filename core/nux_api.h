#ifndef NUX_API_H
#define NUX_API_H

#include "nux_config.h"

#ifdef NUX_INCLUDE_FIXED_TYPES
#else
typedef unsigned char nux_u8_t;
typedef char          nux_c8_t;
typedef int           nux_i16_t;
typedef unsigned int  nux_u16_t;
typedef int           nux_i32_t;
typedef unsigned int  nux_u32_t;
typedef long          nux_i64_t;
typedef unsigned long nux_u64_t;
typedef float         nux_f32_t;
typedef double        nux_f64_t;
#endif

typedef struct nux_env *nux_env_t;

typedef enum
{
    // 16:9
    // NUX_SCREEN_WIDTH  = 1920,
    // NUX_SCREEN_HEIGHT = 1080,

    // 16:10, too high ?
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 400,

    // 4:3
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 480,

    // 16:10
    // NUX_SCREEN_WIDTH  = 512,
    // NUX_SCREEN_HEIGHT = 320,

    // 16:10
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 300,

    // 4:3
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 360,

    // 16:10
    // NUX_SCREEN_WIDTH  = 360,
    // NUX_SCREEN_HEIGHT = 225,

    // 4:3
    // NUX_SCREEN_WIDTH  = 320,
    // NUX_SCREEN_HEIGHT = 240,
    NUX_SCREEN_WIDTH  = 320,
    NUX_SCREEN_HEIGHT = 200,

    NUX_TEXTURE_MIN_SIZE = 32,
    NUX_TEXTURE_MAX_SIZE = 256,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NODE_MAX         = (1 << 16) - 1,
    NUX_NAME_MAX         = 64,
    NUX_PALETTE_LEN      = 256,
    NUX_COLORMAP_LEN     = 256,
} nux_constants_t;

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
    NUX_VERTEX_TRIANGLES = 6,
    NUX_VERTEX_LINES     = 6,
    NUX_VERTEX_POINTS    = 6,

    NUX_VERTEX_POSITION = 1 << 0,
    NUX_VERTEX_UV       = 1 << 1,
    NUX_VERTEX_COLOR    = 1 << 2,
    NUX_VERTEX_INDICES  = 1 << 3,
} nux_vertex_attribute_t;

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
    NUX_STAT_FPS = 1,
} nux_stat_t;

typedef enum
{
    NUX_RAM_SCREEN       = 0x0,
    NUX_RAM_ZBUFFER      = 0x12c00,
    NUX_RAM_COLORMAP     = 0x5dc00,
    NUX_RAM_PALETTE      = 0x5df00,
    NUX_RAM_BUTTONS      = 0x5e000,
    NUX_RAM_AXIS         = 0x5e020,
    NUX_RAM_TIME         = 0x5e0e0,
    NUX_RAM_FRAME        = 0x5e0e4,
    NUX_RAM_CURSORX      = 0x5e0e8,
    NUX_RAM_CURSORY      = 0x5e0ec,
    NUX_RAM_STAT_FPS     = 0x5e0f0,
    NUX_RAM_CAM_EYE      = 0x5e0f4,
    NUX_RAM_CAM_CENTER   = 0x5e100,
    NUX_RAM_CAM_UP       = 0x5e10c,
    NUX_RAM_CAM_FOV      = 0x5e118,
    NUX_RAM_CAM_VIEWPORT = 0x5e11c,
    NUX_RAM_MODEL        = 0x5e12c,
} nux_ram_layout_t;

// Debug API
void nux_trace(nux_env_t env, const nux_c8_t *text);
void nux_dbgi32(nux_env_t env, const nux_c8_t *name, nux_i32_t *p);
void nux_dbgf32(nux_env_t env, const nux_c8_t *name, nux_f32_t *p);

// System API
nux_u32_t nux_stat(nux_env_t env, nux_stat_t info);
nux_f32_t nux_time(nux_env_t env);
nux_f32_t nux_dt(nux_env_t env);
nux_u32_t nux_frame(nux_env_t env);

// 2D API
void nux_rectfill(nux_env_t env,
                  nux_i32_t x0,
                  nux_i32_t y0,
                  nux_i32_t x1,
                  nux_i32_t y1,
                  nux_u8_t  c);
void nux_trifill(nux_env_t env,
                 nux_i32_t x0,
                 nux_i32_t y0,
                 nux_i32_t x1,
                 nux_i32_t y1,
                 nux_i32_t x2,
                 nux_i32_t y2,
                 nux_u8_t  c);
void nux_text(
    nux_env_t env, nux_i32_t x, nux_i32_t y, const nux_c8_t *text, nux_u8_t c);
void      nux_print(nux_env_t env, const nux_c8_t *text, nux_u8_t c);
nux_i32_t nux_cursorx(nux_env_t env);
nux_i32_t nux_cursory(nux_env_t env);
void      nux_cursor(nux_env_t env, nux_i32_t x, nux_i32_t y);
void      nux_line(nux_env_t env,
                   nux_i32_t x0,
                   nux_i32_t y0,
                   nux_i32_t x1,
                   nux_i32_t y1,
                   nux_u8_t  color);
void      nux_circ(
         nux_env_t env, nux_i32_t xm, nux_i32_t ym, nux_i32_t r, nux_u8_t c);
void nux_rect(nux_env_t env,
              nux_i32_t x0,
              nux_i32_t y0,
              nux_i32_t x1,
              nux_i32_t y1,
              nux_u8_t  c);

// 3D API
void nux_cameye(nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z);
void nux_camcenter(nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z);
void nux_camup(nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z);
void nux_camviewport(
    nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h);
void nux_camfov(nux_env_t env, nux_f32_t fov);
void nux_mesht(nux_env_t        env,
               const nux_f32_t *positions,
               const nux_f32_t *uvs,
               nux_u32_t        count,
               const nux_u8_t  *tex,
               nux_u32_t        texw,
               nux_u32_t        texh,
               const nux_f32_t *m);

// Draw State API
void      nux_pal(nux_env_t env, nux_u8_t index, nux_u8_t color);
void      nux_palt(nux_env_t env, nux_u8_t c);
void      nux_palr(nux_env_t env);
nux_u8_t  nux_palc(nux_env_t env, nux_u8_t index);
void      nux_cls(nux_env_t env, nux_u32_t color);
void      nux_clsz(nux_env_t env);
void      nux_pset(nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t c);
nux_u8_t  nux_pget(nux_env_t env, nux_i32_t x, nux_i32_t y);
void      nux_zset(nux_env_t env, nux_i32_t x, nux_i32_t y, nux_f32_t depth);
nux_f32_t nux_zget(nux_env_t env, nux_i32_t x, nux_i32_t y);
nux_u32_t nux_cget(nux_env_t env, nux_u8_t index);
void      nux_cset(nux_env_t env, nux_u8_t index, nux_u32_t c);

#ifdef NUX_BUILD_VARARGS
void nux_textfmt(nux_env_t       env,
                 nux_i32_t       x,
                 nux_i32_t       y,
                 nux_u8_t        c,
                 const nux_c8_t *fmt,
                 ...);
void nux_printfmt(nux_env_t env, nux_u8_t c, const nux_c8_t *fmt, ...);
void nux_tracefmt(nux_env_t env, const nux_c8_t *fmt, ...);
#endif

// Input
nux_u32_t nux_button(nux_env_t env, nux_u32_t player);
nux_f32_t nux_axis(nux_env_t env, nux_u32_t player, nux_axis_t axis);

#endif
