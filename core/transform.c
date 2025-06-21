#include "internal.h"

static nux_b32_t
update_global_matrix (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_ASSERT(t);
    if (t->parent && update_global_matrix(env, t->parent))
    {
        t->dirty = NUX_TRUE;
    }

    if (t->dirty)
    {
        t->global_matrix = nux_m4_trs(
            t->local_translation, t->local_rotation, t->local_scale);
        if (t->parent)
        {
            nux_transform_t *parent_t = nux_scene_get_component(
                env, t->parent, NUX_COMPONENT_TRANSFORM);
            NUX_ASSERT(parent_t);
            t->global_matrix
                = nux_m4_mul(parent_t->global_matrix, t->global_matrix);
        }
        t->dirty = NUX_FALSE;
        return NUX_TRUE;
    }
    return NUX_FALSE;
}

void
nux_transform_add (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_add_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = NUX_V3_ZEROES;
    t->local_rotation    = nux_q4_identity();
    t->local_scale       = NUX_V3_ONES;
    t->dirty             = NUX_TRUE;
    t->parent            = NUX_NULL;
}
void
nux_transform_remove (nux_env_t *env, nux_u32_t entity)
{
    nux_scene_remove_component(env, entity, NUX_COMPONENT_TRANSFORM);
}
void
nux_transform_set_parent (nux_env_t *env, nux_u32_t entity, nux_u32_t parent)
{
    NUX_CHECKM(entity != parent, "Setting transform parent to itself", return);
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->parent = parent;
    t->dirty  = NUX_TRUE;
}
nux_u32_t
nux_transform_get_parent (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_NULL);
    return t->parent;
}
nux_v3_t
nux_transform_get_translation (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    return t->local_translation;
}
nux_v3_t
nux_transform_get_scale (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    return t->local_scale;
}
nux_v3_t
nux_transform_get_global_translation (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    update_global_matrix(env, entity);
    nux_v3_t translation;
    nux_m4_trs_decompose(t->global_matrix, &translation, NUX_NULL, NUX_NULL);
    return translation;
}
nux_v3_t
nux_transform_get_global_scale (nux_env_t *env, nux_u32_t entity)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    update_global_matrix(env, entity);
    return t->local_scale;
}
void
nux_transform_set_translation (nux_env_t *env,
                               nux_u32_t  entity,
                               nux_v3_t   position)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = position;
}
void
nux_transform_set_scale (nux_env_t *env, nux_u32_t entity, nux_v3_t scale)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_scale = scale;
}
void
nux_transform_look_at (nux_env_t *env, nux_u32_t entity, nux_v3_t center)
{
    nux_transform_t *t
        = nux_scene_get_component(env, entity, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_v3_t eye     = nux_transform_get_global_translation(env, entity);
    t->global_matrix = nux_lookat(eye, center, NUX_V3_UP);
}
