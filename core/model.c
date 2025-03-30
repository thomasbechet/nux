#include "internal.h"

nux_status_t
nux_model_add (nux_env_t env, nux_nid_t nid, nux_oid_t mesh, nux_oid_t texture)
{
    NU_CHECK(nux_instance_get_object(env->inst, NUX_OBJECT_MESH, mesh),
             return NUX_FAILURE);
    // texture can be null and default texture must be used by the renderer
    nux_component_t *component
        = nux_node_add_component(env, nid, NUX_COMPONENT_MODEL);
    NU_CHECK(component, return NUX_FAILURE);
    component->model.mesh    = mesh;
    component->model.texture = texture;
    component->model.visible = NU_TRUE;
    return NUX_SUCCESS;
}
void
nux_model_remove (nux_env_t env, nux_nid_t nid)
{
    nux_node_remove_component(env, nid, NUX_COMPONENT_MODEL);
}
