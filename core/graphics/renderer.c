#include "nux_internal.h"

static nux_status_t
push_transforms (nux_ctx_t      *ctx,
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
static void
bind_texture (nux_ctx_t             *ctx,
              nux_gpu_command_vec_t *cmds,
              const nux_texture_t   *texture)
{
    if (ctx->active_texture != texture)
    {
        ctx->active_texture = texture;
        if (ctx->active_texture)
        {
            nux_gpu_bind_texture(
                ctx, cmds, NUX_GPU_DESC_UBER_TEXTURE0, texture->gpu.slot);
        }
        else
        {
            // TODO: use dummy texture
        }
    }
}
static nux_status_t
draw (nux_ctx_t             *ctx,
      nux_gpu_command_vec_t *cmds,
      nux_u32_t              first,
      nux_u32_t              count,
      nux_u32_t              transform_index)
{
    // Create batch
    NUX_ENSURE(ctx->batches_buffer_head
                   < ctx->config.graphics.batches_buffer_size,
               return NUX_FAILURE,
               "out of batches");
    nux_u32_t batch_index = ctx->batches_buffer_head;
    ++ctx->batches_buffer_head;
    nux_gpu_scene_batch_t batch;
    batch.first_transform = transform_index;
    batch.first_vertex    = first;
    batch.has_texture     = ctx->active_texture ? 1 : 0;
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    ctx->batches_buffer.slot,
                                    batch_index * sizeof(batch),
                                    sizeof(batch),
                                    &batch),
               return NUX_FAILURE,
               "failed to update batches buffer");

    // Add commands
    nux_gpu_push_u32(ctx, cmds, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
    nux_gpu_draw(ctx, cmds, count);

    return NUX_SUCCESS;
}
static void
draw_box (nux_ctx_t *ctx, nux_u32_t transform_index, nux_b3_t box)
{
    const nux_v3_t v0 = nux_v3(box.min.x, box.min.y, box.min.z);
    const nux_v3_t v1 = nux_v3(box.max.x, box.min.y, box.min.z);
    const nux_v3_t v2 = nux_v3(box.max.x, box.min.y, box.max.z);
    const nux_v3_t v3 = nux_v3(box.min.x, box.min.y, box.max.z);

    const nux_v3_t v4 = nux_v3(box.min.x, box.max.y, box.min.z);
    const nux_v3_t v5 = nux_v3(box.max.x, box.max.y, box.min.z);
    const nux_v3_t v6 = nux_v3(box.max.x, box.max.y, box.max.z);
    const nux_v3_t v7 = nux_v3(box.min.x, box.max.y, box.max.z);

    const nux_v3_t positions[]
        = { v0, v1, v2, v2, v3, v0, v4, v6, v5, v6, v4, v7,
            v0, v3, v7, v7, v4, v0, v1, v5, v6, v6, v2, v1,
            v0, v4, v5, v5, v1, v0, v3, v2, v6, v6, v7, v3 };

    const nux_v2_t uvs[] = {
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } }, { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 0, 0 } }, { { 0, 1 } }, { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 1, 0 } },
        { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } },
        { { 0, 1 } }, { { 1, 1 } }, { { 1, 1 } }, { { 1, 0 } }, { { 0, 0 } },
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } },
    };

    const nux_u32_t vertices_count = NUX_ARRAY_SIZE(positions);
    nux_f32_t       data[vertices_count * 5];
    for (nux_u32_t i = 0; i < vertices_count; ++i)
    {
        data[i * 5 + 0] = positions[i].x;
        data[i * 5 + 1] = positions[i].y;
        data[i * 5 + 2] = positions[i].z;
        data[i * 5 + 3] = uvs[i].x;
        data[i * 5 + 4] = uvs[i].y;
    }

    nux_u32_t first;
    NUX_CHECK(
        nux_graphics_push_frame_vertices(ctx, vertices_count, data, &first),
        return);

    draw(ctx, &ctx->commands_lines, first, vertices_count * 2, transform_index);
}
void
nux_renderer_render_ecs (nux_ctx_t *ctx, nux_res_t ecs, nux_ent_t camera)
{
    nux_res_t  prev_ecs = nux_ecs_get_active(ctx);
    nux_ecs_t *ins      = nux_res_check(ctx, NUX_RES_ECS, ecs);
    NUX_CHECK(ins, goto error);

    // Reset frame data
    ctx->transforms_buffer_head = 0;
    ctx->batches_buffer_head    = 0;
    nux_gpu_command_vec_clear(&ctx->commands);
    nux_gpu_command_vec_clear(&ctx->commands_lines);
    ctx->active_texture = NUX_NULL;

    // Propagate transforms
    nux_ent_t e = nux_ecs_begin(ctx, ctx->transform_iter);
    while (e)
    {
        nux_transform_update_matrix(ctx, e);
        e = nux_ecs_next(ctx, ctx->transform_iter);
    }

    // Bind framebuffer, pipeline and constants
    nux_gpu_bind_framebuffer(ctx, &ctx->commands, 0);
    nux_gpu_clear(ctx, &ctx->commands, 0x4f9bd9);

    // Begin opaque pass
    nux_gpu_bind_pipeline(ctx, &ctx->commands, ctx->uber_pipeline_opaque.slot);
    nux_gpu_bind_buffer(ctx,
                        &ctx->commands,
                        NUX_GPU_DESC_UBER_CONSTANTS,
                        ctx->constants_buffer.slot);
    nux_gpu_bind_buffer(ctx,
                        &ctx->commands,
                        NUX_GPU_DESC_UBER_BATCHES,
                        ctx->batches_buffer.slot);
    nux_gpu_bind_buffer(ctx,
                        &ctx->commands,
                        NUX_GPU_DESC_UBER_TRANSFORMS,
                        ctx->transforms_buffer.slot);
    nux_gpu_bind_buffer(ctx,
                        &ctx->commands,
                        NUX_GPU_DESC_UBER_VERTICES,
                        ctx->vertices_buffer.slot);

    // Begin lines pass
    nux_gpu_bind_pipeline(
        ctx, &ctx->commands_lines, ctx->uber_pipeline_line.slot);

    // Draw nodes
    e = nux_ecs_begin(ctx, ctx->transform_staticmesh_iter);
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

        // Bind texture
        bind_texture(ctx, &ctx->commands, tex);

        // Create batch
        NUX_CHECK(
            draw(ctx, &ctx->commands, m->first, m->count, transform_index),
            return);

        // Create line batch
        if (m->bounds_first)
        {
            // batch_index = ctx->batches_buffer_head;
            // ++ctx->batches_buffer_head;
            // batch.first_transform = transform_index;
            // batch.first_vertex    = m->bounds_first;
            // batch.has_texture     = 0;
            // NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
            //                                 ctx->batches_buffer.slot,
            //                                 batch_index * sizeof(batch),
            //                                 sizeof(batch),
            //                                 &batch),
            //            continue,
            //            "failed to update batches buffer");
            // nux_gpu_push_u32(ctx,
            //                  &ctx->commands_lines,
            //                  NUX_GPU_DESC_UBER_BATCH_INDEX,
            //                  batch_index);
            // nux_gpu_draw(ctx, &ctx->commands_lines, 12 * 2);
        }
        draw_box(ctx, transform_index, nux_b3(nux_v3s(0), nux_v3s(10)));

        e = nux_ecs_next(ctx, ctx->transform_staticmesh_iter);
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
                         ctx->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit commands
    nux_os_gpu_submit(ctx->userdata, ctx->commands.data, ctx->commands.size);
    // nux_os_gpu_submit(
    //     ctx->userdata, ctx->commands_lines.data, ctx->commands_lines.size);

error:
    nux_ecs_set_active(ctx, prev_ecs);
}
