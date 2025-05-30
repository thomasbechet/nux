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

    // 10:10
    // NUX_SCREEN_WIDTH  = 1024,
    // NUX_SCREEN_HEIGHT = 640,

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
    NUX_CANVAS_WIDTH  = 480,
    NUX_CANVAS_HEIGHT = 300,

    // 4:3
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 360,

    // 16:10
    // NUX_SCREEN_WIDTH  = 360,
    // NUX_SCREEN_HEIGHT = 225,

    // 4:3
    // NUX_SCREEN_WIDTH  = 320,
    // NUX_SCREEN_HEIGHT = 240,
    // NUX_SCREEN_WIDTH  = 320,
    // NUX_SCREEN_HEIGHT = 200,

    NUX_TEXTURE_WIDTH    = 4096,
    NUX_TEXTURE_HEIGHT   = 4096,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NAME_MAX         = 64,
    NUX_PALETTE_SIZE     = 256,
    NUX_COLORMAP_SIZE    = 256,
    NUX_MEMORY_SIZE      = (1 << 27), // 128M
    NUX_GPU_BUFFER_SIZE  = (1 << 24), // 16M
    NUX_GPU_COMMAND_SIZE = 1024,
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
    NUX_RAM_CANVAS       = 0x0,
    NUX_RAM_COLORMAP     = 0x3d090,
    NUX_RAM_PALETTE      = 0x3d490,
    NUX_RAM_TEXTURE      = 0x3d590,
    NUX_RAM_BUTTONS      = 0x103d590,
    NUX_RAM_AXIS         = 0x103d5b0,
    NUX_RAM_TIME         = 0x103d670,
    NUX_RAM_FRAME        = 0x103d674,
    NUX_RAM_CURSORX      = 0x103d678,
    NUX_RAM_CURSORY      = 0x103d67c,
    NUX_RAM_STAT_FPS     = 0x103d680,
    NUX_RAM_TEXTURE_VIEW = 0x103d684,
    NUX_RAM_CAM_EYE      = 0x103d694,
    NUX_RAM_CAM_CENTER   = 0x103d6a0,
    NUX_RAM_CAM_UP       = 0x103d6ac,
    NUX_RAM_CAM_FOV      = 0x103d6b8,
    NUX_RAM_CAM_VIEWPORT = 0x103d6bc,
    NUX_RAM_MODEL        = 0x103d6cc,
    NUX_RAM_USER         = 0x103d70c
} nux_ram_layout_t;

typedef enum
{
    NUX_V3F,
    NUX_V3F_T2F,
    NUX_V3F_T2F_N3F,
} nux_vertex_format_t;

typedef enum
{
    NUX_TRIANGLES,
    NUX_LINES,
} nux_vertex_primitive_t;

typedef enum
{
    NUX_TEXTURE_COLOR,
    NUX_TEXTURE_ALPHA,
    NUX_TEXTURE_NORMAL,
} nux_texture_type_t;

// Debug API
void nux_trace(nux_env_t env, const nux_c8_t *text);
void nux_dbgi32(nux_env_t env, const nux_c8_t *name, nux_i32_t *p);
void nux_dbgf32(nux_env_t env, const nux_c8_t *name, nux_f32_t *p);

// System API
nux_u32_t nux_stat(nux_env_t env, nux_stat_t info);
nux_u32_t nux_tricount(nux_env_t env);
nux_f32_t nux_time(nux_env_t env);
nux_f32_t nux_dt(nux_env_t env);
nux_u32_t nux_frame(nux_env_t env);

// Canvas API
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
void nux_mesh(nux_env_t        env,
              const nux_f32_t *positions,
              const nux_f32_t *uvs,
              nux_u32_t        count,
              const nux_f32_t *m);
void nux_mesh_wire(nux_env_t        env,
                   const nux_f32_t *positions,
                   const nux_f32_t *uvs,
                   nux_u32_t        count,
                   const nux_f32_t *m);
void nux_draw_cube(nux_env_t        env,
                   nux_f32_t        sx,
                   nux_f32_t        sy,
                   nux_f32_t        sz,
                   const nux_f32_t *m);
void nux_draw_plane(nux_env_t        env,
                    nux_f32_t        w,
                    nux_f32_t        h,
                    const nux_f32_t *m);

// Draw State API
void      nux_pal(nux_env_t env, nux_u8_t index, nux_u8_t color);
void      nux_palt(nux_env_t env, nux_u8_t c);
void      nux_palr(nux_env_t env);
nux_u8_t  nux_palc(nux_env_t env, nux_u8_t index);
void      nux_cls(nux_env_t env, nux_u32_t color);
void      nux_pset(nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t c);
nux_u32_t nux_cget(nux_env_t env, nux_u8_t index);
void      nux_cset(nux_env_t env, nux_u8_t index, nux_u16_t c);

nux_u8_t *nux_screen(nux_env_t env);
void      nux_bind_texture(nux_env_t          env,
                           nux_u32_t          x,
                           nux_u32_t          y,
                           nux_u32_t          w,
                           nux_u32_t          h,
                           nux_texture_type_t type);
void      nux_write_texture(nux_env_t       env,
                            nux_u32_t       x,
                            nux_u32_t       y,
                            nux_u32_t       w,
                            nux_u32_t       h,
                            const nux_u8_t *data);
void      nux_copy_texture(nux_env_t       env,
                           nux_u32_t       srcx,
                           nux_u32_t       srcy,
                           nux_u32_t       dstx,
                           nux_u32_t       dsty,
                           nux_u32_t       w,
                           nux_u32_t       h,
                           const nux_u8_t *data);

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
