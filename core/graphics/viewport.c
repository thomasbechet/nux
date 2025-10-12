#include "internal.h"

nux_viewport_t *
nux_viewport_new (nux_arena_t *arena)
{
    nux_viewport_t *vp = nux_resource_new(arena, NUX_RESOURCE_VIEWPORT);
    NUX_CHECK(vp, return NUX_NULL);
    vp->mode           = NUX_VIEWPORT_STRETCH;
    vp->extent         = nux_v4(0, 0, 1, 1);
    vp->source.camera  = NUX_NULL;
    vp->source.texture = NUX_NULL;
    vp->target         = NUX_NULL;
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
nux_viewport_set_camera (nux_viewport_t *vp, nux_nid_t camera)
{
    vp->source.camera  = camera;
    vp->source.texture = NUX_NULL;
}
void
nux_viewport_set_texture (nux_viewport_t *vp, nux_texture_t *texture)
{
    vp->source.camera  = NUX_NULL;
    vp->source.texture = nux_resource_rid(texture);
}
void
nux_viewport_set_target (nux_viewport_t *vp, nux_texture_t *target)
{
    NUX_ENSURE(target->gpu.type == NUX_TEXTURE_RENDER_TARGET,
               return,
               "viewport target must be a render target texture");
    nux_rid_t rid = nux_resource_rid(target);
    NUX_ENSURE(rid != vp->source.texture,
               return,
               "viewport target must be equals to the source");
    vp->target = rid;
}

static nux_v4_t
compute_extent (nux_viewport_t *viewport)
{
    // get target resolution
    nux_texture_t *target
        = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
    nux_v2u_t target_size;
    if (target)
    {
        target_size = nux_v2u(target->gpu.width, target->gpu.height);
    }
    else
    {
        target_size = nux_v2u(nux_stat(NUX_STAT_SCREEN_WIDTH),
                              nux_stat(NUX_STAT_SCREEN_HEIGHT));
    }
    nux_f32_t target_ratio = (nux_f32_t)target_size.x / target_size.y;

    // get source resolution
    nux_v2u_t source_size;
    if (viewport->source.camera)
    {
    }
    else if (viewport->source.texture)
    {
        nux_texture_t *texture
            = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->source.texture);
        NUX_ASSERT(texture);
        source_size = nux_v2u(texture->gpu.width, texture->gpu.height);
    }
    nux_f32_t source_ratio = (nux_f32_t)source_size.x / source_size.y;

    nux_v2_t vsize = { 0, 0 };
    switch (viewport->mode)
    {
        case NUX_VIEWPORT_FIXED: {
            vsize.x = target_size.x;
            vsize.y = target_size.y;
        };
        break;
        case NUX_VIEWPORT_FIXED_BEST_FIT: {
            nux_f32_t w_factor = global_size.x / target_size.x;
            nux_f32_t h_factor = global_size.y / target_size.y;
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
            vsize.x = target_size.x * min;
            vsize.y = target_size.y * min;
        }
        break;
        case NUX_VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                vsize.x = nux_floor(global_size.y * aspect_ratio);
                vsize.y = nux_floor(global_size.y);
            }
            else
            {
                vsize.x = nux_floor(global_size.x);
                vsize.y = nux_floor(global_size.x / aspect_ratio);
            }
        }
        break;
        case NUX_VIEWPORT_STRETCH:
            vsize = global_size;
            break;
        default:
            break;
    }

    struct nk_vec2 vpos;
    vpos.x = global_pos.x + (global_size.x - vsize.x) * 0.5;
    vpos.y = global_pos.y + (global_size.y - vsize.y) * 0.5;

    // Patch pos (bottom left in opengl)
    vpos.y = window_size.y - (vpos.y + vsize.y);

    runtime.viewport.x = vpos.x;
    runtime.viewport.y = vpos.y;
    runtime.viewport.w = vsize.x;
    runtime.viewport.h = vsize.y;
}
