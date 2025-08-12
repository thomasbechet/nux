#ifndef NUX_GRAPHICS_INTERNAL_H
#define NUX_GRAPHICS_INTERNAL_H

#include "base/internal.h"

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

NUX_VEC_DEFINE(nux_gpu_command_vec, nux_gpu_command_t);

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
    nux_u32_t  bounds_first;
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

typedef struct
{
    nux_gpu_command_vec_t  commands;
    nux_gpu_buffer_t       constants_buffer;
    nux_gpu_buffer_t       quads_buffer;
    nux_u32_t              quads_buffer_head;
    nux_gpu_buffer_t       batches_buffer;
    nux_u32_t              batches_buffer_head;
    nux_gpu_canvas_batch_t active_batch;
    nux_u32_t              active_texture;
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

typedef struct
{
    nux_arena_t          *arena;
    nux_gpu_buffer_t      constants_buffer;
    nux_gpu_buffer_t      batches_buffer;
    nux_u32_t             batches_buffer_head;
    nux_gpu_buffer_t      transforms_buffer;
    nux_u32_t             transforms_buffer_head;
    nux_gpu_command_vec_t commands;
    nux_gpu_command_vec_t commands_lines;
    nux_res_t             transform_iter;
    nux_res_t             transform_staticmesh_iter;
} nux_renderer_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// renderer.c

nux_status_t nux_renderer_init(nux_ctx_t *ctx);
void         nux_renderer_free(nux_ctx_t *ctx);
void nux_renderer_render_ecs(nux_ctx_t *ctx, nux_res_t ecs, nux_ent_t camera);

// font.c

nux_status_t nux_font_init_default(nux_ctx_t *ctx, nux_font_t *font);
void         nux_font_free(nux_ctx_t *ctx, nux_font_t *font);
void         nux_font_cleanup(nux_ctx_t *ctx, void *data);

// canvas.c

nux_status_t nux_canvas_init(nux_ctx_t *ctx, nux_canvas_t *canvas);
void         nux_canvas_cleanup(nux_ctx_t *ctx, void *data);

// gpu.c

nux_status_t nux_gpu_buffer_init(nux_ctx_t *ctx, nux_gpu_buffer_t *buffer);
void         nux_gpu_buffer_free(nux_ctx_t *ctx, nux_gpu_buffer_t *buffer);
nux_status_t nux_gpu_texture_init(nux_ctx_t *ctx, nux_gpu_texture_t *texture);
void         nux_gpu_texture_free(nux_ctx_t *ctx, nux_gpu_texture_t *texture);
nux_status_t nux_gpu_pipeline_init(nux_ctx_t          *ctx,
                                   nux_gpu_pipeline_t *pipeline);
void nux_gpu_pipeline_free(nux_ctx_t *ctx, nux_gpu_pipeline_t *pipeline);

void nux_gpu_bind_framebuffer(nux_ctx_t             *ctx,
                              nux_gpu_command_vec_t *cmds,
                              nux_u32_t              slot);
void nux_gpu_bind_pipeline(nux_ctx_t             *ctx,
                           nux_gpu_command_vec_t *cmds,
                           nux_u32_t              slot);
void nux_gpu_bind_texture(nux_ctx_t             *ctx,
                          nux_gpu_command_vec_t *cmds,
                          nux_u32_t              desc,
                          nux_u32_t              slot);
void nux_gpu_bind_buffer(nux_ctx_t             *ctx,
                         nux_gpu_command_vec_t *cmds,
                         nux_u32_t              desc,
                         nux_u32_t              slot);
void nux_gpu_push_u32(nux_ctx_t             *ctx,
                      nux_gpu_command_vec_t *cmds,
                      nux_u32_t              desc,
                      nux_u32_t              value);
void nux_gpu_push_f32(nux_ctx_t             *ctx,
                      nux_gpu_command_vec_t *cmds,
                      nux_u32_t              desc,
                      nux_f32_t              value);
void nux_gpu_push_v2(nux_ctx_t             *ctx,
                     nux_gpu_command_vec_t *cmds,
                     nux_u32_t              desc,
                     nux_v2_t               value);
void nux_gpu_draw(nux_ctx_t *ctx, nux_gpu_command_vec_t *cmds, nux_u32_t count);
void nux_gpu_clear(nux_ctx_t             *ctx,
                   nux_gpu_command_vec_t *cmds,
                   nux_u32_t              color);

// graphics.c

nux_status_t nux_graphics_init(nux_ctx_t *ctx);
nux_status_t nux_graphics_free(nux_ctx_t *ctx);

nux_status_t nux_graphics_push_vertices(nux_ctx_t       *ctx,
                                        nux_u32_t        vcount,
                                        const nux_f32_t *data,
                                        nux_u32_t       *first);

// lua_bindings.c

nux_status_t nux_lua_open_graphics(nux_ctx_t *ctx);

// texture.c

void nux_texture_cleanup(nux_ctx_t *ctx, void *data);
void nux_texture_write(nux_ctx_t  *ctx,
                       nux_res_t   res,
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
