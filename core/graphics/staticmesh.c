#include "internal.h"

nux_b32_t
nux_staticmesh_has (nux_nid_t n)
{
    return nux_component_get(n, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_add (nux_nid_t e)
{
    nux_staticmesh_t *sm = nux_component_add(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_nid_t e)
{
    nux_node_remove(e, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_nid_t e, nux_mesh_t *mesh)
{
    nux_staticmesh_t *sm = nux_component_get(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = nux_resource_rid(mesh);
}
nux_mesh_t *
nux_staticmesh_get_mesh (nux_nid_t n)
{
    nux_staticmesh_t *sm = nux_component_get(n, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return NUX_NULL);
    return nux_resource_check(NUX_RESOURCE_MESH, sm->mesh);
}
void
nux_staticmesh_set_texture (nux_nid_t e, nux_texture_t *texture)
{
    nux_staticmesh_t *sm = nux_component_get(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->texture = nux_resource_rid(texture);
}
nux_texture_t *
nux_staticmesh_get_texture (nux_nid_t n)
{
    nux_staticmesh_t *sm = nux_component_get(n, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return NUX_NULL);
    return nux_resource_check(NUX_RESOURCE_TEXTURE, sm->texture);
}
void
nux_staticmesh_set_colormap (nux_nid_t e, nux_texture_t *colormap)
{
}
void
nux_staticmesh_set_render_layer (nux_nid_t n, nux_u32_t layer)
{
    nux_staticmesh_t *sm = nux_component_get(n, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->render_layer = layer;
}
nux_u32_t
nux_staticmesh_get_render_layer (nux_nid_t n)
{
    nux_staticmesh_t *sm = nux_component_get(n, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return 0);
    return sm->render_layer;
}

nux_status_t
nux_staticmesh_write (nux_serde_writer_t *s, const void *data)
{
    const nux_staticmesh_t *staticmesh = data;
    nux_serde_write_u32(s, "mesh", staticmesh->mesh);
    nux_serde_write_u32(s, "texture", staticmesh->texture);
    return NUX_SUCCESS;
}
nux_status_t
nux_staticmesh_read (nux_serde_reader_t *s, void *data)
{
    nux_staticmesh_t *staticmesh = data;
    nux_serde_read_u32(s, "mesh", &staticmesh->mesh);
    nux_serde_read_u32(s, "texture", &staticmesh->texture);
    staticmesh->transform = 0;
    return NUX_SUCCESS;
}
