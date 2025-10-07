#ifndef NUX_GRAPHICS_API_H
#define NUX_GRAPHICS_API_H

#include <base/api.h>

typedef struct nux_canvas_t  nux_canvas_t;
typedef struct nux_mesh_t    nux_mesh_t;
typedef struct nux_texture_t nux_texture_t;

typedef enum
{
    // 16:9
    // NUX_SCREEN_WIDTH  = 1920,
    // NUX_SCREEN_HEIGHT = 1080,

    // 10:10
    // NUX_SCREEN_WIDTH  = 1024,
    // NUX_SCREEN_HEIGHT = 640,

    // 16:10, too high ?
    NUX_CANVAS_WIDTH  = 640,
    NUX_CANVAS_HEIGHT = 400,

    // 4:3
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 480,

    // 16:10
    // NUX_CANVAS_WIDTH  = 512,
    // NUX_CANVAS_HEIGHT = 320,

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

nux_texture_t *nux_texture_new(nux_arena_t       *arena,
                               nux_texture_type_t format,
                               nux_u32_t          w,
                               nux_u32_t          h);
void           nux_texture_blit(nux_texture_t *texture);

nux_mesh_t *nux_mesh_new(nux_arena_t *arena, nux_u32_t capa);
nux_mesh_t *nux_mesh_new_cube(nux_arena_t *arena,
                              nux_f32_t    sx,
                              nux_f32_t    sy,
                              nux_f32_t    sz);
void        nux_mesh_update_bounds(nux_mesh_t *mesh);
nux_v3_t    nux_mesh_bounds_min(nux_mesh_t *mesh);
nux_v3_t    nux_mesh_bounds_max(nux_mesh_t *mesh);
void        nux_mesh_set_origin(nux_mesh_t *mesh, nux_v3_t origin);

nux_canvas_t  *nux_canvas_new(nux_arena_t *arena,
                              nux_u32_t    width,
                              nux_u32_t    height);
nux_texture_t *nux_canvas_get_texture(nux_canvas_t *canvas);
void           nux_canvas_set_layer(nux_canvas_t *canvas, nux_i32_t layer);
void nux_canvas_set_clear_color(nux_canvas_t *canvas, nux_u32_t color);
void nux_canvas_text(nux_canvas_t   *canvas,
                     nux_u32_t       x,
                     nux_u32_t       y,
                     const nux_c8_t *text);
void nux_canvas_rectangle(
    nux_canvas_t *canvas, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);
// void      nux_graphics_line(
//                             nux_i32_t  x0,
//                             nux_i32_t  y0,
//                             nux_i32_t  x1,
//                             nux_i32_t  y1,
//                             nux_u8_t   color);
// void      nux_graphics_circle(
//           nux_i32_t xm, nux_i32_t ym, nux_i32_t r, nux_u8_t
//          c);
// void nux_graphics_rectangle(
//                             nux_i32_t  x0,
//                             nux_i32_t  y0,
//                             nux_i32_t  x1,
//                             nux_i32_t  y1,
//                             nux_u8_t   c);

void nux_graphics_draw_line_tr(nux_m4_t  tr,
                               nux_v3_t  a,
                               nux_v3_t  b,
                               nux_u32_t color);
void nux_graphics_draw_line(nux_v3_t a, nux_v3_t b, nux_u32_t color);
void nux_graphics_draw_dir(nux_v3_t  origin,
                           nux_v3_t  dir,
                           nux_f32_t length,
                           nux_u32_t color);

void     nux_camera_add(nux_nid_t e);
void     nux_camera_remove(nux_nid_t e);
void     nux_camera_set_fov(nux_nid_t e, nux_f32_t fov);
void     nux_camera_set_near(nux_nid_t e, nux_f32_t near);
void     nux_camera_set_far(nux_nid_t e, nux_f32_t far);
nux_v3_t nux_camera_unproject(nux_nid_t e, nux_v2_t pos);

nux_b32_t      nux_staticmesh_has(nux_nid_t n);
void           nux_staticmesh_add(nux_nid_t e);
void           nux_staticmesh_remove(nux_nid_t e);
void           nux_staticmesh_set_mesh(nux_nid_t e, nux_mesh_t *mesh);
nux_mesh_t    *nux_staticmesh_get_mesh(nux_nid_t n);
void           nux_staticmesh_set_texture(nux_nid_t e, nux_texture_t *texture);
nux_texture_t *nux_staticmesh_get_texture(nux_nid_t n);
void nux_staticmesh_set_colormap(nux_nid_t e, nux_texture_t *colormap);

#endif
