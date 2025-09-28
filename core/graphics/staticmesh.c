#include "internal.h"

void
nux_staticmesh_add (nux_eid_t e)
{
    nux_staticmesh_t *sm = nux_ecs_add(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_eid_t e)
{
    nux_ecs_remove(e, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_eid_t e, nux_mesh_t *mesh)
{
    nux_staticmesh_t *sm = nux_ecs_get(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = nux_resource_rid(mesh);
}
void
nux_staticmesh_set_texture (nux_eid_t e, nux_texture_t *texture)
{
    nux_staticmesh_t *sm = nux_ecs_get(e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->texture = nux_resource_rid(texture);
}
void
nux_staticmesh_set_colormap (nux_eid_t e, nux_texture_t *colormap)
{
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
