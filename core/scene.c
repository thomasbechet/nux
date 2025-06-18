#include "internal.h"

nux_u32_t
nux_scene_new (nux_env_t *env)
{
    nux_scene_t *scene = nux_arena_alloc(env->active_arena, sizeof(*scene));
    NUX_CHECK(scene, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(id, return NUX_NULL);

    scene->arena = env->active_arena;

    return id;
}
void
nux_scene_cleanup (nux_env_t *env, void *data)
{
    nux_scene_t *scene = data;
}
