#include "nux_internal.h"

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
draw_rect (nux_ctx_t      *ctx,
           nux_u32_t       transform_index,
           nux_primitive_t primitive,
           const nux_v3_t *positions)
{
    const nux_u32_t indices[]
        = { 0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7, 0, 3, 7, 7, 4, 0,
            1, 5, 6, 6, 2, 1, 0, 4, 5, 5, 1, 0, 3, 2, 6, 6, 7, 3 };

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

    const nux_u32_t vertices_count = NUX_ARRAY_SIZE(indices);
    nux_f32_t       data[vertices_count * NUX_VERTEX_SIZE];
    for (nux_u32_t i = 0; i < vertices_count; ++i)
    {
        nux_u32_t index               = indices[i];
        data[i * NUX_VERTEX_SIZE + 0] = positions[index].x;
        data[i * NUX_VERTEX_SIZE + 1] = positions[index].y;
        data[i * NUX_VERTEX_SIZE + 2] = positions[index].z;
        data[i * NUX_VERTEX_SIZE + 3] = uvs[index].x;
        data[i * NUX_VERTEX_SIZE + 4] = uvs[index].y;
    }

    nux_u32_t first;
    NUX_CHECK(
        nux_graphics_push_frame_vertices(ctx, vertices_count, data, &first),
        return);

    draw(ctx, &ctx->commands, first, vertices_count, transform_index);
}
static void
draw_box (nux_ctx_t *ctx, nux_u32_t transform_index, nux_b3_t box)
{
    const nux_v3_t positions[] = { nux_v3(box.min.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.max.z) };
    draw_rect(ctx, transform_index, positions);
}
static void
draw_line_rect (nux_ctx_t      *ctx,
                nux_u32_t       transform_index,
                const nux_v3_t *positions)
{
    const nux_u32_t indices[] = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                  6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

    const nux_u32_t vertices_count = NUX_ARRAY_SIZE(indices);
    nux_f32_t       data[vertices_count * NUX_VERTEX_SIZE];
    for (nux_u32_t i = 0; i < vertices_count; ++i)
    {
        data[i * NUX_VERTEX_SIZE + 0] = positions[indices[i]].x;
        data[i * NUX_VERTEX_SIZE + 1] = positions[indices[i]].y;
        data[i * NUX_VERTEX_SIZE + 2] = positions[indices[i]].z;
        data[i * NUX_VERTEX_SIZE + 3] = 0;
        data[i * NUX_VERTEX_SIZE + 4] = 0;
    }

    nux_u32_t first;
    NUX_CHECK(nux_graphics_push_frame_vertices(
                  ctx, NUX_ARRAY_SIZE(indices), data, &first),
              return);

    draw(ctx, &ctx->commands_lines, first, vertices_count, transform_index);
}
static void
draw_line_box (nux_ctx_t *ctx, nux_u32_t transform_index, nux_b3_t box)
{
    const nux_v3_t positions[] = { nux_v3(box.min.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.max.z) };
    draw_line_rect(ctx, transform_index, positions);
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
        NUX_CHECK(nux_graphics_push_transforms(
                      ctx, 1, &t->global_matrix, &transform_index),
                  continue);

        // Bind texture
        bind_texture(ctx, &ctx->commands, tex);

        // Create batch
        NUX_CHECK(
            draw(ctx, &ctx->commands, m->first, m->count, transform_index),
            return);

        // Create line batch
        bind_texture(ctx, &ctx->commands_lines, NUX_NULL);
        draw_line_box(ctx, transform_index, m->bounds);

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
    nux_os_gpu_submit(
        ctx->userdata, ctx->commands_lines.data, ctx->commands_lines.size);

error:
    nux_ecs_set_active(ctx, prev_ecs);
}
