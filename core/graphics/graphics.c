#include "graphics/internal.h"
#include "nux_internal.h"

#include "fonts_data.c.inc"

nux_status_t
nux_graphics_init (nux_ctx_t *ctx)
{
    nux_arena_t *a = &ctx->core_arena;

    // Initialize gpu slots
    NUX_CHECK(
        nux_u32_vec_alloc(
            ctx, a, NUX_GPU_FRAMEBUFFER_MAX, &ctx->free_framebuffer_slots),
        goto error);
    NUX_CHECK(nux_u32_vec_alloc(
                  ctx, a, NUX_GPU_PIPELINE_MAX, &ctx->free_pipeline_slots),
              goto error);
    NUX_CHECK(nux_u32_vec_alloc(
                  ctx, a, NUX_GPU_TEXTURE_MAX, &ctx->free_texture_slots),
              goto error);
    NUX_CHECK(
        nux_u32_vec_alloc(ctx, a, NUX_GPU_BUFFER_MAX, &ctx->free_buffer_slots),
        goto error);

    nux_u32_vec_fill_reversed(&ctx->free_framebuffer_slots);
    nux_u32_vec_fill_reversed(&ctx->free_pipeline_slots);
    nux_u32_vec_fill_reversed(&ctx->free_buffer_slots);
    nux_u32_vec_fill_reversed(&ctx->free_texture_slots);

    // Reserve slot 0 for main framebuffer
    nux_u32_vec_pop(&ctx->free_framebuffer_slots);

    // Create pipelines
    ctx->uber_pipeline_opaque.info.type              = NUX_GPU_PIPELINE_UBER;
    ctx->uber_pipeline_opaque.info.primitive         = NUX_PRIMITIVE_TRIANGLES;
    ctx->uber_pipeline_opaque.info.enable_blend      = NUX_FALSE;
    ctx->uber_pipeline_opaque.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &ctx->uber_pipeline_opaque),
              goto error);
    ctx->uber_pipeline_line.info.type              = NUX_GPU_PIPELINE_UBER;
    ctx->uber_pipeline_line.info.primitive         = NUX_PRIMITIVE_LINES;
    ctx->uber_pipeline_line.info.enable_blend      = NUX_FALSE;
    ctx->uber_pipeline_line.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &ctx->uber_pipeline_line), goto error);
    ctx->canvas_pipeline.info.type              = NUX_GPU_PIPELINE_CANVAS;
    ctx->uber_pipeline_opaque.info.primitive    = NUX_PRIMITIVE_TRIANGLES;
    ctx->canvas_pipeline.info.enable_blend      = NUX_TRUE;
    ctx->canvas_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &ctx->canvas_pipeline), goto error);
    ctx->blit_pipeline.info.type              = NUX_GPU_PIPELINE_BLIT;
    ctx->uber_pipeline_opaque.info.primitive  = NUX_PRIMITIVE_TRIANGLES;
    ctx->blit_pipeline.info.enable_blend      = NUX_TRUE;
    ctx->blit_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &ctx->blit_pipeline), goto error);

    // Create vertices buffers
    ctx->vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    ctx->vertices_buffer.size = NUX_VERTEX_SIZE
                                * ctx->config.graphics.vertices_buffer_size
                                * sizeof(nux_f32_t);
    ctx->vertices_buffer_head       = 0;
    ctx->vertices_buffer_head_frame = ctx->config.graphics.vertices_buffer_size;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &ctx->vertices_buffer), goto error);

    // Create default font
    NUX_CHECK(nux_font_init_default(ctx, &ctx->default_font), goto error);

    // Register lua api
    nux_lua_open_graphics(ctx);

    // Allocate gpu commands buffer
    NUX_CHECK(nux_gpu_encoder_init(
                  ctx, a, ctx->config.graphics.encoder_size, &ctx->encoder),
              return NUX_NULL);

    // Allocate constants buffer
    ctx->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    ctx->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &ctx->constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    ctx->batches_buffer_head = 0;
    ctx->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    ctx->batches_buffer.size = sizeof(nux_gpu_scene_batch_t)
                               * ctx->config.graphics.batches_buffer_size;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &ctx->batches_buffer),
              return NUX_FAILURE);

    // Allocate transforms buffer
    ctx->transforms_buffer_head = 0;
    ctx->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    ctx->transforms_buffer.size = NUX_M4_SIZE
                                  * ctx->config.graphics.transforms_buffer_size
                                  * sizeof(nux_f32_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &ctx->transforms_buffer),
              return NUX_FAILURE);

    // Create iterators
    ctx->transform_iter = nux_ecs_new_iter(ctx, ctx->core_arena_res, 1, 0);
    NUX_CHECK(ctx->transform_iter, return NUX_FAILURE);
    nux_ecs_includes(ctx, ctx->transform_iter, NUX_COMPONENT_TRANSFORM);
    ctx->transform_staticmesh_iter
        = nux_ecs_new_iter(ctx, ctx->core_arena_res, 2, 0);
    NUX_CHECK(ctx->transform_staticmesh_iter, return NUX_FAILURE);
    nux_ecs_includes(
        ctx, ctx->transform_staticmesh_iter, NUX_COMPONENT_TRANSFORM);
    nux_ecs_includes(
        ctx, ctx->transform_staticmesh_iter, NUX_COMPONENT_STATICMESH);

    // Push identity transform
    nux_m4_t identity = nux_m4_identity();
    NUX_ASSERT(nux_graphics_push_transforms(
        ctx, 1, &identity, &ctx->identity_transform_index));

    return NUX_SUCCESS;

error:
    return NUX_FAILURE;
}
nux_status_t
nux_graphics_free (nux_ctx_t *ctx)
{
    nux_gpu_buffer_free(ctx, &ctx->constants_buffer);
    nux_gpu_buffer_free(ctx, &ctx->batches_buffer);
    nux_gpu_buffer_free(ctx, &ctx->transforms_buffer);

    nux_gpu_pipeline_free(ctx, &ctx->uber_pipeline_line);
    nux_gpu_pipeline_free(ctx, &ctx->uber_pipeline_opaque);
    nux_gpu_pipeline_free(ctx, &ctx->canvas_pipeline);
    nux_gpu_pipeline_free(ctx, &ctx->blit_pipeline);

    nux_gpu_buffer_free(ctx, &ctx->vertices_buffer);

    nux_font_free(ctx, &ctx->default_font);

    NUX_ASSERT(ctx->free_texture_slots.size == NUX_GPU_TEXTURE_MAX);
    NUX_ASSERT(ctx->free_buffer_slots.size == NUX_GPU_BUFFER_MAX);
    NUX_ASSERT(ctx->free_pipeline_slots.size == NUX_GPU_PIPELINE_MAX);
    NUX_ASSERT(ctx->free_framebuffer_slots.size
               == NUX_GPU_FRAMEBUFFER_MAX - 1); // 0 reserved for default

    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_begin_render (nux_ctx_t *ctx)
{
    // Reset frame data
    ctx->transforms_buffer_head     = 0;
    ctx->batches_buffer_head        = 0;
    ctx->vertices_buffer_head_frame = ctx->config.graphics.vertices_buffer_size;
    ctx->active_texture             = NUX_NULL;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_end_render (nux_ctx_t *ctx)
{
    // Submit commands
    nux_gpu_encoder_submit(ctx, &ctx->encoder);

    return NUX_SUCCESS;
}

static nux_status_t
update_vertex_buffer (nux_ctx_t       *ctx,
                      nux_u32_t        first,
                      nux_u32_t        count,
                      const nux_f32_t *data)
{
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    ctx->vertices_buffer.slot,
                                    first * NUX_VERTEX_SIZE * sizeof(nux_f32_t),
                                    count * NUX_VERTEX_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update vertex buffer");
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_vertices (nux_ctx_t       *ctx,
                            nux_u32_t        vcount,
                            const nux_f32_t *data,
                            nux_u32_t       *first)
{
    NUX_ENSURE(ctx->vertices_buffer_head + vcount
                   < ctx->vertices_buffer_head_frame,
               return NUX_FAILURE,
               "out of vertices");
    *first = ctx->vertices_buffer_head;
    NUX_CHECK(update_vertex_buffer(ctx, *first, vcount, data),
              return NUX_FAILURE);
    ctx->vertices_buffer_head += vcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_vertices (nux_ctx_t       *ctx,
                                  nux_u32_t        vcount,
                                  const nux_f32_t *data,
                                  nux_u32_t       *first)
{
    NUX_ENSURE(ctx->vertices_buffer_head_frame - vcount
                   > ctx->vertices_buffer_head,
               return NUX_FAILURE,
               "out of frame vertices");
    *first = ctx->vertices_buffer_head_frame - vcount;
    NUX_CHECK(update_vertex_buffer(ctx, *first, vcount, data),
              return NUX_FAILURE);
    ctx->vertices_buffer_head_frame -= vcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_transforms (nux_ctx_t      *ctx,
                              nux_u32_t       mcount,
                              const nux_m4_t *data,
                              nux_u32_t      *index)
{
    NUX_ENSURE(ctx->transforms_buffer_head + mcount
                   < ctx->config.graphics.transforms_buffer_size,
               return NUX_FAILURE,
               "out of transforms");
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    ctx->transforms_buffer.slot,
                                    ctx->transforms_buffer_head * NUX_M4_SIZE
                                        * sizeof(nux_f32_t),
                                    mcount * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    *index = ctx->transforms_buffer_head;
    ctx->transforms_buffer_head += mcount;
    return NUX_SUCCESS;
}
