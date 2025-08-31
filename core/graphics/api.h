#ifndef NUX_GRAPHICS_API_H
#define NUX_GRAPHICS_API_H

#include "base/api.h"

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

    NUX_PALETTE_SIZE  = 256,
    NUX_COLORMAP_SIZE = 256,
} nux_graphics_constants_t;

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
    NUX_VERTEX_V3F         = 0,
    NUX_VERTEX_V3F_T2F     = 1,
    NUX_VERTEX_V3F_T2F_N3F = 2,
} nux_vertex_format_t;

typedef enum
{
    NUX_TEXTURE_IMAGE_RGBA    = 0,
    NUX_TEXTURE_IMAGE_INDEX   = 1,
    NUX_TEXTURE_RENDER_TARGET = 2,
} nux_texture_type_t;

nux_rid_t nux_texture_new(nux_ctx_t         *ctx,
                          nux_rid_t          arena,
                          nux_texture_type_t format,
                          nux_u32_t          w,
                          nux_u32_t          h);
void      nux_texture_blit(nux_ctx_t *ctx, nux_rid_t rid);

nux_rid_t nux_mesh_new(nux_ctx_t *ctx, nux_rid_t arena, nux_u32_t capa);
nux_rid_t nux_mesh_new_cube(
    nux_ctx_t *ctx, nux_rid_t arena, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz);
void     nux_mesh_update_bounds(nux_ctx_t *ctx, nux_rid_t mesh);
nux_v3_t nux_mesh_bounds_min(nux_ctx_t *ctx, nux_rid_t mesh);
nux_v3_t nux_mesh_bounds_max(nux_ctx_t *ctx, nux_rid_t mesh);

nux_rid_t nux_canvas_new(nux_ctx_t *ctx,
                         nux_rid_t  arena,
                         nux_u32_t  width,
                         nux_u32_t  height,
                         nux_u32_t  capa);
nux_rid_t nux_canvas_get_texture(nux_ctx_t *ctx, nux_rid_t rid);
void      nux_canvas_set_layer(nux_ctx_t *ctx, nux_rid_t rid, nux_i32_t layer);
void      nux_canvas_text(nux_ctx_t      *ctx,
                          nux_rid_t       rid,
                          nux_u32_t       x,
                          nux_u32_t       y,
                          const nux_c8_t *text);
void      nux_canvas_rectangle(nux_ctx_t *ctx,
                               nux_rid_t  rid,
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

#endif
