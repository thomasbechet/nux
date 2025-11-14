#include "internal.h"

#include "fonts_data.c.inc"

static nux_graphics_module_t _module;

static nux_status_t
update_transform_buffer (nux_u32_t first, nux_u32_t count, const nux_m4_t *data)
{
    NUX_ENSURE(nux_os_buffer_update(_module.transforms_buffer.slot,
                                    first * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    count * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    return NUX_SUCCESS;
}
static nux_i32_t
viewport_compare (const void *a, const void *b)
{
    const nux_viewport_t *va = *(const nux_viewport_t **)a;
    const nux_viewport_t *vb = *(const nux_viewport_t **)b;
    return va->layer - vb->layer;
}

static void
module_pre_update (void)
{
    // Reset frame data
    nux_dsa_reset_bottom(&_module.batches_dsa);
    nux_dsa_reset_top(&_module.vertices_dsa);
    nux_dsa_reset_top(&_module.transforms_dsa);
    nux_dsa_reset_bottom(&_module.transforms_dsa);
    nux_dsa_push_bottom(
        &_module.transforms_dsa, 1, NUX_NULL); // keep identity transform
    _module.active_texture = NUX_NULL;

    // Reset immediate state
    _module.immediate_state = _module.immediate_states.data;
    nux_graphics_reset_state();

    // Update screen size
    _module.screen_target->gpu.width  = nux_stat_get(NUX_STAT_SCREEN_WIDTH);
    _module.screen_target->gpu.height = nux_stat_get(NUX_STAT_SCREEN_HEIGHT);

    // Update viewports with auto resize enabled
    nux_viewport_t *vp = NUX_NULL;
    while ((vp = nux_resource_next(NUX_RESOURCE_VIEWPORT, vp)))
    {
        if (vp->auto_resize
            && vp->target == nux_resource_rid(_module.screen_target))
        {
            nux_viewport_set_extent(vp,
                                    nux_b2i(0,
                                            0,
                                            _module.screen_target->gpu.width,
                                            _module.screen_target->gpu.height));
        }
    }
}
static void
module_update (void)
{
    // Propagate transforms
    nux_nid_t transform = NUX_NULL;
    while ((transform = nux_query_next(_module.transform_iter, transform)))
    {
        nux_transform_matrix(transform);
    }

    // Upload meshes
    nux_mesh_t *mesh = NUX_NULL;
    while ((mesh = nux_resource_next(NUX_RESOURCE_MESH, mesh)))
    {
        nux_mesh_upload(mesh);
    }

    // Update textures
    nux_texture_t *texture = NUX_NULL;
    while ((texture = nux_resource_next(NUX_RESOURCE_TEXTURE, texture)))
    {
        if (texture->dirty)
        {
            nux_texture_upload(texture);
        }
    }

    // Submit canvas commands
    nux_canvas_t *canvas = NUX_NULL;
    while ((canvas = nux_resource_next(NUX_RESOURCE_CANVAS, canvas)))
    {
        nux_canvas_render(canvas);
    }

    // Collect viewports
    nux_viewport_t *viewports[32];
    nux_u32_t       viewports_count = 0;
    nux_viewport_t *vp              = NUX_NULL;
    while ((vp = nux_resource_next(NUX_RESOURCE_VIEWPORT, vp)))
    {
        NUX_ASSERT(viewports_count < NUX_ARRAY_SIZE(viewports));
        viewports[viewports_count] = vp;
        ++viewports_count;
    }

    // Sort viewports
    nux_qsort(viewports, viewports_count, sizeof(*viewports), viewport_compare);

    // Render viewports
    for (nux_u32_t i = 0; i < viewports_count; ++i)
    {
        nux_viewport_t *viewport = viewports[i];
        nux_v4_t        extent   = nux_viewport_normalized_viewport(viewport);

        nux_texture_t *target
            = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
        // Skip empty viewports
        if (target->gpu.width * target->gpu.height == 0)
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
    nux_vec_clear(&_module.immediate_commands);
}
static nux_status_t
module_init (void)
{
    nux_arena_t *a = nux_arena_core();

    // Register systems
    nux_system_register(NUX_SYSTEM_PRE_UPDATE, module_pre_update);
    nux_system_register(NUX_SYSTEM_UPDATE, module_update);

    // Register resources
    nux_resource_register(
        NUX_RESOURCE_VIEWPORT,
        (nux_resource_info_t) { .name = "viewport",
                                .size = sizeof(nux_viewport_t) });
    nux_resource_register(
        NUX_RESOURCE_TEXTURE,
        (nux_resource_info_t) { .name    = "texture",
                                .size    = sizeof(nux_texture_t),
                                .cleanup = nux_texture_cleanup });
    nux_resource_register(
        NUX_RESOURCE_PALETTE,
        (nux_resource_info_t) { .name = "palette",
                                .size = sizeof(nux_palette_t) });
    nux_resource_register(
        NUX_RESOURCE_MESH,
        (nux_resource_info_t) { .name = "mesh", .size = sizeof(nux_mesh_t) });
    nux_resource_register(
        NUX_RESOURCE_CANVAS,
        (nux_resource_info_t) { .name    = "canvas",
                                .size    = sizeof(nux_canvas_t),
                                .cleanup = nux_canvas_cleanup });
    nux_resource_register(
        NUX_RESOURCE_FONT,
        (nux_resource_info_t) { .name    = "font",
                                .size    = sizeof(nux_font_t),
                                .cleanup = nux_font_cleanup });

    // Register components
    nux_component_register(NUX_COMPONENT_CAMERA,
                           (nux_component_info_t) {
                               .name  = "camera",
                               .size  = sizeof(nux_camera_t),
                               .add   = nux_camera_add,
                               .write = nux_camera_write,
                               .read  = nux_camera_read,
                           });
    nux_component_register(NUX_COMPONENT_STATICMESH,
                           (nux_component_info_t) {
                               .name  = "staticmesh",
                               .size  = sizeof(nux_staticmesh_t),
                               .add   = nux_staticmesh_add,
                               .write = nux_staticmesh_write,
                               .read  = nux_staticmesh_read,
                           });

    // Initialize gpu slots
    nux_vec_init_capa(
        &_module.free_framebuffer_slots, a, NUX_GPU_FRAMEBUFFER_MAX);
    nux_vec_init_capa(&_module.free_pipeline_slots, a, NUX_GPU_PIPELINE_MAX);
    nux_vec_init_capa(&_module.free_texture_slots, a, NUX_GPU_TEXTURE_MAX);
    nux_vec_init_capa(&_module.free_buffer_slots, a, NUX_GPU_BUFFER_MAX);
    nux_u32_vec_fill_reversed(&_module.free_framebuffer_slots);
    nux_u32_vec_fill_reversed(&_module.free_pipeline_slots);
    nux_u32_vec_fill_reversed(&_module.free_buffer_slots);
    nux_u32_vec_fill_reversed(&_module.free_texture_slots);

    // Create pipelines
    _module.uber_pipeline_opaque.info.type              = NUX_GPU_PIPELINE_UBER;
    _module.uber_pipeline_opaque.info.primitive         = NUX_VERTEX_TRIANGLES;
    _module.uber_pipeline_opaque.info.enable_blend      = NUX_FALSE;
    _module.uber_pipeline_opaque.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(&_module.uber_pipeline_opaque), goto error);
    _module.uber_pipeline_line.info.type              = NUX_GPU_PIPELINE_UBER;
    _module.uber_pipeline_line.info.primitive         = NUX_VERTEX_LINES;
    _module.uber_pipeline_line.info.enable_blend      = NUX_FALSE;
    _module.uber_pipeline_line.info.enable_depth_test = NUX_TRUE;
    NUX_CHECK(nux_gpu_pipeline_init(&_module.uber_pipeline_line), goto error);
    _module.canvas_pipeline.info.type              = NUX_GPU_PIPELINE_CANVAS;
    _module.uber_pipeline_opaque.info.primitive    = NUX_VERTEX_TRIANGLES;
    _module.canvas_pipeline.info.enable_blend      = NUX_TRUE;
    _module.canvas_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(&_module.canvas_pipeline), goto error);
    _module.blit_pipeline.info.type              = NUX_GPU_PIPELINE_BLIT;
    _module.uber_pipeline_opaque.info.primitive  = NUX_VERTEX_TRIANGLES;
    _module.blit_pipeline.info.enable_blend      = NUX_TRUE;
    _module.blit_pipeline.info.enable_depth_test = NUX_FALSE;
    NUX_CHECK(nux_gpu_pipeline_init(&_module.blit_pipeline), goto error);

    // Create vertices buffers
    _module.vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.vertices_buffer.size
        = nux_config_get()->graphics.vertices_buffer_size * sizeof(nux_f32_t);
    nux_dsa_init(&_module.vertices_dsa,
                 nux_config_get()->graphics.vertices_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&_module.vertices_buffer), goto error);

    // Create default font
    NUX_CHECK(nux_font_init_default(&_module.default_font), goto error);

    // Create default palette
    NUX_CHECK(nux_palette_register_default(), goto error);

    // Allocate gpu commands buffer
    nux_gpu_encoder_init(a, &_module.encoder);

    // Allocate immediate command buffer
    nux_vec_init(&_module.immediate_commands, a);
    nux_vec_init_capa(&_module.immediate_states,
                      a,
                      NUX_GRAPHICS_DEFAULT_IMMEDIATE_STACK_SIZE);
    nux_vec_push(&_module.immediate_states);

    // Allocate constants buffer
    _module.constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    _module.constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(&_module.constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    _module.batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.batches_buffer.size
        = sizeof(nux_gpu_scene_batch_t)
          * nux_config_get()->graphics.batches_buffer_size;
    nux_dsa_init(&_module.batches_dsa,
                 nux_config_get()->graphics.batches_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&_module.batches_buffer), return NUX_FAILURE);

    // Allocate transforms buffer
    _module.transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.transforms_buffer.size
        = NUX_M4_SIZE * nux_config_get()->graphics.transforms_buffer_size
          * sizeof(nux_f32_t);
    nux_dsa_init(&_module.transforms_dsa,
                 nux_config_get()->graphics.transforms_buffer_size);
    NUX_CHECK(nux_gpu_buffer_init(&_module.transforms_buffer),
              return NUX_FAILURE);

    // Create iterators
    _module.transform_iter = nux_query_new(nux_arena_core(), 1, 0);
    NUX_CHECK(_module.transform_iter, return NUX_FAILURE);
    nux_query_includes(_module.transform_iter, NUX_COMPONENT_TRANSFORM);

    _module.transform_camera_iter = nux_query_new(nux_arena_core(), 2, 0);
    NUX_CHECK(_module.transform_camera_iter, return NUX_FAILURE);
    nux_query_includes(_module.transform_camera_iter, NUX_COMPONENT_TRANSFORM);
    nux_query_includes(_module.transform_camera_iter, NUX_COMPONENT_CAMERA);

    _module.transform_staticmesh_iter = nux_query_new(nux_arena_core(), 2, 0);
    NUX_CHECK(_module.transform_staticmesh_iter, return NUX_FAILURE);
    nux_query_includes(_module.transform_staticmesh_iter,
                       NUX_COMPONENT_TRANSFORM);
    nux_query_includes(_module.transform_staticmesh_iter,
                       NUX_COMPONENT_STATICMESH);

    // Push identity transform
    nux_m4_t identity = nux_m4_identity();
    NUX_ASSERT(nux_graphics_push_transforms(
        1, &identity, &_module.identity_transform_offset));
    NUX_ASSERT(_module.identity_transform_offset == 0);

    // Create screen rendertarget
    _module.screen_target
        = nux_resource_new(nux_arena_core(), NUX_RESOURCE_TEXTURE);
    NUX_CHECK(_module.screen_target, return NUX_FAILURE);
    _module.screen_target->gpu.type             = NUX_TEXTURE_RENDER_TARGET;
    _module.screen_target->gpu.framebuffer_slot = 0;
    _module.screen_target->gpu.width            = 1600;
    _module.screen_target->gpu.height           = 900;
    nux_vec_pop(&_module.free_framebuffer_slots);

    return NUX_SUCCESS;

error:
    return NUX_FAILURE;
}
static void
module_free (void)
{
    nux_gpu_buffer_free(&_module.constants_buffer);
    nux_gpu_buffer_free(&_module.batches_buffer);
    nux_gpu_buffer_free(&_module.transforms_buffer);

    nux_gpu_pipeline_free(&_module.uber_pipeline_line);
    nux_gpu_pipeline_free(&_module.uber_pipeline_opaque);
    nux_gpu_pipeline_free(&_module.canvas_pipeline);
    nux_gpu_pipeline_free(&_module.blit_pipeline);

    nux_gpu_buffer_free(&_module.vertices_buffer);

    nux_font_free(&_module.default_font);

    NUX_ASSERT(_module.free_texture_slots.size == NUX_GPU_TEXTURE_MAX);
    NUX_ASSERT(_module.free_buffer_slots.size == NUX_GPU_BUFFER_MAX);
    NUX_ASSERT(_module.free_pipeline_slots.size == NUX_GPU_PIPELINE_MAX);
    NUX_ASSERT(_module.free_framebuffer_slots.size
               == NUX_GPU_FRAMEBUFFER_MAX - 1); // 0 reserved for default
}
void
nux_graphics_module_register (void)
{
    NUX_MODULE_REGISTER("graphics", &_module, module_init, module_free);
}

nux_graphics_module_t *
nux_graphics (void)
{
    return &_module;
}

nux_status_t
nux_graphics_update_vertices (nux_u32_t        offset,
                              nux_u32_t        count,
                              const nux_f32_t *data)
{
    NUX_ENSURE(nux_os_buffer_update(_module.vertices_buffer.slot,
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
    NUX_ENSURE(nux_dsa_push_bottom(&_module.vertices_dsa, count, offset),
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
    NUX_ENSURE(nux_dsa_push_top(&_module.vertices_dsa, count, offset),
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
    NUX_ENSURE(nux_dsa_push_bottom(&_module.transforms_dsa, count, offset),
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
    NUX_ENSURE(nux_dsa_push_top(&_module.transforms_dsa, count, offset),
               return NUX_FAILURE,
               "out of frame transforms");
    NUX_CHECK(update_transform_buffer(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
