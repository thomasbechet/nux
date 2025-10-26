#ifndef NUX_GRAPHICS_API_H
#define NUX_GRAPHICS_API_H

#include <base/api.h>

typedef struct nux_canvas_t   nux_canvas_t;
typedef struct nux_mesh_t     nux_mesh_t;
typedef struct nux_texture_t  nux_texture_t;
typedef struct nux_palette_t  nux_palette_t;
typedef struct nux_viewport_t nux_viewport_t;

typedef enum
{
    NUX_PALETTE_SIZE  = 256,
    NUX_COLORMAP_SIZE = 256,
    NUX_LAYER_DEFAULT = 0x1,
} nux_graphics_constants_t;

typedef enum
{
    NUX_VERTEX_TRIANGLES = 0,
    NUX_VERTEX_LINES     = 1,
    NUX_VERTEX_POINTS    = 2,
} nux_vertex_assembly_t;

typedef enum
{
    NUX_VERTEX_POSITION = 1 << 0,
    NUX_VERTEX_TEXCOORD = 1 << 1,
    NUX_VERTEX_COLOR    = 1 << 2,
} nux_vertex_attribute_t;

typedef enum
{
    NUX_TEXTURE_IMAGE_RGBA    = 0,
    NUX_TEXTURE_IMAGE_INDEX   = 1,
    NUX_TEXTURE_RENDER_TARGET = 2,
} nux_texture_type_t;

typedef enum
{
    NUX_TEXTURE_WRAP_CLAMP  = 0,
    NUX_TEXTURE_WRAP_REPEAT = 1,
    NUX_TEXTURE_WRAP_MIRROR = 2
} nux_texture_wrap_mode_t;

typedef enum
{
    NUX_COLOR_TRANSPARENT = 0,
    NUX_COLOR_WHITE       = 1,
    NUX_COLOR_RED         = 2,
    NUX_COLOR_GREEN       = 3,
    NUX_COLOR_BLUE        = 4,
    NUX_COLOR_BACKGROUND  = 5,
} nux_color_default_t;

typedef enum
{
    NUX_VIEWPORT_HIDDEN              = 0,
    NUX_VIEWPORT_FIXED               = 1,
    NUX_VIEWPORT_FIXED_BEST_FIT      = 2,
    NUX_VIEWPORT_STRETCH_KEEP_ASPECT = 3,
    NUX_VIEWPORT_STRETCH             = 4,
} nux_viewport_mode_t;

typedef enum
{
    NUX_ANCHOR_CENTER = 0,
    NUX_ANCHOR_TOP    = (1 << 1),
    NUX_ANCHOR_BOTTOM = (1 << 2),
    NUX_ANCHOR_LEFT   = (1 << 3),
    NUX_ANCHOR_RIGHT  = (1 << 4),
} nux_anchor_t;

nux_viewport_t *nux_viewport_new(nux_arena_t *arena, nux_texture_t *target);
void     nux_viewport_set_mode(nux_viewport_t *vp, nux_viewport_mode_t mode);
void     nux_viewport_set_extent(nux_viewport_t *vp, nux_v4_t extent);
void     nux_viewport_set_anchor(nux_viewport_t *vp, nux_u32_t anchor);
void     nux_viewport_set_layer(nux_viewport_t *vp, nux_i32_t layer);
void     nux_viewport_set_clear_depth(nux_viewport_t *vp, nux_b32_t clear);
void     nux_viewport_set_camera(nux_viewport_t *vp, nux_nid_t camera);
void     nux_viewport_set_texture(nux_viewport_t *vp, nux_texture_t *texture);
nux_v2_t nux_viewport_get_target_size(nux_viewport_t *vp);
nux_v4_t nux_viewport_get_global_extent(nux_viewport_t *viewport);
nux_v2_t nux_viewport_to_global(nux_viewport_t *vp, nux_v2_t coord);
nux_v2_t nux_viewport_to_local(nux_viewport_t *vp, nux_v2_t coord);

nux_texture_t *nux_texture_new(nux_arena_t       *arena,
                               nux_texture_type_t format,
                               nux_u32_t          w,
                               nux_u32_t          h);
nux_texture_t *nux_texture_load(nux_arena_t *arena, const nux_c8_t *path);
nux_texture_t *nux_texture_screen(void);
nux_v2i_t      nux_texture_get_size(nux_texture_t *texture);

nux_palette_t *nux_palette_new(nux_arena_t *arena, nux_u32_t size);
nux_palette_t *nux_palette_default(void);
void           nux_palette_set_active(nux_palette_t *palette);
void           nux_palette_set_color(nux_palette_t *palette,
                                     nux_u32_t      index,
                                     nux_v4_t       color);
nux_v4_t nux_palette_get_color(const nux_palette_t *palette, nux_u32_t index);

nux_mesh_t *nux_mesh_new(nux_arena_t           *arena,
                         nux_u32_t              capa,
                         nux_vertex_attribute_t attributes,
                         nux_vertex_assembly_t  assembly);
nux_mesh_t *nux_mesh_new_cube(nux_arena_t *arena,
                              nux_f32_t    sx,
                              nux_f32_t    sy,
                              nux_f32_t    sz);
nux_mesh_t *nux_mesh_new_plane(nux_arena_t *arena, nux_f32_t sx, nux_f32_t sy);
void        nux_mesh_update_bounds(nux_mesh_t *mesh);
nux_v3_t    nux_mesh_bounds_min(nux_mesh_t *mesh);
nux_v3_t    nux_mesh_bounds_max(nux_mesh_t *mesh);
void        nux_mesh_set_origin(nux_mesh_t *mesh, nux_v3_t origin);

nux_canvas_t  *nux_canvas_new(nux_arena_t *arena,
                              nux_u32_t    width,
                              nux_u32_t    height);
nux_texture_t *nux_canvas_get_texture(nux_canvas_t *canvas);
nux_v2i_t      nux_canvas_get_size(nux_canvas_t *canvas);
void nux_canvas_set_clear_color(nux_canvas_t *canvas, nux_u32_t color);
void nux_canvas_set_wrap_mode(nux_canvas_t           *canvas,
                              nux_texture_wrap_mode_t mode);
void nux_canvas_text(nux_canvas_t   *canvas,
                     nux_u32_t       x,
                     nux_u32_t       y,
                     const nux_c8_t *text);
void nux_canvas_rectangle(
    nux_canvas_t *canvas, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);
void nux_canvas_blit(nux_canvas_t  *canvas,
                     nux_texture_t *texture,
                     nux_b2i_t      extent,
                     nux_b2i_t      tex_extent);
void nux_canvas_blit_sliced(nux_canvas_t  *canvas,
                            nux_texture_t *texture,
                            nux_b2i_t      extent,
                            nux_b2i_t      tex_extent,
                            nux_b2i_t      inner);

void nux_graphics_begin_state(void);
void nux_graphics_end_state(void);
void nux_graphics_reset_state(void);
void nux_graphics_draw_line(nux_v3_t a, nux_v3_t b);
void nux_graphics_draw_dir(nux_v3_t origin, nux_v3_t dir, nux_f32_t length);
void nux_graphics_set_layer(nux_u32_t layer);
void nux_graphics_set_color(nux_u32_t color);
void nux_graphics_set_transform(nux_m4_t transform);
void nux_graphics_set_transform_identity(void);

void      nux_camera_set_fov(nux_nid_t e, nux_f32_t fov);
void      nux_camera_set_near(nux_nid_t e, nux_f32_t near);
void      nux_camera_set_far(nux_nid_t e, nux_f32_t far);
void      nux_camera_set_aspect(nux_nid_t e, nux_f32_t aspect);
void      nux_camera_reset_aspect(nux_nid_t e, nux_viewport_t *viewport);
void      nux_camera_set_ortho(nux_nid_t e, nux_b32_t ortho);
void      nux_camera_set_ortho_size(nux_nid_t e, nux_v2_t size);
nux_m4_t  nux_camera_get_projection(nux_nid_t e);
nux_v3_t  nux_camera_unproject(nux_nid_t e, nux_v2_t pos);
void      nux_camera_set_render_mask(nux_nid_t n, nux_u32_t mask);
nux_u32_t nux_camera_get_render_mask(nux_nid_t n);

void           nux_staticmesh_set_mesh(nux_nid_t e, nux_mesh_t *mesh);
nux_mesh_t    *nux_staticmesh_get_mesh(nux_nid_t n);
void           nux_staticmesh_set_texture(nux_nid_t e, nux_texture_t *texture);
nux_texture_t *nux_staticmesh_get_texture(nux_nid_t n);
void      nux_staticmesh_set_colormap(nux_nid_t e, nux_texture_t *colormap);
void      nux_staticmesh_set_render_layer(nux_nid_t n, nux_u32_t layer);
nux_u32_t nux_staticmesh_get_render_layer(nux_nid_t n);
void      nux_staticmesh_set_draw_bounds(nux_nid_t n, nux_b32_t draw);

#endif
