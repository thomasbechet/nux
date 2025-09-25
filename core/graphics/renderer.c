#include "internal.h"

static void
bind_texture (nux_gpu_encoder_t *enc, const nux_texture_t *texture)
{
    nux_graphics_module_t *module = nux_graphics_module();
    if (module->active_texture != texture)
    {
        module->active_texture = texture;
        if (module->active_texture)
        {
            nux_gpu_bind_texture(
                enc, NUX_GPU_DESC_UBER_TEXTURE0, texture->gpu.slot);
        }
        else
        {
            // TODO: use dummy texture
        }
    }
}
static nux_status_t
draw (nux_gpu_encoder_t *enc,
      nux_u32_t          first,
      nux_u32_t          count,
      nux_u32_t          transform)
{
    nux_graphics_module_t *module = nux_graphics_module();

    // Create batch
    NUX_ENSURE(module->batches_buffer_head
                   < nux_config()->graphics.batches_buffer_size,
               return NUX_FAILURE,
               "out of batches");
    nux_u32_t batch_index = module->batches_buffer_head;
    ++module->batches_buffer_head;
    nux_gpu_scene_batch_t batch;
    batch.first_transform = transform;
    batch.first_vertex    = first;
    batch.has_texture     = module->active_texture ? 1 : 0;
    NUX_ENSURE(nux_os_buffer_update(nux_userdata(),
                                    module->batches_buffer.slot,
                                    batch_index * sizeof(batch),
                                    sizeof(batch),
                                    &batch),
               return NUX_FAILURE,
               "failed to update batches buffer");

    // Add commands
    nux_gpu_push_u32(enc, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
    nux_gpu_draw(enc, count);

    return NUX_SUCCESS;
}
static void
draw_rect (nux_gpu_encoder_t *enc,
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
    NUX_CHECK(nux_graphics_push_frame_vertices(vertex_count, data, &first),
              return);

    draw(enc, first, vertex_count, transform);
}
static void
draw_box (nux_gpu_encoder_t *enc,
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
    draw_rect(enc, transform, primitive, positions);
}
static void
draw_line (nux_gpu_encoder_t *enc, nux_u32_t transform, nux_v3_t a, nux_v3_t b)
{
    const nux_v3_t  positions[]  = { a, b };
    const nux_u32_t vertex_count = NUX_ARRAY_SIZE(positions);
    nux_f32_t       data[NUX_VERTEX_SIZE * vertex_count];
    for (nux_u32_t i = 0; i < vertex_count; ++i)
    {
        data[i * NUX_VERTEX_SIZE + 0] = positions[i].x;
        data[i * NUX_VERTEX_SIZE + 1] = positions[i].y;
        data[i * NUX_VERTEX_SIZE + 2] = positions[i].z;
        data[i * NUX_VERTEX_SIZE + 3] = 0;
        data[i * NUX_VERTEX_SIZE + 4] = 0;
    }
    nux_u32_t first;
    NUX_CHECK(nux_graphics_push_frame_vertices(vertex_count, data, &first),
              return);
    draw(enc, first, vertex_count, transform);
}
void
nux_renderer_render (nux_ecs_t *ecs)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_gpu_encoder_t     *enc    = &module->encoder;

    // Propagate transforms
    nux_eid_t it = NUX_NULL;
    while ((it = nux_ecs_next(module->transform_iter, it)))
    {
        nux_transform_update_matrix(it);
    }

    // Find current camera
    nux_eid_t camera = NUX_NULL;
    it               = NUX_NULL;
    while ((it = nux_ecs_next(module->transform_camera_iter, it)))
    {
        nux_camera_t *c = nux_ecs_get(it, NUX_COMPONENT_CAMERA);
        // TODO: check current / active
        camera = it;
        break;
    }

    // Render scene
    if (camera)
    {
        // Update constants
        nux_transform_t *ct = nux_ecs_get(camera, NUX_COMPONENT_TRANSFORM);
        nux_camera_t    *cc = nux_ecs_get(camera, NUX_COMPONENT_CAMERA);

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
        constants.screen_size = nux_v2u(nux_stat(NUX_STAT_SCREEN_WIDTH),
                                        nux_stat(NUX_STAT_SCREEN_HEIGHT));
        constants.time        = nux_time_elapsed();
        nux_os_buffer_update(nux_userdata(),
                             module->constants_buffer.slot,
                             0,
                             sizeof(constants),
                             &constants);

        // Bind framebuffer, pipeline and constants
        nux_gpu_bind_framebuffer(enc, 0);
        nux_gpu_clear(enc, 0x4f9bd9);

        // Render nodes
        nux_gpu_bind_pipeline(enc, module->uber_pipeline_opaque.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_CONSTANTS, module->constants_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_BATCHES, module->batches_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_TRANSFORMS, module->transforms_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_VERTICES, module->vertices_buffer.slot);
        it = NUX_NULL;
        while ((it = nux_ecs_next(module->transform_staticmesh_iter, it)))
        {
            nux_staticmesh_t *sm = nux_ecs_get(it, NUX_COMPONENT_STATICMESH);
            if (!sm->mesh)
            {
                continue;
            }
            nux_transform_t *t = nux_ecs_get(it, NUX_COMPONENT_TRANSFORM);
            nux_mesh_t *m = nux_resource_check(NUX_RESOURCE_MESH, sm->mesh);
            NUX_ASSERT(m);
            nux_texture_t *tex = NUX_NULL;
            if (sm->texture)
            {
                tex = nux_resource_check(NUX_RESOURCE_TEXTURE, sm->texture);
            }

            // Push transform
            NUX_CHECK(nux_graphics_push_transforms(
                          1, &t->global_matrix, &sm->transform),
                      continue);

            // Draw
            bind_texture(enc, tex);
            NUX_CHECK(draw(enc, m->first, m->count, sm->transform), return);
        }

        // Draw debug lines
        nux_gpu_bind_pipeline(enc, module->uber_pipeline_line.slot);
        it = NUX_NULL;
        while ((it = nux_ecs_next(module->transform_staticmesh_iter, it)))
        {
            nux_staticmesh_t *sm = nux_ecs_get(it, NUX_COMPONENT_STATICMESH);
            if (!sm->mesh)
            {
                continue;
            }
            nux_mesh_t *m = nux_resource_check(NUX_RESOURCE_MESH, sm->mesh);
            NUX_ASSERT(m);

            // Draw
            bind_texture(enc, NUX_NULL);
            draw_box(enc, sm->transform, NUX_PRIMITIVE_LINES, m->bounds);
        }

        // Submit commands
        nux_gpu_encoder_submit(&module->encoder);

        // Submit immediate commands
        nux_gpu_encoder_submit(&module->immediate_encoder);
    }
}
void
nux_renderer_draw_rect (const nux_v3_t *positions)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_gpu_encoder_t     *enc    = &module->immediate_encoder;

    nux_gpu_bind_framebuffer(enc, 0);
    nux_gpu_bind_pipeline(enc, module->uber_pipeline_line.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_CONSTANTS, module->constants_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_BATCHES, module->batches_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_TRANSFORMS, module->transforms_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_VERTICES, module->vertices_buffer.slot);
    draw_rect(
        enc, module->identity_transform_index, NUX_PRIMITIVE_LINES, positions);
}
void
draw_line_tr (nux_u32_t transform_index,
              nux_v3_t  a,
              nux_v3_t  b,
              nux_u32_t color)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_gpu_encoder_t     *enc    = &module->immediate_encoder;

    nux_gpu_bind_framebuffer(enc, 0);
    nux_gpu_bind_pipeline(enc, module->uber_pipeline_line.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_CONSTANTS, module->constants_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_BATCHES, module->batches_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_TRANSFORMS, module->transforms_buffer.slot);
    nux_gpu_bind_buffer(
        enc, NUX_GPU_DESC_UBER_VERTICES, module->vertices_buffer.slot);
    draw_line(enc, transform_index, a, b);
}
void
nux_graphics_draw_line_tr (nux_m4_t tr, nux_v3_t a, nux_v3_t b, nux_u32_t color)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_u32_t              transform_index;
    NUX_CHECK(nux_graphics_push_frame_transforms(1, &tr, &transform_index),
              return);
    draw_line_tr(transform_index, a, b, color);
}
void
nux_graphics_draw_line (nux_v3_t a, nux_v3_t b, nux_u32_t color)
{
    nux_graphics_module_t *module = nux_graphics_module();
    draw_line_tr(module->identity_transform_index, a, b, color);
}
void
nux_graphics_draw_dir (nux_v3_t  origin,
                       nux_v3_t  dir,
                       nux_f32_t length,
                       nux_u32_t color)
{
    nux_v3_t a = origin;
    nux_v3_t b = nux_v3_add(origin, nux_v3_muls(dir, length));
    nux_graphics_draw_line(a, b, color);
}
