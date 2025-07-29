#ifndef NUX_API_H
#define NUX_API_H

#include "nux_config.h"

#include <stdint.h>

typedef _Bool          nux_b32_t;
typedef uint8_t        nux_u8_t;
typedef char           nux_c8_t;
typedef int16_t        nux_i16_t;
typedef uint16_t       nux_u16_t;
typedef int32_t        nux_i32_t;
typedef uint32_t       nux_u32_t;
typedef int64_t        nux_i64_t;
typedef uint64_t       nux_u64_t;
typedef float          nux_f32_t;
typedef double         nux_f64_t;
typedef intptr_t       nux_intptr_t;
typedef struct nux_id *nux_res_t;

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
    // 16:9
    // NUX_SCREEN_WIDTH  = 1920,
    // NUX_SCREEN_HEIGHT = 1080,

    // 10:10
    // NUX_SCREEN_WIDTH  = 1024,
    // NUX_SCREEN_HEIGHT = 640,

    // 16:10, too high ?
    // NUX_CANVAS_WIDTH  = 640,
    // NUX_CANVAS_HEIGHT = 400,

    // 4:3
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 480,

    // 16:10
    NUX_CANVAS_WIDTH  = 512,
    NUX_CANVAS_HEIGHT = 320,

    // 16:10
    // NUX_CANVAS_WIDTH  = 480,
    // NUX_CANVAS_HEIGHT = 300,

    // 4:3
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 360,

    // 16:10
    // NUX_SCREEN_WIDTH  = 360,
    // NUX_SCREEN_HEIGHT = 225,

    // 4:3
    // NUX_SCREEN_WIDTH  = 320,
    // NUX_SCREEN_HEIGHT = 240,
    // NUX_CANVAS_WIDTH  = 320,
    // NUX_CANVAS_HEIGHT = 200,

    NUX_TEXTURE_WIDTH    = 4096,
    NUX_TEXTURE_HEIGHT   = 4096,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NAME_MAX         = 64,
    NUX_PALETTE_SIZE     = 256,
    NUX_COLORMAP_SIZE    = 256,
    NUX_GPU_BUFFER_SIZE  = (1 << 24), // 16M
    NUX_GPU_COMMAND_SIZE = 1024,
    NUX_DISK_MAX         = 8,
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
    NUX_PRIMITIVE_TRIANGLES = 0,
    NUX_PRIMITIVE_LINES     = 1,
    NUX_PRIMITIVE_POINTS    = 2,
} nux_primitive_t;

typedef enum
{
    NUX_VERTEX_TRIANGLES = 0,
    NUX_VERTEX_LINES     = 1,
    NUX_VERTEX_POINTS    = 2,

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
    NUX_STAT_FPS           = 0,
    NUX_STAT_SCREEN_WIDTH  = 1,
    NUX_STAT_SCREEN_HEIGHT = 2,
    NUX_STAT_MAX           = 3
} nux_stat_t;

typedef enum
{
    NUX_V3F         = 0,
    NUX_V3F_T2F     = 1,
    NUX_V3F_T2F_N3F = 2,
} nux_vertex_format_t;

typedef enum
{
    NUX_TRIANGLES = 0,
    NUX_LINES     = 1,
} nux_vertex_primitive_t;

typedef enum
{
    NUX_TEXTURE_IMAGE_RGBA    = 0,
    NUX_TEXTURE_IMAGE_INDEX   = 1,
    NUX_TEXTURE_RENDER_TARGET = 2,
} nux_texture_type_t;

typedef enum
{
    NUX_LOG_DEBUG   = 5,
    NUX_LOG_INFO    = 4,
    NUX_LOG_WARNING = 3,
    NUX_LOG_ERROR   = 2,
} nux_log_level_t;

// Debug API
void nux_trace(nux_ctx_t *ctx, const nux_c8_t *text);

// System API
nux_u32_t nux_stat(nux_ctx_t *ctx, nux_stat_t info);
nux_f32_t nux_time(nux_ctx_t *ctx);
nux_f32_t nux_dt(nux_ctx_t *ctx);
nux_u32_t nux_frame(nux_ctx_t *ctx);

// Random

nux_u32_t nux_random(nux_ctx_t *ctx);

// Draw State API

void      nux_pal(nux_ctx_t *ctx, nux_u8_t index, nux_u8_t color);
void      nux_palt(nux_ctx_t *ctx, nux_u8_t c);
void      nux_palr(nux_ctx_t *ctx);
nux_u8_t  nux_palc(nux_ctx_t *ctx, nux_u8_t index);
void      nux_cls(nux_ctx_t *ctx, nux_u32_t color);
void      nux_pset(nux_ctx_t *ctx, nux_i32_t x, nux_i32_t y, nux_u8_t c);
nux_u32_t nux_cget(nux_ctx_t *ctx, nux_u8_t index);
void      nux_cset(nux_ctx_t *ctx, nux_u8_t index, nux_u32_t c);

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

// Input

nux_u32_t nux_input_button(nux_ctx_t *ctx, nux_u32_t player);
nux_f32_t nux_input_axis(nux_ctx_t *ctx, nux_u32_t player, nux_axis_t axis);
nux_b32_t nux_button_pressed(nux_ctx_t   *ctx,
                             nux_u32_t    player,
                             nux_button_t button);
nux_b32_t nux_button_released(nux_ctx_t   *ctx,
                              nux_u32_t    player,
                              nux_button_t button);
nux_b32_t nux_button_just_pressed(nux_ctx_t   *ctx,
                                  nux_u32_t    player,
                                  nux_button_t button);
nux_b32_t nux_button_just_released(nux_ctx_t   *ctx,
                                   nux_u32_t    player,
                                   nux_button_t button);

nux_res_t nux_texture_new(nux_ctx_t         *ctx,
                          nux_texture_type_t format,
                          nux_u32_t          w,
                          nux_u32_t          h);
void      nux_texture_blit(nux_ctx_t *ctx, nux_res_t res);

nux_res_t nux_arena_new(nux_ctx_t *ctx, nux_u32_t capa);
void      nux_arena_reset(nux_ctx_t *ctx, nux_res_t res);
nux_res_t nux_arena_frame(nux_ctx_t *ctx);

nux_res_t nux_scene_new(nux_ctx_t *ctx);
void      nux_scene_render(nux_ctx_t *ctx, nux_res_t scene, nux_res_t camera);
nux_res_t nux_scene_get_node(nux_ctx_t *ctx, nux_res_t scene, nux_u32_t index);
nux_res_t nux_scene_load_gltf(nux_ctx_t *ctx, const nux_c8_t *path);

nux_res_t nux_node_new(nux_ctx_t *ctx, nux_res_t scene);
void      nux_node_set_parent(nux_ctx_t *ctx, nux_res_t res, nux_res_t parent);
nux_res_t nux_node_get_parent(nux_ctx_t *ctx, nux_res_t res);
nux_res_t nux_node_get_scene(nux_ctx_t *ctx, nux_res_t res);

void     nux_transform_add(nux_ctx_t *ctx, nux_res_t res);
void     nux_transform_remove(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_get_local_translation(nux_ctx_t *ctx, nux_res_t res);
nux_q4_t nux_transform_get_local_rotation(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_get_local_scale(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_get_translation(nux_ctx_t *ctx, nux_res_t res);
nux_q4_t nux_transform_get_rotation(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_get_scale(nux_ctx_t *ctx, nux_res_t res);
void     nux_transform_set_translation(nux_ctx_t *ctx,
                                       nux_res_t  node,
                                       nux_v3_t   position);
void     nux_transform_set_rotation(nux_ctx_t *ctx,
                                    nux_res_t  node,
                                    nux_q4_t   rotation);
void     nux_transform_set_rotation_euler(nux_ctx_t *ctx,
                                          nux_res_t  node,
                                          nux_v3_t   euler);
void     nux_transform_set_scale(nux_ctx_t *ctx, nux_res_t res, nux_v3_t scale);
nux_v3_t nux_transform_forward(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_backward(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_left(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_right(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_up(nux_ctx_t *ctx, nux_res_t res);
nux_v3_t nux_transform_down(nux_ctx_t *ctx, nux_res_t res);
void     nux_transform_rotate(nux_ctx_t *ctx,
                              nux_res_t  node,
                              nux_v3_t   axis,
                              nux_f32_t  angle);
void     nux_transform_rotate_x(nux_ctx_t *ctx, nux_res_t res, nux_f32_t angle);
void     nux_transform_rotate_y(nux_ctx_t *ctx, nux_res_t res, nux_f32_t angle);
void     nux_transform_rotate_z(nux_ctx_t *ctx, nux_res_t res, nux_f32_t angle);
void     nux_transform_look_at(nux_ctx_t *ctx, nux_res_t res, nux_v3_t center);

void nux_camera_add(nux_ctx_t *ctx, nux_res_t res);
void nux_camera_remove(nux_ctx_t *ctx, nux_res_t res);
void nux_camera_set_fov(nux_ctx_t *ctx, nux_res_t res, nux_f32_t fov);
void nux_camera_set_near(nux_ctx_t *ctx, nux_res_t res, nux_f32_t near);
void nux_camera_set_far(nux_ctx_t *ctx, nux_res_t res, nux_f32_t far);

void nux_staticmesh_add(nux_ctx_t *ctx, nux_res_t res);
void nux_staticmesh_remove(nux_ctx_t *ctx, nux_res_t res);
void nux_staticmesh_set_mesh(nux_ctx_t *ctx, nux_res_t res, nux_res_t mesh);
void nux_staticmesh_set_texture(nux_ctx_t *ctx,
                                nux_res_t  node,
                                nux_res_t  texture);
void nux_staticmesh_set_colormap(nux_ctx_t *ctx,
                                 nux_res_t  node,
                                 nux_res_t  colormap);

nux_res_t nux_mesh_new(nux_ctx_t *ctx, nux_u32_t capa);
void      nux_mesh_gen_bounds(nux_ctx_t *ctx, nux_res_t res);
nux_res_t nux_mesh_gen_cube(nux_ctx_t *ctx,
                            nux_f32_t  sx,
                            nux_f32_t  sy,
                            nux_f32_t  sz);

nux_res_t nux_canvas_new(nux_ctx_t *ctx);
void      nux_canvas_clear(nux_ctx_t *ctx, nux_res_t res);
void      nux_canvas_render(nux_ctx_t *ctx, nux_res_t res, nux_res_t target);
void      nux_canvas_text(nux_ctx_t      *ctx,
                          nux_res_t       res,
                          nux_u32_t       x,
                          nux_u32_t       y,
                          const nux_c8_t *text);
void      nux_canvas_rectangle(nux_ctx_t *ctx,
                               nux_res_t  res,
                               nux_u32_t  x,
                               nux_u32_t  y,
                               nux_u32_t  w,
                               nux_u32_t  h);
// void      nux_graphics_line(nux_ctx_t *ctx,
//                             nux_i32_t  x0,
//                             nux_i32_t  y0,
//                             nux_i32_t  x1,
//                             nux_i32_t  y1,
//                             nux_u8_t   color);
// void      nux_graphics_circle(
//          nux_ctx_t *ctx, nux_i32_t xm, nux_i32_t ym, nux_i32_t r, nux_u8_t
//          c);
// void nux_graphics_rectangle(nux_ctx_t *ctx,
//                             nux_i32_t  x0,
//                             nux_i32_t  y0,
//                             nux_i32_t  x1,
//                             nux_i32_t  y1,
//                             nux_u8_t   c);

nux_status_t nux_io_cart_begin(nux_ctx_t      *ctx,
                               const nux_c8_t *path,
                               nux_u32_t       entry_count);
nux_status_t nux_io_cart_end(nux_ctx_t *ctx);
nux_status_t nux_io_write_cart_file(nux_ctx_t *ctx, const nux_c8_t *path);

#endif
