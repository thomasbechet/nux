#include "internal.h"

void
nux_staticmesh_add (nux_ctx_t *ctx, nux_u32_t entity)
{
    nux_staticmesh_t *sm
        = nux_scene_add_component(ctx, entity, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_ctx_t *ctx, nux_u32_t entity)
{
    nux_scene_remove_component(ctx, entity, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_ctx_t *ctx, nux_u32_t entity, nux_u32_t mesh)
{
    if (mesh)
    {
        NUX_CHECK(nux_object_get(ctx, NUX_OBJECT_MESH, mesh), return);
    }
    nux_staticmesh_t *sm
        = nux_scene_get_component(ctx, entity, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = mesh;
}
