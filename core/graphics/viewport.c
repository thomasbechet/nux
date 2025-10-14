#include "internal.h"

nux_viewport_t *
nux_viewport_new (nux_arena_t *arena, nux_texture_t *target)
{
    NUX_ENSURE(target->gpu.type == NUX_TEXTURE_RENDER_TARGET,
               return NUX_NULL,
               "viewport target must be a render target texture");
    nux_viewport_t *vp = nux_resource_new(arena, NUX_RESOURCE_VIEWPORT);
    NUX_CHECK(vp, return NUX_NULL);
    vp->mode           = NUX_VIEWPORT_STRETCH_KEEP_ASPECT;
    vp->extent         = nux_v4(0, 0, 1, 1);
    vp->anchor         = NUX_ANCHOR_CENTER;
    vp->source.camera  = NUX_NULL;
    vp->source.texture = NUX_NULL;
    vp->target         = nux_resource_rid(target);
    return vp;
}
void
nux_viewport_set_mode (nux_viewport_t *vp, nux_viewport_mode_t mode)
{
    vp->mode = mode;
}
void
nux_viewport_set_extent (nux_viewport_t *vp, nux_v4_t extent)
{
    vp->extent = extent;
}
void
nux_viewport_set_anchor (nux_viewport_t *vp, nux_u32_t anchor)
{
    vp->anchor = anchor;
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
nux_viewport_get_render_extent (nux_viewport_t *viewport)
{
    nux_v2_t extent_pos  = nux_v2(viewport->extent.x, viewport->extent.y);
    nux_v2_t extent_size = nux_v2(viewport->extent.z, viewport->extent.w);

    // Get target resolution
    nux_texture_t *target
        = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
    nux_v2u_t target_size;
    target_size.x          = target->gpu.width * extent_size.x;
    target_size.y          = target->gpu.height * extent_size.y;
    nux_f32_t target_ratio = (nux_f32_t)target_size.x / target_size.y;

    // Get source resolution
    nux_v2u_t source_size;
    if (viewport->source.camera)
    {
        nux_camera_t *cam
            = nux_component_get(viewport->source.camera, NUX_COMPONENT_CAMERA);
        NUX_ASSERT(cam);
        if (cam->ratio != 0)
        {
            source_size.x = nux_floor(target_size.y * cam->ratio);
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
        case NUX_VIEWPORT_FIXED: {
            vsize.x = source_size.x;
            vsize.y = source_size.y;
        };
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
        case NUX_VIEWPORT_STRETCH_KEEP_ASPECT: {
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
        }
        break;
        case NUX_VIEWPORT_STRETCH:
            vsize = target_size;
            break;
        default:
            break;
    }

    // Compute final pixels coordinates
    nux_v2u_t vpos;
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
        vpos.x = (target_size.x - vsize.x) * 0.5;
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
        vpos.y = (target_size.y - vsize.y) * 0.5;
    }
    vpos.x += target->gpu.width * extent_pos.x;
    vpos.y += target->gpu.height * extent_pos.y;

    // Compute final extent
    nux_v4_t extent;
    extent.x = (nux_f32_t)vpos.x / target->gpu.width;
    extent.y = (nux_f32_t)vpos.y / target->gpu.height;
    extent.z = (nux_f32_t)vsize.x / target->gpu.width;
    extent.w = (nux_f32_t)vsize.y / target->gpu.height;
    return extent;
}
