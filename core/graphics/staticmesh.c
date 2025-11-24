#include "internal.h"

void
nux_staticmesh_set_mesh (nux_node_t *node, nux_mesh_t *mesh)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return);
    sm->mesh = nux_object_id(mesh);
}
nux_mesh_t *
nux_staticmesh_mesh (nux_node_t *node)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return nullptr);
    return nux_object_check(NUX_OBJECT_MESH, sm->mesh);
}
void
nux_staticmesh_set_texture (nux_node_t *node, nux_texture_t *texture)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return);
    sm->texture = nux_object_id(texture);
}
nux_texture_t *
nux_staticmesh_texture (nux_node_t *node)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return nullptr);
    return nux_object_check(NUX_OBJECT_TEXTURE, sm->texture);
}
void
nux_staticmesh_set_colormap (nux_node_t *node, nux_texture_t *colormap)
{
}
void
nux_staticmesh_set_render_layer (nux_node_t *node, nux_u32_t layer)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return);
    sm->render_layer = layer;
}
nux_u32_t
nux_staticmesh_render_layer (nux_node_t *node)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return 0);
    return sm->render_layer;
}
void
nux_staticmesh_set_draw_bounds (nux_node_t *node, nux_b32_t draw)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    nux_check(sm, return);
    sm->draw_bounds = draw;
}

void
nux_staticmesh_add (nux_node_t *node)
{
    nux_staticmesh_t *sm = nux_node_check(NUX_NODE_STATICMESH, node);
    sm->mesh             = NUX_NULL;
    sm->render_layer     = NUX_LAYER_DEFAULT; // visible in default layer
    sm->draw_bounds      = false;
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
