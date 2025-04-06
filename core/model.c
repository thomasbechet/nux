#include "internal.h"

nux_status_t
nux_create_model (nux_env_t env,
                  nux_id_t  parent,
                  nux_id_t  mesh,
                  nux_id_t  texture)
{
    nux_id_t id
        = nux_create_node_with_object(env, parent, NUX_OBJECT_NODE_MODEL);
    NU_CHECK(id, return NU_NULL);
    nux_model_t *m = nux_object_get_unchecked(env, nux_node_object(env, id));
    // texture can be null and default texture must be used by the renderer
    m->mesh    = mesh;
    m->texture = texture;
    m->visible = NU_TRUE;
    return id;
}
