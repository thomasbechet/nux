#include "internal.h"

nux_u32_t
nux_scene_new (nux_env_t *env)
{
    nux_scene_t *s = nux_arena_alloc(env->active_arena, sizeof(*s));
    NUX_CHECK(s, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_SCENE, s);
    NUX_CHECK(id, return NUX_NULL);

    s->arena = env->active_arena;
    NUX_CHECK(nux_entity_pool_alloc(env->active_arena, 1024, &s->entities),
              return NUX_NULL);
    NUX_CHECK(nux_component_pool_alloc(env->active_arena, 1024, &s->components),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_entity_pool_add(&s->entities);
    nux_component_pool_add(&s->components);

    return id;
}
void
nux_scene_draw (nux_env_t *env, nux_u32_t scene)
{
    nux_scene_t *s = nux_arena_alloc(env->active_arena, sizeof(*s));
    NUX_CHECK(s, return);
    for (nux_u32_t ei = 0; ei < s->entities.size; ++ei)
    {
        nux_entity_t *e = s->entities.data + ei;
        if (!e->id)
        {
            continue;
        }
        if (e->components[NUX_COMPONENT_TRANSFORM]
            && e->components[NUX_COMPONENT_STATICMESH])
        {
            nux_staticmesh_t *sm
                = &s->components.data[e->components[NUX_COMPONENT_STATICMESH]]
                       .staticmesh;
            nux_transform_t *t
                = &s->components.data[e->components[NUX_COMPONENT_TRANSFORM]]
                       .transform;
            if (t->dirty)
            {
                nux_transform_update_matrix(env, e->id);
            }
        }
    }
}
nux_u32_t
nux_entity_new (nux_env_t *env, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_entity_t *e = nux_entity_pool_add(&s->entities);
    NUX_CHECK(e, return NUX_NULL);
    nux_u32_t id = nux_object_create(env, s->arena, NUX_OBJECT_ENTITY, e);
    NUX_CHECK(id, return NUX_NULL);
    nux_memset(e, 0, sizeof(*e));
    e->scene = scene;
    e->id    = id;
    return id;
}
void
nux_scene_cleanup (nux_env_t *env, void *data)
{
    nux_scene_t *scene = data;
}
void *
nux_scene_add_component (nux_env_t           *env,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(env, entity, type);
    }
    nux_scene_t     *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
    nux_component_t *c = nux_component_pool_add(&s->components);
    NUX_CHECKM(c, "Out of scene items", return NUX_NULL);
    *comp_index = c - s->components.data;
    return c;
}
void
nux_scene_remove_component (nux_env_t           *env,
                            nux_u32_t            entity,
                            nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        nux_component_pool_remove(&s->components,
                                  &s->components.data[*comp_index]);
    }
    *comp_index = NUX_NULL;
}
void *
nux_scene_get_component (nux_env_t           *env,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        return &s->components.data[*comp_index];
    }
    return NUX_NULL;
}
