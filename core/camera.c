#include "internal.h"

nux_status_t
nux_camera_add (nux_env_t env, nux_nid_t nid)
{
    nux_component_t *component
        = nux_node_add_component(env, nid, NUX_COMPONENT_CAMERA);
    NU_CHECK(component, return NUX_FAILURE);
    component->camera.near = 0.1;
    component->camera.far  = 100;
    component->camera.fov  = nu_radian(60);
    return NUX_SUCCESS;
}
void
nux_camera_remove (nux_env_t env, nux_nid_t nid)
{
    nux_node_remove_component(env, nid, NUX_COMPONENT_CAMERA);
}
void
nux_camera_set_perspective (
    nux_env_t env, nux_nid_t nid, nux_f32_t fov, nux_f32_t near, nux_f32_t far)
{
    nux_component_t *component
        = nux_node_get_component(env, nid, NUX_COMPONENT_CAMERA);
    NU_CHECK(component, return);
    component->camera.fov  = fov;
    component->camera.near = near;
    component->camera.far  = far;
}
