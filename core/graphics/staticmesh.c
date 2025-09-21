#include "internal.h"

void
nux_staticmesh_add (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_staticmesh_t *sm = nux_ecs_add(ctx, e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_ecs_remove(ctx, e, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_ctx_t *ctx, nux_eid_t e, nux_rid_t mesh)
{
    if (mesh)
    {
        NUX_CHECK(nux_resource_check(ctx, NUX_RESOURCE_MESH, mesh), return);
    }
    nux_staticmesh_t *sm = nux_ecs_get(ctx, e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = mesh;
}
void
nux_staticmesh_set_texture (nux_ctx_t *ctx, nux_eid_t e, nux_rid_t texture)
{
    if (texture)
    {
        NUX_CHECK(nux_resource_check(ctx, NUX_RESOURCE_TEXTURE, texture),
                  return);
    }
    nux_staticmesh_t *sm = nux_ecs_get(ctx, e, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->texture = texture;
}
void
nux_staticmesh_set_colormap (nux_ctx_t *ctx, nux_eid_t e, nux_rid_t colormap)
{
}

nux_status_t
nux_staticmesh_write (nux_serde_writer_t *s,
                      const nux_c8_t     *key,
                      const void         *data)
{
    const nux_staticmesh_t *staticmesh = data;
    nux_serde_write_object(s, key);
    nux_serde_write_u32(s, "mesh", staticmesh->mesh);
    nux_serde_write_u32(s, "texture", staticmesh->texture);
    nux_serde_write_end(s);
    return NUX_SUCCESS;
}
nux_status_t
nux_staticmesh_read (nux_serde_reader_t *s, const nux_c8_t *key, void *data)
{
    nux_staticmesh_t *staticmesh = data;
    nux_serde_read_object(s, key);
    nux_serde_read_u32(s, "mesh", &staticmesh->mesh);
    nux_serde_read_u32(s, "texture", &staticmesh->texture);
    staticmesh->transform = 0;
    nux_serde_read_end(s);
    return NUX_SUCCESS;
}
