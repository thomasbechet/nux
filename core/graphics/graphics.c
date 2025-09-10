#include "internal.h"

#include "fonts_data.c.inc"

NUX_VEC_IMPL(nux_gpu_command_vec, nux_gpu_command_t)

nux_status_t
nux_graphics_init (nux_ctx_t *ctx)
{
    ctx->graphics = nux_arena_alloc(&ctx->core_arena, sizeof(*ctx->graphics));
    NUX_CHECK(ctx->graphics, return NUX_FAILURE);

    nux_graphics_module_t *module = ctx->graphics;
    nux_arena_t           *a      = &ctx->core_arena;

    // Register resources
    nux_resource_type_t *type;
    type = nux_resource_register(
        ctx, NUX_RESOURCE_TEXTURE, sizeof(nux_texture_t), "texture");
    type->cleanup = nux_texture_cleanup;
    type          = nux_resource_register(
        ctx, NUX_RESOURCE_MESH, sizeof(nux_mesh_t), "mesh");
    type = nux_resource_register(
        ctx, NUX_RESOURCE_CANVAS, sizeof(nux_canvas_t), "canvas");
    type->cleanup = nux_canvas_cleanup;
    type          = nux_resource_register(
        ctx, NUX_RESOURCE_FONT, sizeof(nux_font_t), "font");
    type->cleanup = nux_font_cleanup;

    // Register components
    nux_ecs_register_component(
        ctx, NUX_COMPONENT_CAMERA, "camera", sizeof(nux_camera_t));
    nux_ecs_register_component(
        ctx, NUX_COMPONENT_STATICMESH, "staticmesh", sizeof(nux_staticmesh_t));

    // Initialize gpu slots
    NUX_CHECK(nux_u32_vec_init_capa(
                  a, NUX_GPU_FRAMEBUFFER_MAX, &module->free_framebuffer_slots),
              goto error);
    NUX_CHECK(nux_u32_vec_init_capa(
                  a, NUX_GPU_PIPELINE_MAX, &module->free_pipeline_slots),
              goto error);
    NUX_CHECK(nux_u32_vec_init_capa(
                  a, NUX_GPU_TEXTURE_MAX, &module->free_texture_slots),
              goto error);
    NUX_CHECK(nux_u32_vec_init_capa(
                  a, NUX_GPU_BUFFER_MAX, &module->free_buffer_slots),
              goto error);

    nux_u32_vec_fill_reversed(&module->free_framebuffer_slots);
    nux_u32_vec_fill_reversed(&module->free_pipeline_slots);
    nux_u32_vec_fill_reversed(&module->free_buffer_slots);
    nux_u32_vec_fill_reversed(&module->free_texture_slots);

    // Reserve slot 0 for main framebuffer
    nux_u32_vec_pop(&module->free_framebuffer_slots);

    // Create pipelines
    module->uber_pipeline_opaque.info.type         = NUX_GPU_PIPELINE_UBER;
    module->uber_pipeline_opaque.info.primitive    = NUX_PRIMITIVE_TRIANGLES;
    module->uber_pipeline_opaque.info.enable_blend = NUX_FALSE;
    module->uber_pipeline_opaque.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &module->uber_pipeline_opaque),
              goto error);
    module->uber_pipeline_line.info.type              = NUX_GPU_PIPELINE_UBER;
    module->uber_pipeline_line.info.primitive         = NUX_PRIMITIVE_LINES;
    module->uber_pipeline_line.info.enable_blend      = NUX_FALSE;
    module->uber_pipeline_line.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &module->uber_pipeline_line),
              goto error);
    module->canvas_pipeline.info.type              = NUX_GPU_PIPELINE_CANVAS;
    module->uber_pipeline_opaque.info.primitive    = NUX_PRIMITIVE_TRIANGLES;
    module->canvas_pipeline.info.enable_blend      = NUX_TRUE;
    module->canvas_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &module->canvas_pipeline), goto error);
    module->blit_pipeline.info.type              = NUX_GPU_PIPELINE_BLIT;
    module->uber_pipeline_opaque.info.primitive  = NUX_PRIMITIVE_TRIANGLES;
    module->blit_pipeline.info.enable_blend      = NUX_TRUE;
    module->blit_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(ctx, &module->blit_pipeline), goto error);

    // Create vertices buffers
    module->vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->vertices_buffer.size = NUX_VERTEX_SIZE
                                   * ctx->config.graphics.vertices_buffer_size
                                   * sizeof(nux_f32_t);
    module->vertices_buffer_head = 0;
    module->vertices_buffer_head_frame
        = ctx->config.graphics.vertices_buffer_size;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &module->vertices_buffer), goto error);

    // Create default font
    NUX_CHECK(nux_font_init_default(ctx, &module->default_font), goto error);

    // Register lua api
    nux_lua_open_graphics(ctx);

    // Allocate gpu commands buffer
    NUX_CHECK(nux_gpu_encoder_init(a, &module->encoder), return NUX_NULL);
    NUX_CHECK(nux_gpu_encoder_init(a, &module->immediate_encoder),
              return NUX_NULL);

    // Allocate constants buffer
    module->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    module->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &module->constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    module->batches_buffer_head = 0;
    module->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->batches_buffer.size = sizeof(nux_gpu_scene_batch_t)
                                  * ctx->config.graphics.batches_buffer_size;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &module->batches_buffer),
              return NUX_FAILURE);

    // Allocate transforms buffer
    module->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->transforms_buffer.size
        = NUX_M4_SIZE * ctx->config.graphics.transforms_buffer_size
          * sizeof(nux_f32_t);
    module->transforms_buffer_head = 0;
    module->transforms_buffer_head_frame
        = ctx->config.graphics.transforms_buffer_size;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &module->transforms_buffer),
              return NUX_FAILURE);

    // Create iterators
    module->transform_iter = nux_ecs_new_iter(ctx, ctx->core_arena_rid, 1, 0);
    NUX_CHECK(module->transform_iter, return NUX_FAILURE);
    nux_ecs_includes(ctx, module->transform_iter, NUX_COMPONENT_TRANSFORM);

    module->transform_camera_iter
        = nux_ecs_new_iter(ctx, ctx->core_arena_rid, 2, 0);
    NUX_CHECK(module->transform_camera_iter, return NUX_FAILURE);
    nux_ecs_includes(
        ctx, module->transform_camera_iter, NUX_COMPONENT_TRANSFORM);
    nux_ecs_includes(ctx, module->transform_camera_iter, NUX_COMPONENT_CAMERA);

    module->transform_staticmesh_iter
        = nux_ecs_new_iter(ctx, ctx->core_arena_rid, 2, 0);
    NUX_CHECK(module->transform_staticmesh_iter, return NUX_FAILURE);
    nux_ecs_includes(
        ctx, module->transform_staticmesh_iter, NUX_COMPONENT_TRANSFORM);
    nux_ecs_includes(
        ctx, module->transform_staticmesh_iter, NUX_COMPONENT_STATICMESH);

    // Push identity transform
    nux_m4_t identity = nux_m4_identity();
    NUX_ASSERT(nux_graphics_push_transforms(
        ctx, 1, &identity, &module->identity_transform_index));
    NUX_ASSERT(module->identity_transform_index == 0);

    return NUX_SUCCESS;

error:
    return NUX_FAILURE;
}
nux_status_t
nux_graphics_free (nux_ctx_t *ctx)
{
    nux_graphics_module_t *module = ctx->graphics;

    nux_gpu_buffer_free(ctx, &module->constants_buffer);
    nux_gpu_buffer_free(ctx, &module->batches_buffer);
    nux_gpu_buffer_free(ctx, &module->transforms_buffer);

    nux_gpu_pipeline_free(ctx, &module->uber_pipeline_line);
    nux_gpu_pipeline_free(ctx, &module->uber_pipeline_opaque);
    nux_gpu_pipeline_free(ctx, &module->canvas_pipeline);
    nux_gpu_pipeline_free(ctx, &module->blit_pipeline);

    nux_gpu_buffer_free(ctx, &module->vertices_buffer);

    nux_font_free(ctx, &module->default_font);

    NUX_ASSERT(module->free_texture_slots.size == NUX_GPU_TEXTURE_MAX);
    NUX_ASSERT(module->free_buffer_slots.size == NUX_GPU_BUFFER_MAX);
    NUX_ASSERT(module->free_pipeline_slots.size == NUX_GPU_PIPELINE_MAX);
    NUX_ASSERT(module->free_framebuffer_slots.size
               == NUX_GPU_FRAMEBUFFER_MAX - 1); // 0 reserved for default

    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_pre_update (nux_ctx_t *ctx)
{
    nux_graphics_module_t *module = ctx->graphics;

    // Reset frame data
    module->transforms_buffer_head = 1; // keep identity transform
    module->batches_buffer_head    = 0;
    module->vertices_buffer_head_frame
        = ctx->config.graphics.vertices_buffer_size;
    module->transforms_buffer_head_frame
        = ctx->config.graphics.transforms_buffer_size;
    module->active_texture = NUX_NULL;

    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_update (nux_ctx_t *ctx)
{
    // Submit canvas commands
    nux_rid_t canvas = NUX_NULL;
    while ((canvas = nux_resource_next(ctx, NUX_RESOURCE_CANVAS, canvas)))
    {
        nux_canvas_t *c = nux_resource_check(ctx, NUX_RESOURCE_CANVAS, canvas);
        nux_canvas_render(ctx, c);
    }

    // Render ECS
    nux_renderer_render(ctx, ctx->ecs->active);

    // Blit canvas layers
    canvas = NUX_NULL;
    while ((canvas = nux_resource_next(ctx, NUX_RESOURCE_CANVAS, canvas)))
    {
        nux_canvas_t *c = nux_resource_check(ctx, NUX_RESOURCE_CANVAS, canvas);
        if (c->target && c->layer >= 0)
        {
            nux_texture_blit(ctx, c->target);
        }
    }

    return NUX_SUCCESS;
}

static nux_status_t
update_vertex_buffer (nux_ctx_t       *ctx,
                      nux_u32_t        first,
                      nux_u32_t        count,
                      const nux_f32_t *data)
{
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    ctx->graphics->vertices_buffer.slot,
                                    first * NUX_VERTEX_SIZE * sizeof(nux_f32_t),
                                    count * NUX_VERTEX_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update vertex buffer");
    return NUX_SUCCESS;
}
static nux_status_t
update_transform_buffer (nux_ctx_t      *ctx,
                         nux_u32_t       first,
                         nux_u32_t       count,
                         const nux_m4_t *data)
{
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    ctx->graphics->transforms_buffer.slot,
                                    first * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    count * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_vertices (nux_ctx_t       *ctx,
                            nux_u32_t        vcount,
                            const nux_f32_t *data,
                            nux_u32_t       *first)
{
    nux_graphics_module_t *module = ctx->graphics;
    NUX_ENSURE(module->vertices_buffer_head + vcount
                   < module->vertices_buffer_head_frame,
               return NUX_FAILURE,
               "out of vertices");
    *first = module->vertices_buffer_head;
    NUX_CHECK(update_vertex_buffer(ctx, *first, vcount, data),
              return NUX_FAILURE);
    module->vertices_buffer_head += vcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_vertices (nux_ctx_t       *ctx,
                                  nux_u32_t        vcount,
                                  const nux_f32_t *data,
                                  nux_u32_t       *first)
{
    nux_graphics_module_t *module = ctx->graphics;
    NUX_ENSURE(module->vertices_buffer_head_frame - vcount
                   > module->vertices_buffer_head,
               return NUX_FAILURE,
               "out of frame vertices");
    *first = module->vertices_buffer_head_frame - vcount;
    NUX_CHECK(update_vertex_buffer(ctx, *first, vcount, data),
              return NUX_FAILURE);
    module->vertices_buffer_head_frame -= vcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_transforms (nux_ctx_t      *ctx,
                              nux_u32_t       mcount,
                              const nux_m4_t *data,
                              nux_u32_t      *index)
{
    nux_graphics_module_t *module = ctx->graphics;
    NUX_ENSURE(module->transforms_buffer_head + mcount
                   < module->transforms_buffer_head_frame,
               return NUX_FAILURE,
               "out of transforms");
    *index = module->transforms_buffer_head;
    NUX_CHECK(update_transform_buffer(ctx, *index, mcount, data),
              return NUX_FAILURE);
    module->transforms_buffer_head += mcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_transforms (nux_ctx_t      *ctx,
                                    nux_u32_t       mcount,
                                    const nux_m4_t *data,
                                    nux_u32_t      *index)
{
    nux_graphics_module_t *module = ctx->graphics;
    NUX_ENSURE(module->transforms_buffer_head_frame - mcount
                   > module->transforms_buffer_head,
               return NUX_FAILURE,
               "out of frame transforms");
    *index = module->transforms_buffer_head_frame - mcount;
    NUX_CHECK(update_transform_buffer(ctx, *index, mcount, data),
              return NUX_FAILURE);
    module->transforms_buffer_head_frame -= mcount;
    return NUX_SUCCESS;
}
