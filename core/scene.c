#include "internal.h"

static nux_id_t
node_add (nux_env_t env, nux_id_t parent, nux_id_t object)
{
    nux_scene_t *s
        = nux_object_get(env, nux_node_scene(env, parent), NUX_OBJECT_SCENE);
    NU_CHECK(s, return NU_NULL);

    nux_id_t id = nux_pool_add(env, s->pool, NUX_OBJECT_NODE);
    NU_CHECK(id, return NU_NULL);
    nux_node_t *n = nux_object_get_unchecked(env, id);
    n->flags      = 0;
    n->parent     = parent;
    n->next       = NU_NULL;
    n->prev       = NU_NULL;
    n->child      = NU_NULL;
    n->object     = object;

    n->transform = nux_pool_add(env, s->pool, NUX_OBJECT_NODE_TRANSFORM);
    NU_CHECK(n->transform, return NU_NULL);

    nux_node_t *p = nux_object_get(env, parent, NUX_OBJECT_NODE);
    NU_CHECK(s, return NU_NULL);
    nux_id_t child = p->child;
    if (child)
    {
        nux_node_t *nc = nux_object_get_unchecked(env, child);
        nc->prev       = id;
    }
    n->next  = child;
    p->child = id;

    return id;
}

nux_id_t
nux_scene_create (nux_env_t env, nux_id_t stack, nux_u32_t object_capa)
{
    nux_id_t id
        = nux_stack_push(env, stack, NUX_OBJECT_SCENE, sizeof(nux_scene_t));
    NU_CHECK(id, return NU_NULL);
    nux_scene_t *scene = nux_object_get_unchecked(env, id);
    scene->pool
        = nux_pool_new(env, stack, sizeof(nux_scene_object_t), object_capa);
    NU_CHECK(scene->pool, return NU_NULL);

    scene->root = nux_pool_add(env, scene->pool, NUX_OBJECT_NODE);
    NU_CHECK(id, return NU_NULL);
    nux_node_t *r = nux_object_get_unchecked(env, id);
    r->flags      = 0;
    r->parent     = NU_NULL;
    r->next       = NU_NULL;
    r->prev       = NU_NULL;
    r->child      = NU_NULL;
    r->object     = NU_NULL;
    r->transform  = NU_NULL;

    return id;
}
nux_id_t
nux_node_root (nux_env_t env, nux_id_t scene)
{
    nux_scene_t *s = nux_object_get(env, scene, NUX_OBJECT_SCENE);
    NU_CHECK(s, return NU_NULL);
    return s->root;
}

nux_id_t
nux_node_create (nux_env_t env, nux_id_t parent)
{
    return node_add(env, parent, NU_NULL);
}
nux_id_t
nux_node_create_instance (nux_env_t env, nux_id_t parent, nux_id_t instance)
{
    nux_id_t    id = node_add(env, parent, instance);
    nux_node_t *n  = nux_object_get_unchecked(env, id);
    n->flags |= NUX_NODE_INSTANCED;
    return id;
}
void
nux_node_delete (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_scene_t *s = nux_object_get_unchecked(env, n->scene);
    if (n->flags & NUX_NODE_OBJECT_OWNED)
    {
        nux_pool_remove(env, s->pool, n->object);
    }
    nux_pool_remove(env, s->pool, n->transform);
    nux_pool_remove(env, s->pool, id);
}

void
nux_node_translation (nux_env_t env, nux_id_t node, nux_f32_t *pos)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(pos, t->translation, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_set_translation (nux_env_t env, nux_id_t node, const nux_f32_t *pos)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(t->translation, pos, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_rotation (nux_env_t env, nux_id_t node, nux_f32_t *rot)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(rot, t->rotation, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_set_rotation (nux_env_t env, nux_id_t node, const nux_f32_t *rot)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(t->rotation, rot, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_scale (nux_env_t env, nux_id_t node, nux_f32_t *scale)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(scale, t->scale, sizeof(*scale) * NU_V3_SIZE);
}
void
nux_node_set_scale (nux_env_t env, nux_id_t node, const nux_f32_t *scale)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_node_transform_t *t = nux_object_get_unchecked(env, n->transform);
    nu_memcpy(t->scale, scale, sizeof(*scale) * NU_V3_SIZE);
}
nux_id_t
nux_node_scene (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    return n->scene;
}
nux_id_t
nux_node_parent (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    return n->parent;
}
nux_id_t
nux_node_next (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    return n->next;
}
nux_id_t
nux_node_child (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    return n->child;
}

nux_id_t
nux_create_node_with_object (nux_env_t         env,
                             nux_id_t          parent,
                             nux_object_type_t type)
{
    nux_scene_t *s
        = nux_object_get(env, nux_node_scene(env, parent), NUX_OBJECT_SCENE);
    NU_CHECK(s, return NU_NULL);
    nux_id_t object_id = nux_pool_add(env, s->pool, type);
    NU_CHECK(object_id, return NU_NULL);
    return node_add(env, parent, object_id);
}
nux_id_t
nux_node_object (nux_env_t env, nux_id_t node)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    return n->object;
}
