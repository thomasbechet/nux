#include "internal.h"

static void
bind_texture (nux_gpu_encoder_t *enc, const nux_texture_t *texture)
{
    nux_graphics_module_t *gfx = nux_graphics();
    if (gfx->active_texture != texture)
    {
        gfx->active_texture = texture;
        if (gfx->active_texture)
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
      nux_u32_t          vertex_count,
      nux_u32_t          transform,
      nux_u32_t          attributes,
      nux_v4_t           color)
{
    nux_graphics_module_t *gfx = nux_graphics();

    // Create batch
    nux_u32_t batch_index;
    NUX_ENSURE(nux_dsa_push_bottom(&gfx->batches_dsa, 1, &batch_index),
               return NUX_FAILURE,
               "out of batches");
    nux_gpu_scene_batch_t batch;
    batch.vertex_offset     = first;
    batch.vertex_attributes = attributes;
    batch.transform_offset  = transform;
    batch.has_texture       = gfx->active_texture ? 1 : 0;
    batch.color             = color;
    NUX_ENSURE(nux_os_buffer_update(gfx->batches_buffer.slot,
                                    batch_index * sizeof(batch),
                                    sizeof(batch),
                                    &batch),
               return NUX_FAILURE,
               "failed to update batches buffer");

    // Add commands
    nux_gpu_push_u32(enc, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
    nux_gpu_draw(enc, vertex_count);

    return NUX_SUCCESS;
}
static void
draw_rect (nux_gpu_encoder_t     *enc,
           nux_u32_t              transform,
           nux_vertex_primitive_t primitive,
           const nux_v3_t        *positions,
           nux_u32_t              color)
{
    nux_graphics_module_t *gfx = nux_graphics();

    nux_u32_t        vertex_count;
    const nux_u32_t  stride = 5;
    nux_f32_t        data[24 * stride]; // must constains all primitives type
    const nux_u32_t *indices;

    switch (primitive)
    {
        case NUX_VERTEX_TRIANGLES: {
            static const nux_u32_t triangles_indices[]
                = { 0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7, 0, 3, 7, 7, 4, 0,
                    1, 5, 6, 6, 2, 1, 0, 4, 5, 5, 1, 0, 3, 2, 6, 6, 7, 3 };
            indices      = triangles_indices;
            vertex_count = NUX_ARRAY_SIZE(triangles_indices);
        }
        break;
        case NUX_VERTEX_LINES: {
            static const nux_u32_t lines_indices[]
                = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                    6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
            indices      = lines_indices;
            vertex_count = NUX_ARRAY_SIZE(lines_indices);
        }
        break;
        case NUX_VERTEX_POINTS:
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

    nux_v4_t c = nux_palette_get_color(gfx->active_palette, color);
    for (nux_u32_t i = 0; i < vertex_count; ++i)
    {
        nux_u32_t index      = indices[i];
        data[i * stride + 0] = positions[index].x;
        data[i * stride + 1] = positions[index].y;
        data[i * stride + 2] = positions[index].z;
        data[i * stride + 3] = uvs[index].x;
        data[i * stride + 4] = uvs[index].y;
    }

    nux_u32_t offset;
    NUX_CHECK(
        nux_graphics_push_frame_vertices(vertex_count * stride, data, &offset),
        return);

    draw(enc,
         offset,
         vertex_count,
         transform,
         NUX_VERTEX_POSITION | NUX_VERTEX_TEXCOORD,
         c);
}
static void
draw_box (nux_gpu_encoder_t     *enc,
          nux_u32_t              transform,
          nux_vertex_primitive_t primitive,
          nux_b3_t               box,
          nux_u32_t              color)
{
    const nux_v3_t positions[] = { nux_v3(box.min.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.min.z),
                                   nux_v3(box.max.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.min.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.min.z),
                                   nux_v3(box.max.x, box.max.y, box.max.z),
                                   nux_v3(box.min.x, box.max.y, box.max.z) };
    draw_rect(enc, transform, primitive, positions, color);
}

void
nux_renderer_render_scene (nux_scene_t *scene, nux_viewport_t *viewport)
{
    nux_graphics_module_t *gfx    = nux_graphics();
    nux_gpu_encoder_t     *enc    = &gfx->encoder;
    nux_v4_t               extent = nux_viewport_normalized_viewport(viewport);
    nux_nid_t              camera = viewport->source.camera;
    nux_texture_t         *target
        = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
    NUX_ASSERT(target);

    // Bind framebuffer
    nux_gpu_bind_framebuffer(enc, target->gpu.framebuffer_slot);
    nux_gpu_viewport(enc, extent);
    if (viewport->clear_depth)
    {
        nux_gpu_clear_depth(enc);
    }
    if (viewport->clear_color.w != 0)
    {
        nux_gpu_clear_color(enc, nux_color_to_hex(viewport->clear_color));
    }

    // Render scene
    if (camera)
    {
        // Update constants
        nux_camera_t *cc = nux_component_get(camera, NUX_COMPONENT_CAMERA);
        NUX_ASSERT(nux_node_has(camera, NUX_COMPONENT_TRANSFORM) && cc);

        nux_m4_t global_matrix = nux_transform_matrix(camera);

        nux_v3_t eye    = nux_m4_mulv3(global_matrix, NUX_V3_ZEROS, 1);
        nux_v3_t center = nux_m4_mulv3(global_matrix, NUX_V3_FORWARD, 1);
        nux_v3_t up     = nux_m4_mulv3(global_matrix, NUX_V3_UP, 0);

        nux_gpu_constants_buffer_t constants;
        constants.view        = nux_m4_lookat(eye, center, up);
        constants.proj        = nux_camera_projection(camera);
        constants.screen_size = nux_v2u(nux_stat_get(NUX_STAT_SCREEN_WIDTH),
                                        nux_stat_get(NUX_STAT_SCREEN_HEIGHT));
        constants.time        = nux_time_elapsed();
        nux_os_buffer_update(
            gfx->constants_buffer.slot, 0, sizeof(constants), &constants);

        // Render nodes
        nux_gpu_bind_pipeline(enc, gfx->uber_pipeline_opaque.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_CONSTANTS, gfx->constants_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_BATCHES, gfx->batches_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_TRANSFORMS, gfx->transforms_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_VERTICES, gfx->vertices_buffer.slot);
        nux_nid_t it = NUX_NULL;
        while ((it = nux_query_next(gfx->transform_staticmesh_iter, it)))
        {
            nux_staticmesh_t *sm
                = nux_component_get(it, NUX_COMPONENT_STATICMESH);
            if (!sm->mesh)
            {
                continue;
            }
            if (!(sm->render_layer & cc->render_mask))
            {
                continue;
            }
            nux_m4_t    global_matrix = nux_transform_matrix(it);
            nux_mesh_t *m = nux_resource_check(NUX_RESOURCE_MESH, sm->mesh);
            if (!m)
            {
                continue;
            }
            nux_texture_t *tex = NUX_NULL;
            if (sm->texture)
            {
                tex = nux_resource_check(NUX_RESOURCE_TEXTURE, sm->texture);
            }

            // Push transform
            NUX_CHECK(
                nux_graphics_push_transforms(1, &global_matrix, &sm->transform),
                continue);

            // Draw
            bind_texture(enc, tex);
            NUX_CHECK(draw(enc,
                           m->gpu.offset,
                           nux_mesh_size(m),
                           sm->transform,
                           m->attributes,
                           nux_v4s(1)),
                      return);
        }

        // Draw debug lines
        nux_gpu_bind_pipeline(enc, gfx->uber_pipeline_line.slot);
        it = NUX_NULL;
        while ((it = nux_query_next(gfx->transform_staticmesh_iter, it)))
        {
            nux_staticmesh_t *sm
                = nux_component_get(it, NUX_COMPONENT_STATICMESH);
            if (!sm->mesh || !sm->draw_bounds)
            {
                continue;
            }
            if (!(sm->render_layer & cc->render_mask))
            {
                continue;
            }
            nux_mesh_t *m = nux_resource_check(NUX_RESOURCE_MESH, sm->mesh);
            NUX_ASSERT(m);

            // Draw
            bind_texture(enc, NUX_NULL);
            draw_box(enc,
                     sm->transform,
                     NUX_VERTEX_LINES,
                     m->bounds,
                     NUX_COLOR_WHITE);
        }

        // Draw immediate lines
        nux_gpu_bind_pipeline(enc, gfx->uber_pipeline_line.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_CONSTANTS, gfx->constants_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_BATCHES, gfx->batches_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_TRANSFORMS, gfx->transforms_buffer.slot);
        nux_gpu_bind_buffer(
            enc, NUX_GPU_DESC_UBER_VERTICES, gfx->vertices_buffer.slot);
        for (nux_u32_t i = 0; i < gfx->immediate_commands.size; ++i)
        {
            nux_graphics_command_t *cmd = gfx->immediate_commands.data + i;
            if (cmd->layer & cc->render_mask)
            {
                draw(enc,
                     cmd->vertex_offset,
                     cmd->vertex_count,
                     cmd->transform_offset,
                     cmd->vertex_attributes,
                     cmd->color);
            }
        }

        // Submit commands
        nux_gpu_encoder_submit(&gfx->encoder);
    }
}
