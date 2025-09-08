#ifndef NUX_GRAPHICS_INTERNAL_H
#define NUX_GRAPHICS_INTERNAL_H

#include <ecs/internal.h>
#include <io/internal.h>

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

typedef struct
{
    nux_gpu_texture_t gpu;
    nux_u8_t         *data;
} nux_texture_t;

typedef struct
{
    nux_f32_t *data;
    nux_u32_t  first;
    nux_u32_t  count; // vertex count
    nux_b3_t   bounds;
} nux_mesh_t;

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

typedef struct nux_canvas
{
    nux_gpu_encoder_t      encoder;
    nux_gpu_buffer_t       constants_buffer;
    nux_gpu_buffer_t       quads_buffer;
    nux_u32_t              quads_buffer_head;
    nux_gpu_buffer_t       batches_buffer;
    nux_u32_t              batches_buffer_head;
    nux_gpu_canvas_batch_t active_batch;
    nux_u32_t              active_texture;
    nux_rid_t              target;
    nux_i32_t              layer;
    nux_u32_t              clear_color;
    struct nux_canvas     *prev;
    struct nux_canvas     *next;
} nux_canvas_t;

typedef struct
{
    nux_gpu_texture_t texture;
    nux_u32_t         glyph_width;
    nux_u32_t         glyph_height;
    const nux_u8_t   *char_to_glyph_index;
} nux_font_t;

typedef struct
{
    nux_u32_t first_vertex;
    nux_u32_t first_transform;
    nux_u32_t has_texture;
} nux_gpu_scene_batch_t;

typedef enum
{
    NUX_VERTEX_SIZE = 5
} nux_graphics_constants_internal_t;

typedef struct nux_graphics_module
{
    nux_gpu_pipeline_t uber_pipeline_opaque;
    nux_gpu_pipeline_t uber_pipeline_line;
    nux_gpu_pipeline_t blit_pipeline;
    nux_gpu_pipeline_t canvas_pipeline;
    nux_gpu_buffer_t   vertices_buffer;
    nux_u32_t          vertices_buffer_head;
    nux_u32_t          vertices_buffer_head_frame;
    nux_gpu_buffer_t   transforms_buffer;
    nux_u32_t          transforms_buffer_head;
    nux_u32_t          transforms_buffer_head_frame;
    nux_font_t         default_font;
    nux_u32_vec_t      free_texture_slots;
    nux_u32_vec_t      free_buffer_slots;
    nux_u32_vec_t      free_pipeline_slots;
    nux_u32_vec_t      free_framebuffer_slots;
    nux_u32_t          identity_transform_index;

    nux_gpu_encoder_t encoder;
    nux_gpu_encoder_t immediate_encoder;
    nux_gpu_buffer_t  constants_buffer;
    nux_gpu_buffer_t  batches_buffer;
    nux_u32_t         batches_buffer_head;
    nux_rid_t         transform_iter;
    nux_rid_t         transform_staticmesh_iter;
    nux_rid_t         transform_camera_iter;

    const nux_texture_t *active_texture;
} nux_graphics_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// graphics.c

nux_status_t nux_graphics_init(nux_ctx_t *ctx);
nux_status_t nux_graphics_free(nux_ctx_t *ctx);
nux_status_t nux_graphics_pre_update(nux_ctx_t *ctx);
nux_status_t nux_graphics_update(nux_ctx_t *ctx);

nux_status_t nux_graphics_push_vertices(nux_ctx_t       *ctx,
                                        nux_u32_t        vcount,
                                        const nux_f32_t *data,
                                        nux_u32_t       *first);
nux_status_t nux_graphics_push_frame_vertices(nux_ctx_t       *ctx,
                                              nux_u32_t        vcount,
                                              const nux_f32_t *data,
                                              nux_u32_t       *first);
nux_status_t nux_graphics_push_transforms(nux_ctx_t      *ctx,
                                          nux_u32_t       mcount,
                                          const nux_m4_t *data,
                                          nux_u32_t      *index);
nux_status_t nux_graphics_push_frame_transforms(nux_ctx_t      *ctx,
                                                nux_u32_t       mcount,
                                                const nux_m4_t *data,
                                                nux_u32_t      *index);

// renderer.c

void nux_renderer_render(nux_ctx_t *ctx, nux_ecs_t *ecs);
void nux_graphics_draw_line(nux_ctx_t *ctx,
                            nux_v3_t   a,
                            nux_v3_t   b,
                            nux_u32_t  color);

// font.c

nux_status_t nux_font_init_default(nux_ctx_t *ctx, nux_font_t *font);
void         nux_font_free(nux_ctx_t *ctx, nux_font_t *font);
void         nux_font_cleanup(nux_ctx_t *ctx, nux_rid_t rid);

// canvas.c

void nux_canvas_cleanup(nux_ctx_t *ctx, nux_rid_t rid);
void nux_canvas_render(nux_ctx_t *ctx, nux_canvas_t *c);

// gpu.c

nux_status_t nux_gpu_buffer_init(nux_ctx_t *ctx, nux_gpu_buffer_t *buffer);
void         nux_gpu_buffer_free(nux_ctx_t *ctx, nux_gpu_buffer_t *buffer);
nux_status_t nux_gpu_texture_init(nux_ctx_t *ctx, nux_gpu_texture_t *texture);
void         nux_gpu_texture_free(nux_ctx_t *ctx, nux_gpu_texture_t *texture);
nux_status_t nux_gpu_pipeline_init(nux_ctx_t          *ctx,
                                   nux_gpu_pipeline_t *pipeline);
void nux_gpu_pipeline_free(nux_ctx_t *ctx, nux_gpu_pipeline_t *pipeline);

nux_status_t nux_gpu_encoder_init(nux_arena_t *arena, nux_gpu_encoder_t *enc);
void         nux_gpu_encoder_submit(nux_ctx_t *ctx, nux_gpu_encoder_t *enc);
void         nux_gpu_bind_framebuffer(nux_ctx_t         *ctx,
                                      nux_gpu_encoder_t *enc,
                                      nux_u32_t          slot);
void         nux_gpu_bind_pipeline(nux_ctx_t         *ctx,
                                   nux_gpu_encoder_t *enc,
                                   nux_u32_t          slot);
void         nux_gpu_bind_texture(nux_ctx_t         *ctx,
                                  nux_gpu_encoder_t *enc,
                                  nux_u32_t          desc,
                                  nux_u32_t          slot);
void         nux_gpu_bind_buffer(nux_ctx_t         *ctx,
                                 nux_gpu_encoder_t *enc,
                                 nux_u32_t          desc,
                                 nux_u32_t          slot);
void         nux_gpu_push_u32(nux_ctx_t         *ctx,
                              nux_gpu_encoder_t *enc,
                              nux_u32_t          desc,
                              nux_u32_t          value);
void         nux_gpu_push_f32(nux_ctx_t         *ctx,
                              nux_gpu_encoder_t *enc,
                              nux_u32_t          desc,
                              nux_f32_t          value);
void         nux_gpu_push_v2(nux_ctx_t         *ctx,
                             nux_gpu_encoder_t *enc,
                             nux_u32_t          desc,
                             nux_v2_t           value);
void nux_gpu_draw(nux_ctx_t *ctx, nux_gpu_encoder_t *enc, nux_u32_t count);
void nux_gpu_clear(nux_ctx_t *ctx, nux_gpu_encoder_t *enc, nux_u32_t color);

// lua_bindings.c

nux_status_t nux_lua_open_graphics(nux_ctx_t *ctx);

// texture.c

void nux_texture_cleanup(nux_ctx_t *ctx, nux_rid_t rid);
void nux_texture_write(nux_ctx_t  *ctx,
                       nux_rid_t   rid,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data);

// camera.c

nux_m4_t nux_perspective(nux_f32_t fov,
                         nux_f32_t aspect_ratio,
                         nux_f32_t z_near,
                         nux_f32_t z_far);
nux_m4_t nux_ortho(nux_f32_t left,
                   nux_f32_t right,
                   nux_f32_t bottom,
                   nux_f32_t top,
                   nux_f32_t near,
                   nux_f32_t far);
nux_m4_t nux_lookat(nux_v3_t eye, nux_v3_t center, nux_v3_t up);

#endif
