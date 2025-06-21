#include "internal.h"

void
nux_staticmesh_add (nux_env_t *env, nux_u32_t entity)
{
    nux_staticmesh_t *sm
        = nux_scene_add_component(env, entity, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = NUX_NULL;
}
void
nux_staticmesh_remove (nux_env_t *env, nux_u32_t entity)
{
    nux_scene_remove_component(env, entity, NUX_COMPONENT_STATICMESH);
}
void
nux_staticmesh_set_mesh (nux_env_t *env, nux_u32_t entity, nux_u32_t mesh)
{
    if (mesh)
    {
        NUX_CHECK(nux_object_get(env, NUX_OBJECT_MESH, mesh), return);
    }
    nux_staticmesh_t *sm
        = nux_scene_get_component(env, entity, NUX_COMPONENT_STATICMESH);
    NUX_CHECK(sm, return);
    sm->mesh = mesh;
}
