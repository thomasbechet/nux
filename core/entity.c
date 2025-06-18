#include "internal.h"

nux_u32_t
nux_entity_new (nux_env_t *env, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, scene);
    NUX_CHECKM(s, "Invalid scene id", return NUX_NULL);
    nux_entity_t *e = NUX_NULL;
    nux_u32_t     id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_ENTITY, e);
    NUX_CHECK(id, goto error);
    return id;

error:
    nux_object_delete(env, id);
    return NUX_NULL;
}
void
nux_entity_cleanup (nux_env_t *env, void *data)
{
    nux_entity_t *e = data;
}
