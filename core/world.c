#include "internal.h"

nux_u32_t
nux_world_new (nux_env_t *env)
{
    nux_frame_t frame = nux_frame_begin(env);

    nux_u32_t    id;
    nux_world_t *world = nux_object_add_struct(
        env, NUX_OBJECT_WORLD, sizeof(nux_world_t), &id);
    NUX_CHECK(world, goto error);

    world->arena = env->arena;

    return id;

error:
    nux_frame_reset(env, frame);
    return NUX_NULL;
}
void
nux_world_cleanup (nux_env_t *env, void *data)
{
    nux_world_t *world = data;
}
