#include "internal.h"

nux_id_t
nux_camera_create (nux_env_t env, nux_id_t parent)
{
    nux_id_t id
        = nux_create_node_with_object(env, parent, NUX_OBJECT_NODE_CAMERA);
    NU_CHECK(id, return NU_NULL);
    nux_camera_t *c = nux_object_get_unchecked(env, nux_node_object(env, id));
    c->near         = 0.1;
    c->far          = 100;
    c->fov          = nu_radian(60);
    return id;
}
void
nux_camera_set_perspective (
    nux_env_t env, nux_id_t id, nux_f32_t fov, nux_f32_t near, nux_f32_t far)
{
    nux_camera_t *c
        = nux_object_get(env, nux_node_object(env, id), NUX_OBJECT_NODE_CAMERA);
    NU_CHECK(c, return);
    c->fov  = fov;
    c->near = near;
    c->far  = far;
}
