#include "internal.h"

#include "fonts_data.c.inc"

static nux_graphics_module_t _module;

static nux_status_t
update_transform_buffer (nux_u32_t first, nux_u32_t count, const nux_m4_t *data)
{
    nux_ensure(nux_os_buffer_update(_module.transforms_buffer.slot,
                                    first * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    count * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    return NUX_SUCCESS;
}
static nux_i32_t
camera_compare (const void *a, const void *b)
{
    const nux_camera_t *ca
        = nux_component_get(*(const nux_id_t *)a, NUX_COMPONENT_CAMERA);
    const nux_camera_t *cb
        = nux_component_get(*(const nux_id_t *)b, NUX_COMPONENT_CAMERA);
    return ca->layer - cb->layer;
}
static nux_b2i_t
apply_viewport_mode (nux_v2u_t           source_size,
                     nux_b2i_t           target_viewport,
                     nux_viewport_mode_t mode)
{
    // Get target resolution
    nux_v2u_t target_size  = nux_v2u(target_viewport.w, target_viewport.h);
    nux_f32_t target_ratio = (nux_f32_t)target_size.x / target_size.y;

    // Get source resolution
    nux_f32_t source_ratio = (nux_f32_t)source_size.x / source_size.y;

    nux_v2u_t vsize = { 0, 0 };
    switch (mode)
    {
        case NUX_VIEWPORT_FIXED:
            vsize.x = source_size.x;
            vsize.y = source_size.y;
            break;
        case NUX_VIEWPORT_FIXED_BEST_FIT: {
            nux_f32_t w_factor = (nux_f32_t)target_size.x / source_size.x;
            nux_f32_t h_factor = (nux_f32_t)target_size.y / source_size.y;
            nux_f32_t min      = w_factor < h_factor ? w_factor : h_factor;
            if (min < 1)
            {
                // 0.623 => 0.5
                // 0,432 => 0.25
                // 0.115 => 0,125
                nux_f32_t n = 2;
                while (min < (1 / n))
                {
                    n *= 2;
                }
                min = 1 / n;
            }
            else
            {
                min = nux_floor(min);
            }
            vsize.x = source_size.x * min;
            vsize.y = source_size.y * min;
        }
        break;
        case NUX_VIEWPORT_STRETCH_KEEP_ASPECT:
            if (target_ratio >= source_ratio)
            {
                vsize.x = nux_floor(target_size.y * source_ratio);
                vsize.y = nux_floor(target_size.y);
            }
            else
            {
                vsize.x = nux_floor(target_size.x);
                vsize.y = nux_floor(target_size.x / source_ratio);
            }
            break;
        case NUX_VIEWPORT_STRETCH:
            vsize = target_size;
            break;
        default:
            break;
    }

    // // Compute final pixels coordinates
    nux_v2i_t vpos;
    nux_u32_t anchor = 0;
    if (anchor & NUX_ANCHOR_LEFT)
    {
        vpos.x = 0;
    }
    else if (anchor & NUX_ANCHOR_RIGHT)
    {
        vpos.x = target_viewport.w - vsize.x;
    }
    else
    {
        vpos.x = ((nux_i32_t)target_size.x - (nux_i32_t)vsize.x) / 2;
    }
    if (anchor & NUX_ANCHOR_TOP)
    {
        vpos.y = 0;
    }
    else if (anchor & NUX_ANCHOR_BOTTOM)
    {
        vpos.y = target_viewport.h - vsize.y;
    }
    else
    {
        vpos.y = ((nux_i32_t)target_size.y - (nux_i32_t)vsize.y) / 2;
    }
    vpos.x += target_viewport.x;
    vpos.y += target_viewport.y;

    return nux_b2i(vpos.x, vpos.y, vsize.x, vsize.y);
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
        &_module.transforms_dsa, 1, nullptr); // keep identity transform
    _module.active_texture = nullptr;

    // Reset immediate state
    _module.immediate_state = _module.immediate_states.data;
    nux_graphics_reset_state();

    // Update screen size
    _module.window_size.x = nux_stat_get(NUX_STAT_SCREEN_WIDTH);
    _module.window_size.y = nux_stat_get(NUX_STAT_SCREEN_HEIGHT);
}
static void
module_update (void)
{
    // Propagate transforms
    nux_id_t transform = NUX_NULL;
    while ((transform = nux_query_next(_module.query_transform, transform)))
    {
        nux_transform_matrix(transform);
    }

    // Upload meshes
    nux_mesh_t *mesh = nullptr;
    while ((mesh = nux_object_next(NUX_OBJECT_MESH, mesh)))
    {
        nux_mesh_upload(mesh);
    }

    // Update textures
    nux_texture_t *texture = nullptr;
    while ((texture = nux_object_next(NUX_OBJECT_TEXTURE, texture)))
    {
        if (texture->dirty)
        {
            nux_texture_upload(texture);
        }
    }

    // Collect cameras
    nux_id_t  cameras[32];
    nux_u32_t cameras_count = 0;
    nux_id_t  it            = NUX_NULL;
    while ((it = nux_query_next(_module.query_transform_camera, it)))
    {
        nux_assert(cameras_count < nux_array_size(cameras));
        cameras[cameras_count] = it;
        ++cameras_count;
    }

    // Sort cameras
    nux_qsort(cameras, cameras_count, sizeof(*cameras), camera_compare);

    // Render cameras
    for (nux_u32_t i = 0; i < cameras_count; ++i)
    {
        nux_id_t     camera = cameras[i];
        nux_scene_t *scene  = nux_scene_active();
        nux_renderer_render_scene(scene, camera);
    }

    // Submit canvas commands
    nux_canvas_t *canvas = nullptr;
    while ((canvas = nux_object_next(NUX_OBJECT_CANVAS, canvas)))
    {
        nux_canvas_render(canvas);
    }

    // Blit screen to window
    nux_b2i_t viewport = apply_viewport_mode(
        nux_v2u(_module.screen_target->gpu.width,
                _module.screen_target->gpu.height),
        nux_b2i(0, 0, _module.window_size.x, _module.window_size.y),
        NUX_VIEWPORT_STRETCH_KEEP_ASPECT);
    nux_texture_blit(_module.screen_target, nullptr, viewport);

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
    nux_object_register(NUX_OBJECT_TEXTURE,
                        (nux_object_info_t) { .name    = "texture",
                                              .size    = sizeof(nux_texture_t),
                                              .cleanup = nux_texture_cleanup });
    nux_object_register(NUX_OBJECT_PALETTE,
                        (nux_object_info_t) { .name = "palette",
                                              .size = sizeof(nux_palette_t) });
    nux_object_register(
        NUX_OBJECT_MESH,
        (nux_object_info_t) { .name = "mesh", .size = sizeof(nux_mesh_t) });
    nux_object_register(NUX_OBJECT_CANVAS,
                        (nux_object_info_t) { .name    = "canvas",
                                              .size    = sizeof(nux_canvas_t),
                                              .cleanup = nux_canvas_cleanup });
    nux_object_register(NUX_OBJECT_FONT,
                        (nux_object_info_t) { .name    = "font",
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
    nux_u32_vec_fill_reversed(&_module.free_framebuffer_slots,
                              NUX_GPU_FRAMEBUFFER_MAX);
    nux_u32_vec_fill_reversed(&_module.free_pipeline_slots,
                              NUX_GPU_PIPELINE_MAX);
    nux_u32_vec_fill_reversed(&_module.free_buffer_slots, NUX_GPU_BUFFER_MAX);
    nux_u32_vec_fill_reversed(&_module.free_texture_slots, NUX_GPU_TEXTURE_MAX);

    // Reserve slot 0 for window framebuffer
    nux_assert(*nux_vec_pop(&_module.free_framebuffer_slots) == 0);

    // Create pipelines
    _module.uber_pipeline_opaque.info.type              = NUX_GPU_PIPELINE_UBER;
    _module.uber_pipeline_opaque.info.primitive         = NUX_VERTEX_TRIANGLES;
    _module.uber_pipeline_opaque.info.enable_blend      = false;
    _module.uber_pipeline_opaque.info.enable_depth_test = true;
    nux_check(nux_gpu_pipeline_init(&_module.uber_pipeline_opaque), goto error);
    _module.uber_pipeline_line.info.type              = NUX_GPU_PIPELINE_UBER;
    _module.uber_pipeline_line.info.primitive         = NUX_VERTEX_LINES;
    _module.uber_pipeline_line.info.enable_blend      = false;
    _module.uber_pipeline_line.info.enable_depth_test = true;
    nux_check(nux_gpu_pipeline_init(&_module.uber_pipeline_line), goto error);
    _module.canvas_pipeline.info.type              = NUX_GPU_PIPELINE_CANVAS;
    _module.uber_pipeline_opaque.info.primitive    = NUX_VERTEX_TRIANGLES;
    _module.canvas_pipeline.info.enable_blend      = true;
    _module.canvas_pipeline.info.enable_depth_test = false;
    nux_check(nux_gpu_pipeline_init(&_module.canvas_pipeline), goto error);
    _module.blit_pipeline.info.type              = NUX_GPU_PIPELINE_BLIT;
    _module.uber_pipeline_opaque.info.primitive  = NUX_VERTEX_TRIANGLES;
    _module.blit_pipeline.info.enable_blend      = true;
    _module.blit_pipeline.info.enable_depth_test = false;
    nux_check(nux_gpu_pipeline_init(&_module.blit_pipeline), goto error);

    // Create vertices buffers
    _module.vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.vertices_buffer.size
        = nux_config_get()->graphics.vertices_buffer_size * sizeof(nux_f32_t);
    nux_dsa_init(&_module.vertices_dsa,
                 nux_config_get()->graphics.vertices_buffer_size);
    nux_check(nux_gpu_buffer_init(&_module.vertices_buffer), goto error);

    // Create default font
    nux_check(nux_font_init_default(&_module.default_font), goto error);

    // Create default palette
    nux_check(nux_palette_register_default(), goto error);

    // Allocate gpu commands buffer
    nux_gpu_encoder_init(a, &_module.encoder);

    // Allocate immediate command buffer
    nux_vec_init(&_module.immediate_commands, a);
    nux_vec_init_capa(
        &_module.immediate_states, a, GRAPHICS_DEFAULT_IMMEDIATE_STACK_SIZE);
    nux_vec_push(&_module.immediate_states);

    // Allocate constants buffer
    _module.constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    _module.constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    nux_check(nux_gpu_buffer_init(&_module.constants_buffer),
              return NUX_FAILURE);

    // Allocate batches buffer
    _module.batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.batches_buffer.size
        = sizeof(nux_gpu_scene_batch_t)
          * nux_config_get()->graphics.batches_buffer_size;
    nux_dsa_init(&_module.batches_dsa,
                 nux_config_get()->graphics.batches_buffer_size);
    nux_check(nux_gpu_buffer_init(&_module.batches_buffer), return NUX_FAILURE);

    // Allocate transforms buffer
    _module.transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    _module.transforms_buffer.size
        = NUX_M4_SIZE * nux_config_get()->graphics.transforms_buffer_size
          * sizeof(nux_f32_t);
    nux_dsa_init(&_module.transforms_dsa,
                 nux_config_get()->graphics.transforms_buffer_size);
    nux_check(nux_gpu_buffer_init(&_module.transforms_buffer),
              return NUX_FAILURE);

    // Create queries
    _module.query_transform = nux_query_new(nux_arena_core(), 1, 0);
    nux_check(_module.query_transform, return NUX_FAILURE);
    nux_query_includes(_module.query_transform, NUX_COMPONENT_TRANSFORM);

    _module.query_transform_camera = nux_query_new(nux_arena_core(), 2, 0);
    nux_check(_module.query_transform_camera, return NUX_FAILURE);
    nux_query_includes(_module.query_transform_camera, NUX_COMPONENT_TRANSFORM);
    nux_query_includes(_module.query_transform_camera, NUX_COMPONENT_CAMERA);

    _module.query_transform_staticmesh = nux_query_new(nux_arena_core(), 2, 0);
    nux_check(_module.query_transform_staticmesh, return NUX_FAILURE);
    nux_query_includes(_module.query_transform_staticmesh,
                       NUX_COMPONENT_TRANSFORM);
    nux_query_includes(_module.query_transform_staticmesh,
                       NUX_COMPONENT_STATICMESH);

    // Push identity transform
    nux_m4_t identity = nux_m4_identity();
    nux_assert(nux_graphics_push_transforms(
        1, &identity, &_module.identity_transform_offset));
    nux_assert(_module.identity_transform_offset == 0);

    // Create screen rendertarget
    _module.screen_target
        = nux_texture_new(a, NUX_TEXTURE_RENDER_TARGET, 900, 600);
    nux_check(_module.screen_target, return NUX_FAILURE);

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

    nux_assert(_module.free_texture_slots.size == NUX_GPU_TEXTURE_MAX);
    nux_assert(_module.free_buffer_slots.size == NUX_GPU_BUFFER_MAX);
    nux_assert(_module.free_pipeline_slots.size == NUX_GPU_PIPELINE_MAX);
    nux_assert(_module.free_framebuffer_slots.size
               == NUX_GPU_FRAMEBUFFER_MAX - 1); // 0 reserved for default
}
void
nux_graphics_module_register (void)
{
    nux_module_register((nux_module_info_t) { .name = "graphics",
                                              .data = &_module,
                                              .init = module_init,
                                              .free = module_free });
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
    nux_ensure(nux_os_buffer_update(_module.vertices_buffer.slot,
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
    nux_ensure(nux_dsa_push_bottom(&_module.vertices_dsa, count, offset),
               return NUX_FAILURE,
               "out of vertices");
    nux_check(nux_graphics_update_vertices(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_vertices (nux_u32_t        count,
                                  const nux_f32_t *data,
                                  nux_u32_t       *offset)
{
    nux_ensure(nux_dsa_push_top(&_module.vertices_dsa, count, offset),
               return NUX_FAILURE,
               "out of frame vertices");
    nux_check(nux_graphics_update_vertices(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_transforms (nux_u32_t       count,
                              const nux_m4_t *data,
                              nux_u32_t      *offset)
{
    nux_ensure(nux_dsa_push_bottom(&_module.transforms_dsa, count, offset),
               return NUX_FAILURE,
               "out of transforms");
    nux_check(update_transform_buffer(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_frame_transforms (nux_u32_t       count,
                                    const nux_m4_t *data,
                                    nux_u32_t      *offset)
{
    nux_ensure(nux_dsa_push_top(&_module.transforms_dsa, count, offset),
               return NUX_FAILURE,
               "out of frame transforms");
    nux_check(update_transform_buffer(*offset, count, data),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
