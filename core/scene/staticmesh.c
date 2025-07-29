#include "internal.h"

void
nux_staticmesh_add (nux_ctx_t *ctx, nux_res_t node)
{
    nux_staticmesh_t *sm
        = nux_scene_add_component(ctx, node, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_ctx_t *ctx, nux_res_t node)
{
    nux_scene_remove_component(ctx, node, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_ctx_t *ctx, nux_res_t node, nux_res_t mesh)
{
    if (mesh)
    {
        NUX_CHECK(nux_res_check(ctx, NUX_RES_MESH, mesh), return);
    }
    nux_staticmesh_t *sm
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = mesh;
}
void
nux_staticmesh_set_texture (nux_ctx_t *ctx, nux_res_t node, nux_res_t texture)
{
    if (texture)
    {
        NUX_CHECK(nux_res_check(ctx, NUX_RES_TEXTURE, texture), return);
    }
    nux_staticmesh_t *sm
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->texture = texture;
}
void
nux_staticmesh_set_colormap (nux_ctx_t *ctx, nux_res_t node, nux_res_t colormap)
{
}
