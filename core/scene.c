#include "internal.h"

nux_u32_t
nux_scene_new (nux_env_t *env)
{
    nux_frame_t frame = nux_frame_begin(env);

    nux_u32_t    id;
    nux_scene_t *scene = nux_object_add_struct(
        env, NUX_OBJECT_SCENE, sizeof(nux_scene_t), &id);
    NUX_CHECK(scene, goto error);

    scene->arena = env->arena;

    return id;

error:
    nux_frame_reset(env, frame);
    return NUX_NULL;
}
void
nux_scene_cleanup (nux_env_t *env, void *data)
{
    nux_scene_t *scene = data;
}
