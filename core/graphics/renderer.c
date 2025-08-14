#include "nux_internal.h"

#define TRANSFORMS_DEFAULT_SIZE 4096 * 2
#define BATCH_DEFAULT_SIZE      4096 * 2

static nux_status_t
push_transforms (nux_ctx_t      *ctx,
                 nux_u32_t       mcount,
                 const nux_m4_t *data,
                 nux_u32_t      *index)
{
    nux_renderer_t *r = &ctx->renderer;
    NUX_ENSURE(r->transforms_buffer_head + mcount < TRANSFORMS_DEFAULT_SIZE,
               return NUX_FAILURE,
               "out of transforms");
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    r->transforms_buffer.slot,
                                    r->transforms_buffer_head * NUX_M4_SIZE
                                        * sizeof(nux_f32_t),
                                    mcount * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    *index = r->transforms_buffer_head;
    r->transforms_buffer_head += mcount;
    return NUX_SUCCESS;
}

nux_status_t
nux_renderer_init (nux_ctx_t *ctx)
{
    nux_renderer_t *r = &ctx->renderer;
    nux_arena_t    *a = &ctx->core_arena;

    // Allocate gpu commands buffer
    NUX_CHECK(nux_gpu_command_vec_alloc(ctx, a, 4096, &r->commands),
              return NUX_NULL);
    NUX_CHECK(nux_gpu_command_vec_alloc(ctx, a, 4096, &r->commands_lines),
              return NUX_NULL);

    // Allocate constants buffer
    r->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    r->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &r->constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    r->batches_buffer_head = 0;
    r->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    r->batches_buffer.size = sizeof(nux_gpu_scene_batch_t) * BATCH_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &r->batches_buffer), return NUX_FAILURE);

    // Allocate transforms buffer
    r->transforms_buffer_head = 0;
    r->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    r->transforms_buffer.size
        = NUX_M4_SIZE * TRANSFORMS_DEFAULT_SIZE * sizeof(nux_f32_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &r->transforms_buffer),
              return NUX_FAILURE);

    // Create iterators
    r->transform_iter = nux_ecs_new_iter(ctx, ctx->core_arena_res, 1, 0);
    NUX_CHECK(r->transform_iter, return NUX_FAILURE);
    nux_ecs_includes(ctx, r->transform_iter, NUX_COMPONENT_TRANSFORM);
    r->transform_staticmesh_iter
        = nux_ecs_new_iter(ctx, ctx->core_arena_res, 2, 0);
    NUX_CHECK(r->transform_staticmesh_iter, return NUX_FAILURE);
    nux_ecs_includes(
        ctx, r->transform_staticmesh_iter, NUX_COMPONENT_TRANSFORM);
    nux_ecs_includes(
        ctx, r->transform_staticmesh_iter, NUX_COMPONENT_STATICMESH);

    return NUX_SUCCESS;
}
void
nux_renderer_free (nux_ctx_t *ctx)
{
    nux_renderer_t *r = &ctx->renderer;
    nux_gpu_buffer_free(ctx, &r->constants_buffer);
    nux_gpu_buffer_free(ctx, &r->batches_buffer);
    nux_gpu_buffer_free(ctx, &r->transforms_buffer);
}
void
nux_renderer_render_ecs (nux_ctx_t *ctx, nux_res_t ecs, nux_ent_t camera)
{
    nux_res_t       prev_ecs = nux_ecs_get_active(ctx);
    nux_renderer_t *r        = &ctx->renderer;
    nux_ecs_t      *ins      = nux_res_check(ctx, NUX_RES_ECS, ecs);
    NUX_CHECK(ins, goto error);

    // Reset frame data
    r->transforms_buffer_head = 0;
    r->batches_buffer_head    = 0;
    nux_gpu_command_vec_clear(&r->commands);
    nux_gpu_command_vec_clear(&r->commands_lines);

    // Propagate transforms
    nux_ent_t e = nux_ecs_begin(ctx, r->transform_iter);
    while (e)
    {
        nux_transform_update_matrix(ctx, e);
        e = nux_ecs_next(ctx, r->transform_iter);
    }

    // Bind framebuffer, pipeline and constants
    nux_gpu_bind_framebuffer(ctx, &r->commands, 0);
    nux_gpu_clear(ctx, &r->commands, 0x4f9bd9);

    // Begin opaque pass
    nux_gpu_bind_pipeline(ctx, &r->commands, ctx->uber_pipeline_opaque.slot);
    nux_gpu_bind_buffer(ctx,
                        &r->commands,
                        NUX_GPU_DESC_UBER_CONSTANTS,
                        r->constants_buffer.slot);
    nux_gpu_bind_buffer(
        ctx, &r->commands, NUX_GPU_DESC_UBER_BATCHES, r->batches_buffer.slot);
    nux_gpu_bind_buffer(ctx,
                        &r->commands,
                        NUX_GPU_DESC_UBER_TRANSFORMS,
                        r->transforms_buffer.slot);
    nux_gpu_bind_buffer(ctx,
                        &r->commands,
                        NUX_GPU_DESC_UBER_VERTICES,
                        ctx->vertices_buffer.slot);

    // Begin lines pass
    nux_gpu_bind_pipeline(
        ctx, &r->commands_lines, ctx->uber_pipeline_line.slot);

    // Draw nodes
    e = nux_ecs_begin(ctx, r->transform_staticmesh_iter);
    while (e)
    {
        nux_staticmesh_t *sm = nux_ecs_get(ctx, e, NUX_COMPONENT_STATICMESH);
        if (!sm->mesh)
        {
            continue;
        }
        nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
        nux_mesh_t      *m = nux_res_check(ctx, NUX_RES_MESH, sm->mesh);
        NUX_ASSERT(m);
        nux_texture_t *tex = NUX_NULL;
        if (sm->texture)
        {
            tex = nux_res_check(ctx, NUX_RES_TEXTURE, sm->texture);
        }

        // Push transform
        nux_u32_t transform_index;
        NUX_CHECK(push_transforms(ctx, 1, &t->global_matrix, &transform_index),
                  continue);

        // Create batch
        NUX_ENSURE(r->batches_buffer_head < BATCH_DEFAULT_SIZE,
                   continue,
                   "out of batches");
        nux_u32_t batch_index = r->batches_buffer_head;
        ++r->batches_buffer_head;
        nux_gpu_scene_batch_t batch;
        batch.first_transform = transform_index;
        batch.first_vertex    = m->first;
        batch.has_texture     = tex ? 1 : 0;
        NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                        r->batches_buffer.slot,
                                        batch_index * sizeof(batch),
                                        sizeof(batch),
                                        &batch),
                   continue,
                   "failed to update batches buffer");

        // Create commands
        if (tex)
        {
            nux_gpu_bind_texture(
                ctx, &r->commands, NUX_GPU_DESC_UBER_TEXTURE0, tex->gpu.slot);
        }
        nux_gpu_push_u32(
            ctx, &r->commands, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
        nux_gpu_draw(ctx, &r->commands, m->count);

        // Create line batch
        if (m->bounds_first)
        {
            batch_index = r->batches_buffer_head;
            ++r->batches_buffer_head;
            batch.first_transform = transform_index;
            batch.first_vertex    = m->bounds_first;
            batch.has_texture     = 0;
            NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                            r->batches_buffer.slot,
                                            batch_index * sizeof(batch),
                                            sizeof(batch),
                                            &batch),
                       continue,
                       "failed to update batches buffer");
            nux_gpu_push_u32(ctx,
                             &r->commands_lines,
                             NUX_GPU_DESC_UBER_BATCH_INDEX,
                             batch_index);
            nux_gpu_draw(ctx, &r->commands_lines, 12 * 2);
        }

        e = nux_ecs_next(ctx, r->transform_staticmesh_iter);
    }

    // Update constants
    nux_transform_t *ct = nux_ecs_get(ctx, camera, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(ct, goto error);
    nux_camera_t *cc = nux_ecs_get(ctx, camera, NUX_COMPONENT_CAMERA);
    NUX_CHECK(cc, goto error);

    nux_v3_t eye    = nux_m4_mulv3(ct->global_matrix, NUX_V3_ZEROS, 1);
    nux_v3_t center = nux_m4_mulv3(ct->global_matrix, NUX_V3_FORWARD, 1);
    nux_v3_t up     = nux_m4_mulv3(ct->global_matrix, NUX_V3_UP, 0);

    nux_gpu_constants_buffer_t constants;
    constants.view = nux_lookat(eye, center, up);
    constants.proj
        = nux_perspective(nux_radian(cc->fov),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          cc->near,
                          cc->far);
    constants.screen_size = nux_v2u(ctx->stats[NUX_STAT_SCREEN_WIDTH],
                                    ctx->stats[NUX_STAT_SCREEN_HEIGHT]);
    constants.time        = ctx->time_elapsed;
    nux_os_buffer_update(ctx->userdata,
                         r->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit commands
    nux_os_gpu_submit(ctx->userdata, r->commands.data, r->commands.size);
    nux_os_gpu_submit(
        ctx->userdata, r->commands_lines.data, r->commands_lines.size);

error:
    nux_ecs_set_active(ctx, prev_ecs);
}
