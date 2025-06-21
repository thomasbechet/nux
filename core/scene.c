#include "internal.h"

static nux_u32_t *
entity_component_index (nux_entity_t *e, nux_component_type_t type)
{
    switch (type)
    {
        case NUX_COMPONENT_TRANSFORM:
            return &e->transform_index;
        case NUX_COMPONENT_CAMERA:
            return &e->camera_index;
        case NUX_COMPONENT_STATICMESH:
            return &e->staticmesh_index;
        default:
            break;
    }
    return NUX_NULL;
}

nux_u32_t
nux_scene_new (nux_env_t *env)
{
    nux_scene_t *s = nux_arena_alloc(env->active_arena, sizeof(*s));
    NUX_CHECK(s, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_SCENE, s);
    NUX_CHECK(id, return NUX_NULL);

    s->arena = env->active_arena;
    NUX_CHECK(nux_scene_item_pool_alloc(env->active_arena, 1024, &s->items),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_scene_item_pool_add(&s->items);

    return id;
}
nux_u32_t
nux_entity_new (nux_env_t *env, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_scene_item_t *i = nux_scene_item_pool_add(&s->items);
    NUX_CHECK(i, return NUX_NULL);
    nux_entity_t *e = &i->entity;
    nux_memset(e, 0, sizeof(*e));
    e->scene     = scene;
    nux_u32_t id = nux_object_create(env, s->arena, NUX_OBJECT_ENTITY, e);
    NUX_CHECK(id, return NUX_NULL);
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
    nux_u32_t *comp_index = entity_component_index(e, type);
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(env, entity, type);
    }
    nux_scene_t      *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
    nux_scene_item_t *i = nux_scene_item_pool_add(&s->items);
    NUX_CHECKM(i, "Out of scene items", return NUX_NULL);
    *comp_index = i - &s->items.items->data;
    return i;
}
void
nux_scene_remove_component (nux_env_t           *env,
                            nux_u32_t            entity,
                            nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return);
    nux_u32_t *comp_index = entity_component_index(e, type);
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        nux_scene_item_pool_remove(&s->items,
                                   &s->items.items[*comp_index].data);
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
    nux_u32_t *comp_index = entity_component_index(e, type);
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        return &s->items.items[*comp_index].data;
    }
    return NUX_NULL;
}
