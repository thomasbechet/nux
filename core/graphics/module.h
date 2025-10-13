#ifndef NUX_GRAPHICS_MODULE_H
#define NUX_GRAPHICS_MODULE_H

#include <scene/module.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct
{
    nux_m4_t  view;
    nux_m4_t  proj;
    nux_v2u_t screen_size;
    nux_f32_t time;
    nux_f32_t _pad[3];
} nux_gpu_constants_buffer_t;

typedef struct
{
    nux_u32_t             slot;
    nux_gpu_buffer_type_t type;
    nux_u32_t             size;
} nux_gpu_buffer_t;

typedef struct
{
    nux_gpu_pipeline_info_t info;
    nux_u32_t               slot;
} nux_gpu_pipeline_t;

typedef struct
{
    nux_u32_t          slot;
    nux_u32_t          framebuffer_slot;
    nux_texture_type_t type;
    nux_u32_t          width;
    nux_u32_t          height;
} nux_gpu_texture_t;

struct nux_texture_t
{
    nux_gpu_texture_t gpu;
    nux_u8_t         *data;
};

struct nux_palette_t
{
    nux_v4_t *colors;
    nux_u32_t size;
};

struct nux_viewport_t
{
    nux_rid_t           target;
    nux_viewport_mode_t mode;
    nux_v4_t            extent;
    struct
    {
        nux_nid_t camera;
        nux_rid_t texture;
    } source;
    nux_i32_t layer;
};

typedef struct
{
    nux_u8_t stride;
    nux_u8_t position;
    nux_u8_t texcoord;
    nux_u8_t color;
} nux_vertex_layout_t;

struct nux_mesh_t
{
    nux_f32_t             *data;
    nux_u32_t              vertex_offset;
    nux_u32_t              vertex_count; // vertex count
    nux_vertex_layout_t    vertex_layout;
    nux_vertex_attribute_t vertex_attributes;
    nux_b3_t               bounds;
};

typedef struct
{
    nux_u32_t pos;
    nux_u32_t tex;
    nux_u32_t size;
} nux_gpu_canvas_quad_t;

typedef struct
{
    nux_u32_t mode;
    nux_u32_t first;
    nux_u32_t count;
    nux_u32_t texture_width;
    nux_u32_t texture_height;
    nux_u32_t _pad0[3];
    nux_v4_t  color;
} nux_gpu_canvas_batch_t;

NUX_VEC_DEFINE(nux_gpu_command_vec, nux_gpu_command_t);

typedef struct
{
    nux_gpu_command_vec_t cmds;
} nux_gpu_encoder_t;

struct nux_canvas_t
{
    nux_gpu_encoder_t      encoder;
    nux_gpu_buffer_t       constants_buffer;
    nux_gpu_buffer_t       quads_buffer;
    nux_u32_t              quads_buffer_head;
    nux_gpu_buffer_t       batches_buffer;
    nux_u32_t              batches_buffer_head;
    nux_gpu_canvas_batch_t active_batch;
    nux_u32_t              active_texture;
    nux_texture_t         *target;
    nux_u32_t              clear_color;
    nux_canvas_t          *prev;
    nux_canvas_t          *next;
};

typedef struct
{
    nux_gpu_texture_t texture;
    nux_u32_t         glyph_width;
    nux_u32_t         glyph_height;
    const nux_u8_t   *char_to_glyph_index;
} nux_font_t;

typedef struct
{
    nux_u32_t vertex_offset;
    nux_u32_t vertex_attributes;
    nux_u32_t transform_offset;
    nux_u32_t has_texture;
    nux_v4_t  color;
} nux_gpu_scene_batch_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// graphics.c

nux_status_t nux_graphics_update_vertices(nux_u32_t        offset,
                                          nux_u32_t        count,
                                          const nux_f32_t *data);
nux_status_t nux_graphics_push_vertices(nux_u32_t        count,
                                        const nux_f32_t *data,
                                        nux_u32_t       *offset);
nux_status_t nux_graphics_push_frame_vertices(nux_u32_t        vcount,
                                              const nux_f32_t *data,
                                              nux_u32_t       *offset);
nux_status_t nux_graphics_push_transforms(nux_u32_t       mcount,
                                          const nux_m4_t *data,
                                          nux_u32_t      *offset);
nux_status_t nux_graphics_push_frame_transforms(nux_u32_t       mcount,
                                                const nux_m4_t *data,
                                                nux_u32_t      *offset);

// renderer.c

void nux_renderer_render(nux_scene_t   *scene,
                         nux_nid_t      camera,
                         nux_texture_t *target,
                         nux_v4_t       extent);

// font.c

nux_status_t nux_font_init_default(nux_font_t *font);
void         nux_font_free(nux_font_t *font);
void         nux_font_cleanup(void *data);

// canvas.c

void nux_canvas_cleanup(void *data);
void nux_canvas_render(nux_canvas_t *c);

// gpu.c

nux_status_t nux_gpu_buffer_init(nux_gpu_buffer_t *buffer);
void         nux_gpu_buffer_free(nux_gpu_buffer_t *buffer);
nux_status_t nux_gpu_texture_init(nux_gpu_texture_t *texture);
void         nux_gpu_texture_free(nux_gpu_texture_t *texture);
nux_status_t nux_gpu_pipeline_init(nux_gpu_pipeline_t *pipeline);
void         nux_gpu_pipeline_free(nux_gpu_pipeline_t *pipeline);

nux_status_t nux_gpu_encoder_init(nux_arena_t *arena, nux_gpu_encoder_t *enc);
void         nux_gpu_encoder_submit(nux_gpu_encoder_t *enc);
void         nux_gpu_bind_framebuffer(nux_gpu_encoder_t *enc, nux_u32_t slot);
void         nux_gpu_bind_pipeline(nux_gpu_encoder_t *enc, nux_u32_t slot);
void         nux_gpu_bind_texture(nux_gpu_encoder_t *enc,
                                  nux_u32_t          desc,
                                  nux_u32_t          slot);
void         nux_gpu_bind_buffer(nux_gpu_encoder_t *enc,
                                 nux_u32_t          desc,
                                 nux_u32_t          slot);
void nux_gpu_push_u32(nux_gpu_encoder_t *enc, nux_u32_t desc, nux_u32_t value);
void nux_gpu_push_f32(nux_gpu_encoder_t *enc, nux_u32_t desc, nux_f32_t value);
void nux_gpu_push_v2(nux_gpu_encoder_t *enc, nux_u32_t desc, nux_v2_t value);
void nux_gpu_draw(nux_gpu_encoder_t *enc, nux_u32_t count);
void nux_gpu_clear(nux_gpu_encoder_t *enc, nux_u32_t color);
void nux_gpu_viewport(nux_gpu_encoder_t *enc, nux_v4_t viewport);

// texture.c

void nux_texture_cleanup(void *data);
void nux_texture_write(nux_texture_t *tex,
                       nux_u32_t      x,
                       nux_u32_t      y,
                       nux_u32_t      w,
                       nux_u32_t      h,
                       const void    *data);
void nux_texture_blit(nux_texture_t *tex,
                      nux_texture_t *target,
                      nux_v4_t       extent);

// staticmesh.c

nux_status_t nux_staticmesh_write(nux_serde_writer_t *s, const void *data);
nux_status_t nux_staticmesh_read(nux_serde_reader_t *s, void *data);

// camera.c

nux_status_t nux_camera_write(nux_serde_writer_t *s, const void *data);
nux_status_t nux_camera_read(nux_serde_reader_t *s, void *data);

#endif
