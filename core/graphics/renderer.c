#include "nux_internal.h"

static void
bind_texture (nux_ctx_t           *ctx,
              nux_gpu_encoder_t   *enc,
              const nux_texture_t *texture)
{
    if (ctx->active_texture != texture)
    {
        ctx->active_texture = texture;
        if (ctx->active_texture)
        {
            nux_gpu_bind_texture(
                ctx, enc, NUX_GPU_DESC_UBER_TEXTURE0, texture->gpu.slot);
        }
        else
        {
            // TODO: use dummy texture
        }
    }
}
static nux_status_t
draw (nux_ctx_t         *ctx,
      nux_gpu_encoder_t *enc,
      nux_u32_t          first,
      nux_u32_t          count,
      nux_u32_t          transform)
{
    // Create batch
    NUX_ENSURE(ctx->batches_buffer_head
                   < ctx->config.graphics.batches_buffer_size,
               return NUX_FAILURE,
               "out of batches");
    nux_u32_t batch_index = ctx->batches_buffer_head;
    ++ctx->batches_buffer_head;
    nux_gpu_scene_batch_t batch;
    batch.first_transform = transform;
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
    nux_gpu_push_u32(ctx, enc, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
    nux_gpu_draw(ctx, enc, count);

    return NUX_SUCCESS;
}
static void
draw_rect (nux_ctx_t         *ctx,
           nux_gpu_encoder_t *enc,
           nux_u32_t          transform,
           nux_primitive_t    primitive,
           const nux_v3_t    *positions)
{
    nux_u32_t vertex_count;
    nux_f32_t data[24 * NUX_VERTEX_SIZE]; // must constains all primitives type
    const nux_u32_t *indices;

    switch (primitive)
    {
        case NUX_PRIMITIVE_TRIANGLES: {
            static const nux_u32_t triangles_indices[]
                = { 0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7, 0, 3, 7, 7, 4, 0,
                    1, 5, 6, 6, 2, 1, 0, 4, 5, 5, 1, 0, 3, 2, 6, 6, 7, 3 };
            indices      = triangles_indices;
            vertex_count = NUX_ARRAY_SIZE(triangles_indices);
        }
        break;
        case NUX_PRIMITIVE_LINES: {
            static const nux_u32_t lines_indices[]
                = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                    6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
            indices      = lines_indices;
            vertex_count = NUX_ARRAY_SIZE(lines_indices);
        }
        break;
        case NUX_PRIMITIVE_POINTS:
            break;
    }

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

    for (nux_u32_t i = 0; i < vertex_count; ++i)
    {
        nux_u32_t index               = indices[i];
        data[i * NUX_VERTEX_SIZE + 0] = positions[index].x;
        data[i * NUX_VERTEX_SIZE + 1] = positions[index].y;
        data[i * NUX_VERTEX_SIZE + 2] = positions[index].z;
        data[i * NUX_VERTEX_SIZE + 3] = uvs[index].x;
        data[i * NUX_VERTEX_SIZE + 4] = uvs[index].y;
    }

    nux_u32_t first;
    NUX_CHECK(nux_graphics_push_frame_vertices(ctx, vertex_count, data, &first),
              return);

    draw(ctx, enc, first, vertex_count, transform);
}
static void
draw_box (nux_ctx_t         *ctx,
          nux_gpu_encoder_t *enc,
          nux_u32_t          transform,
          nux_primitive_t    primitive,
          nux_b3_t           box)
{
    const nux_v3_t positions[] = { nux_v3(box.min.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.max.z) };
    draw_rect(ctx, enc, transform, primitive, positions);
}
void
nux_renderer_render_ecs (nux_ctx_t *ctx, nux_res_t ecs, nux_ent_t camera)
{
    nux_res_t  prev_ecs = nux_ecs_get_active(ctx);
    nux_ecs_t *ins      = nux_res_check(ctx, NUX_RES_ECS, ecs);
    NUX_CHECK(ins, goto error);
    nux_gpu_encoder_t *enc = &ctx->encoder;

    // Propagate transforms
    nux_ent_t e = nux_ecs_begin(ctx, ctx->transform_iter);
    while (e)
    {
        nux_transform_update_matrix(ctx, e);
        e = nux_ecs_next(ctx, ctx->transform_iter);
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
    // Bind framebuffer, pipeline and constants
    nux_gpu_bind_framebuffer(ctx, enc, 0);
    nux_gpu_clear(ctx, enc, 0x4f9bd9);

    // Draw nodes
    nux_gpu_bind_pipeline(ctx, enc, ctx->uber_pipeline_opaque.slot);
    nux_gpu_bind_buffer(
        ctx, enc, NUX_GPU_DESC_UBER_CONSTANTS, ctx->constants_buffer.slot);
    nux_gpu_bind_buffer(
        ctx, enc, NUX_GPU_DESC_UBER_BATCHES, ctx->batches_buffer.slot);
    nux_gpu_bind_buffer(
        ctx, enc, NUX_GPU_DESC_UBER_TRANSFORMS, ctx->transforms_buffer.slot);
    nux_gpu_bind_buffer(
        ctx, enc, NUX_GPU_DESC_UBER_VERTICES, ctx->vertices_buffer.slot);
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
        NUX_CHECK(nux_graphics_push_transforms(
                      ctx, 1, &t->global_matrix, &sm->transform),
                  continue);

        // Draw
        bind_texture(ctx, enc, tex);
        NUX_CHECK(draw(ctx, enc, m->first, m->count, sm->transform), return);

        e = nux_ecs_next(ctx, ctx->transform_staticmesh_iter);
    }

    // Draw debug lines
    nux_gpu_bind_pipeline(ctx, enc, ctx->uber_pipeline_line.slot);
    e = nux_ecs_begin(ctx, ctx->transform_staticmesh_iter);
    while (e)
    {
        nux_staticmesh_t *sm = nux_ecs_get(ctx, e, NUX_COMPONENT_STATICMESH);
        if (!sm->mesh)
        {
            continue;
        }
        nux_mesh_t *m = nux_res_check(ctx, NUX_RES_MESH, sm->mesh);
        NUX_ASSERT(m);

        // Draw
        bind_texture(ctx, enc, NUX_NULL);
        draw_box(ctx, enc, sm->transform, NUX_PRIMITIVE_LINES, m->bounds);

        e = nux_ecs_next(ctx, ctx->transform_staticmesh_iter);
    }

error:
    nux_ecs_set_active(ctx, prev_ecs);
}
