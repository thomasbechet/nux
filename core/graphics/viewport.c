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
