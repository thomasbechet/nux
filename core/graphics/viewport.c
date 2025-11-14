#include "internal.h"

nux_viewport_t *
nux_viewport_new (nux_arena_t *arena, nux_texture_t *target)
{
    NUX_ENSURE(target->gpu.type == NUX_TEXTURE_RENDER_TARGET,
               return nullptr,
               "viewport target must be a render target texture");
    nux_viewport_t *vp = nux_resource_new(arena, NUX_RESOURCE_VIEWPORT);
    NUX_CHECK(vp, return nullptr);
    vp->mode           = NUX_VIEWPORT_STRETCH_KEEP_ASPECT;
    vp->extent         = nux_b2i(0, 0, target->gpu.width, target->gpu.height);
    vp->anchor         = NUX_ANCHOR_CENTER;
    vp->layer          = 0;
    vp->source.camera  = NUX_NULL;
    vp->source.texture = NUX_NULL;
    vp->target         = nux_resource_rid(target);
    vp->clear_color    = NUX_COLOR_RGBA(0, 0, 0, 1);
    vp->clear_depth    = false;
    vp->auto_resize    = false;
    return vp;
}
void
nux_viewport_set_mode (nux_viewport_t *vp, nux_viewport_mode_t mode)
{
    vp->mode = mode;
}
void
nux_viewport_set_extent (nux_viewport_t *vp, nux_b2i_t extent)
{
    vp->extent = extent;
}
void
nux_viewport_set_anchor (nux_viewport_t *vp, nux_u32_t anchor)
{
    vp->anchor = anchor;
}
void
nux_viewport_set_layer (nux_viewport_t *vp, nux_i32_t layer)
{
    vp->layer = layer;
}
void
nux_viewport_set_clear_depth (nux_viewport_t *vp, nux_b32_t clear)
{
    vp->clear_depth = clear;
}
void
nux_viewport_set_auto_resize (nux_viewport_t *vp, nux_b32_t enable)
{
    vp->auto_resize = enable;
}
void
nux_viewport_set_camera (nux_viewport_t *vp, nux_nid_t camera)
{
    vp->source.camera  = camera;
    vp->source.texture = NUX_NULL;
}
void
nux_viewport_set_texture (nux_viewport_t *vp, nux_texture_t *texture)
{
    NUX_ENSURE(nux_resource_rid(texture) != vp->target,
               return,
               "viewport target must be equals to the source");
    vp->source.camera  = NUX_NULL;
    vp->source.texture = nux_resource_rid(texture);
}
nux_v4_t
nux_viewport_normalized_viewport (nux_viewport_t *viewport)
{
    // Get target resolution
    nux_texture_t *target
        = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
    nux_v2u_t target_size  = nux_v2u(target->gpu.width, target->gpu.height);
    nux_f32_t target_ratio = (nux_f32_t)target_size.x / target_size.y;

    // Get source resolution
    nux_v2u_t source_size;
    if (viewport->source.camera)
    {
        nux_camera_t *cam
            = nux_component_get(viewport->source.camera, NUX_COMPONENT_CAMERA);
        NUX_ASSERT(cam);
        if (cam->aspect != 0)
        {
            source_size.x = nux_floor(target_size.y * cam->aspect);
            source_size.y = nux_floor(target_size.y);
        }
        else
        {
            source_size = target_size;
        }
    }
    else if (viewport->source.texture)
    {
        nux_texture_t *texture
            = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->source.texture);
        NUX_ASSERT(texture);
        source_size = nux_v2u(texture->gpu.width, texture->gpu.height);
    }
    nux_f32_t source_ratio = (nux_f32_t)source_size.x / source_size.y;

    nux_v2u_t vsize = { 0, 0 };
    switch (viewport->mode)
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

    // Compute final pixels coordinates
    nux_v2i_t vpos;
    if (viewport->anchor & NUX_ANCHOR_LEFT)
    {
        vpos.x = 0;
    }
    else if (viewport->anchor & NUX_ANCHOR_RIGHT)
    {
        vpos.x = target->gpu.width - vsize.x;
    }
    else
    {
        vpos.x = ((nux_i32_t)target_size.x - (nux_i32_t)vsize.x) / 2;
    }
    if (viewport->anchor & NUX_ANCHOR_TOP)
    {
        vpos.y = 0;
    }
    else if (viewport->anchor & NUX_ANCHOR_BOTTOM)
    {
        vpos.y = target->gpu.height - vsize.y;
    }
    else
    {
        vpos.y = ((nux_i32_t)target_size.y - (nux_i32_t)vsize.y) / 2;
    }
    vpos.x += viewport->extent.x;
    vpos.y += viewport->extent.y;

    // Compute final extent
    nux_v4_t extent;
    extent.x = (nux_f32_t)vpos.x / target->gpu.width;
    extent.y = (nux_f32_t)vpos.y / target->gpu.height;
    extent.z = (nux_f32_t)vsize.x / target->gpu.width;
    extent.w = (nux_f32_t)vsize.y / target->gpu.height;
    return extent;
}
nux_v2_t
nux_viewport_to_global (nux_viewport_t *vp, nux_v2_t coord)
{
    nux_v4_t extent = nux_viewport_normalized_viewport(vp);
    nux_v2_t global;
    global.x = extent.x + coord.x * extent.z;
    global.y = extent.y + coord.y * extent.w;
    return global;
}
nux_v2_t
nux_viewport_to_local (nux_viewport_t *vp, nux_v2_t coord)
{
    nux_v4_t extent = nux_viewport_normalized_viewport(vp);
    nux_v2_t local;
    local.x = (coord.x - extent.x) / extent.z;
    local.y = (coord.y - extent.y) / extent.w;
    return local;
}
