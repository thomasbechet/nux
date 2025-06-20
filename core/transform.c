#include "internal.h"

void
nux_transform_add (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_add_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->position = nux_v3s(0);
    t->rotation = nux_q4_identity();
    t->scale    = nux_v3s(1);
}
void
nux_transform_remove (nux_env_t *env, nux_u32_t entity)
{
    nux_scene_remove_component(env, entity, NUX_COMPONENT_TRANSFORM);
}
nux_v3_t
nux_transform_get_position (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_v3s(0));
    return t->position;
}
nux_v3_t
nux_transform_get_scale (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_v3s(0));
    return t->scale;
}
void
nux_transform_set_position (nux_env_t *env, nux_u32_t entity, nux_v3_t position)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->position = position;
}
