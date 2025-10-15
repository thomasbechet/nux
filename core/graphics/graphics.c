#include "internal.h"

#include "fonts_data.c.inc"

NUX_VEC_IMPL(nux_gpu_command_vec, nux_gpu_command_t)
NUX_VEC_IMPL(nux_graphics_command_vec, nux_graphics_command_t)

static nux_status_t
update_transform_buffer (nux_u32_t first, nux_u32_t count, const nux_m4_t *data)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_os_buffer_update(nux_userdata(),
                                    module->transforms_buffer.slot,
                                    first * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    count * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    return NUX_SUCCESS;
}

nux_status_t
nux_graphics_init (void)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_arena_t           *a      = nux_arena_core();

    // Register resources
    nux_resource_type_t *type;
    type = nux_resource_register(
        NUX_RESOURCE_VIEWPORT, sizeof(nux_viewport_t), "viewport");
    type = nux_resource_register(
        NUX_RESOURCE_TEXTURE, sizeof(nux_texture_t), "texture");
    type->cleanup = nux_texture_cleanup;
    type          = nux_resource_register(
        NUX_RESOURCE_PALETTE, sizeof(nux_palette_t), "palette");
    type = nux_resource_register(NUX_RESOURCE_MESH, sizeof(nux_mesh_t), "mesh");
    type = nux_resource_register(
        NUX_RESOURCE_CANVAS, sizeof(nux_canvas_t), "canvas");
    type->cleanup = nux_canvas_cleanup;
    type = nux_resource_register(NUX_RESOURCE_FONT, sizeof(nux_font_t), "font");
    type->cleanup = nux_font_cleanup;

    // Register components
    nux_component_t *comp;
    comp = nux_component_register(
        NUX_COMPONENT_CAMERA, "camera", sizeof(nux_camera_t));
    comp->read  = nux_camera_read;
    comp->write = nux_camera_write;
    comp        = nux_component_register(
        NUX_COMPONENT_STATICMESH, "staticmesh", sizeof(nux_staticmesh_t));
    comp->read  = nux_staticmesh_read;
    comp->write = nux_staticmesh_write;

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

    // Create pipelines
    module->uber_pipeline_opaque.info.type         = NUX_GPU_PIPELINE_UBER;
    module->uber_pipeline_opaque.info.primitive    = NUX_PRIMITIVE_TRIANGLES;
    module->uber_pipeline_opaque.info.enable_blend = NUX_FALSE;
    module->uber_pipeline_opaque.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(&module->uber_pipeline_opaque), goto error);
    module->uber_pipeline_line.info.type              = NUX_GPU_PIPELINE_UBER;
    module->uber_pipeline_line.info.primitive         = NUX_PRIMITIVE_LINES;
    module->uber_pipeline_line.info.enable_blend      = NUX_FALSE;
    module->uber_pipeline_line.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(&module->uber_pipeline_line), goto error);
    module->canvas_pipeline.info.type              = NUX_GPU_PIPELINE_CANVAS;
    module->uber_pipeline_opaque.info.primitive    = NUX_PRIMITIVE_TRIANGLES;
    module->canvas_pipeline.info.enable_blend      = NUX_TRUE;
    module->canvas_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(&module->canvas_pipeline), goto error);
    module->blit_pipeline.info.type              = NUX_GPU_PIPELINE_BLIT;
    module->uber_pipeline_opaque.info.primitive  = NUX_PRIMITIVE_TRIANGLES;
    module->blit_pipeline.info.enable_blend      = NUX_TRUE;
    module->blit_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(&module->blit_pipeline), goto error);

    // Create vertices buffers
    module->vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->vertices_buffer.size
        = nux_config()->graphics.vertices_buffer_size * sizeof(nux_f32_t);
    nux_dsa_init(&module->vertices_dsa,
                 nux_config()->graphics.vertices_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&module->vertices_buffer), goto error);

    // Create default font
    NUX_CHECK(nux_font_init_default(&module->default_font), goto error);

    // Create default palette
    NUX_CHECK(nux_palette_register_default(), goto error);

    // Allocate gpu commands buffer
    NUX_CHECK(nux_gpu_encoder_init(a, &module->encoder), return NUX_NULL);

    // Allocate immediate command buffer
    NUX_CHECK(nux_graphics_command_vec_init(a, &module->immediate_commands),
              return NUX_NULL);

    // Allocate constants buffer
    module->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    module->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(&module->constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    module->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->batches_buffer.size = sizeof(nux_gpu_scene_batch_t)
                                  * nux_config()->graphics.batches_buffer_size;
    nux_dsa_init(&module->batches_dsa,
                 nux_config()->graphics.batches_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&module->batches_buffer), return NUX_FAILURE);

    // Allocate transforms buffer
    module->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    module->transforms_buffer.size
        = NUX_M4_SIZE * nux_config()->graphics.transforms_buffer_size
          * sizeof(nux_f32_t);
    nux_dsa_init(&module->transforms_dsa,
                 nux_config()->graphics.transforms_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&module->transforms_buffer),
              return NUX_FAILURE);

    // Create iterators
    module->transform_iter = nux_query_new(nux_arena_core(), 1, 0);
    NUX_CHECK(module->transform_iter, return NUX_FAILURE);
    nux_query_includes(module->transform_iter, NUX_COMPONENT_TRANSFORM);

    module->transform_camera_iter = nux_query_new(nux_arena_core(), 2, 0);
    NUX_CHECK(module->transform_camera_iter, return NUX_FAILURE);
    nux_query_includes(module->transform_camera_iter, NUX_COMPONENT_TRANSFORM);
    nux_query_includes(module->transform_camera_iter, NUX_COMPONENT_CAMERA);

    module->transform_staticmesh_iter = nux_query_new(nux_arena_core(), 2, 0);
    NUX_CHECK(module->transform_staticmesh_iter, return NUX_FAILURE);
    nux_query_includes(module->transform_staticmesh_iter,
                       NUX_COMPONENT_TRANSFORM);
    nux_query_includes(module->transform_staticmesh_iter,
                       NUX_COMPONENT_STATICMESH);

    // Push identity transform
    nux_m4_t identity = nux_m4_identity();
    NUX_ASSERT(nux_graphics_push_transforms(
        1, &identity, &module->identity_transform_offset));
    NUX_ASSERT(module->identity_transform_offset == 0);

    // Create screen rendertarget
    module->screen_target
        = nux_resource_new(nux_arena_core(), NUX_RESOURCE_TEXTURE);
    NUX_CHECK(module->screen_target, return NUX_FAILURE);
    module->screen_target->gpu.type             = NUX_TEXTURE_RENDER_TARGET;
    module->screen_target->gpu.framebuffer_slot = 0;
    module->screen_target->gpu.width            = 0;
    module->screen_target->gpu.height           = 0;
    nux_u32_vec_pop(&module->free_framebuffer_slots);

    return NUX_SUCCESS;

error:
    return NUX_FAILURE;
}
void
nux_graphics_free (void)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_CHECK(module, return);

    nux_gpu_buffer_free(&module->constants_buffer);
    nux_gpu_buffer_free(&module->batches_buffer);
    nux_gpu_buffer_free(&module->transforms_buffer);

    nux_gpu_pipeline_free(&module->uber_pipeline_line);
    nux_gpu_pipeline_free(&module->uber_pipeline_opaque);
    nux_gpu_pipeline_free(&module->canvas_pipeline);
    nux_gpu_pipeline_free(&module->blit_pipeline);

    nux_gpu_buffer_free(&module->vertices_buffer);

    nux_font_free(&module->default_font);

    NUX_ASSERT(module->free_texture_slots.size == NUX_GPU_TEXTURE_MAX);
    NUX_ASSERT(module->free_buffer_slots.size == NUX_GPU_BUFFER_MAX);
    NUX_ASSERT(module->free_pipeline_slots.size == NUX_GPU_PIPELINE_MAX);
    NUX_ASSERT(module->free_framebuffer_slots.size
               == NUX_GPU_FRAMEBUFFER_MAX - 1); // 0 reserved for default
}
nux_status_t
nux_graphics_pre_update (void)
{
    nux_graphics_module_t *module = nux_graphics_module();

    // Reset frame data
    nux_dsa_reset_bottom(&module->batches_dsa);
    nux_dsa_reset_top(&module->vertices_dsa);
    nux_dsa_reset_top(&module->transforms_dsa);
    nux_dsa_reset_bottom(&module->transforms_dsa);
    nux_dsa_push_bottom(
        &module->transforms_dsa, 1, NUX_NULL); // keep identity transform
    module->active_texture = NUX_NULL;

    // Update default screen size
    module->screen_target->gpu.width  = nux_stat(NUX_STAT_SCREEN_WIDTH);
    module->screen_target->gpu.height = nux_stat(NUX_STAT_SCREEN_HEIGHT);

    return NUX_SUCCESS;
}
static nux_i32_t
viewport_compare (const void *a, const void *b)
{
    const nux_viewport_t *va = *(const nux_viewport_t **)a;
    const nux_viewport_t *vb = *(const nux_viewport_t **)b;
    return va->layer - vb->layer;
}
nux_status_t
nux_graphics_update (void)
{
    nux_graphics_module_t *module = nux_graphics_module();

    // Propagate transforms
    {
        nux_nid_t it = NUX_NULL;
        while ((it = nux_query_next(module->transform_iter, it)))
        {
            nux_transform_get_matrix(it);
        }
    }

    // Submit canvas commands
    nux_rid_t canvas = NUX_NULL;
    while ((canvas = nux_resource_next(NUX_RESOURCE_CANVAS, canvas)))
    {
        nux_canvas_t *c = nux_resource_check(NUX_RESOURCE_CANVAS, canvas);
        nux_canvas_render(c);
    }

    // Collect viewports
    nux_viewport_t *viewports[32];
    nux_u32_t       viewports_count = 0;
    nux_rid_t       it              = NUX_NULL;
    while ((it = nux_resource_next(NUX_RESOURCE_VIEWPORT, it)))
    {
        viewports[viewports_count]
            = nux_resource_get(NUX_RESOURCE_VIEWPORT, it);
        ++viewports_count;
    }

    // Sort viewports
    nux_qsort(viewports, viewports_count, sizeof(*viewports), viewport_compare);

    // Render viewports
    for (nux_u32_t i = 0; i < viewports_count; ++i)
    {
        nux_viewport_t *viewport = viewports[i];
        nux_v4_t        extent   = nux_viewport_get_render_extent(viewport);

        nux_texture_t *target
            = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
        // Empty frame
        if (target->gpu.width * target->gpu.height <= 0)
        {
            continue;
        }
        if (viewport->source.camera) // Render scene
        {
            nux_scene_t *scene = nux_scene_active();
            nux_renderer_render_scene(scene, viewport);
        }
        else if (viewport->source.texture) // Blit texture
        {
            nux_texture_t *texture = nux_resource_get(NUX_RESOURCE_TEXTURE,
                                                      viewport->source.texture);
            NUX_ASSERT(texture);
            nux_texture_blit(texture, target, extent);
        }
    }

    // Clear frame buffers
    nux_graphics_command_vec_clear(&module->immediate_commands);

    return NUX_SUCCESS;
}

nux_status_t
nux_graphics_update_vertices (nux_u32_t        offset,
                              nux_u32_t        count,
                              const nux_f32_t *data)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_os_buffer_update(nux_userdata(),
                                    module->vertices_buffer.slot,
                                    offset * sizeof(nux_f32_t),
                                    count * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update vertex buffer");
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_vertices (nux_u32_t        count,
                            const nux_f32_t *data,
                            nux_u32_t       *offset)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_dsa_push_bottom(&module->vertices_dsa, count, offset),
               return NUX_FAILURE,
               "out of vertices");
    NUX_CHECK(nux_graphics_update_vertices(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_vertices (nux_u32_t        count,
                                  const nux_f32_t *data,
                                  nux_u32_t       *offset)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_dsa_push_top(&module->vertices_dsa, count, offset),
               return NUX_FAILURE,
               "out of frame vertices");
    NUX_CHECK(nux_graphics_update_vertices(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_transforms (nux_u32_t       count,
                              const nux_m4_t *data,
                              nux_u32_t      *offset)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_dsa_push_bottom(&module->transforms_dsa, count, offset),
               return NUX_FAILURE,
               "out of transforms");
    NUX_CHECK(update_transform_buffer(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_transforms (nux_u32_t       count,
                                    const nux_m4_t *data,
                                    nux_u32_t      *offset)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_ENSURE(nux_dsa_push_top(&module->transforms_dsa, count, offset),
               return NUX_FAILURE,
               "out of frame transforms");
    NUX_CHECK(update_transform_buffer(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_texture_t *
nux_graphics_screen_target (void)
{
    return nux_graphics_module()->screen_target;
}
