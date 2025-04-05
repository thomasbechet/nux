#include "internal.h"

#define COMPONENT_TO_INDEX(component) nu_log2(component)

static nux_object_type_t component_index_to_object_type[]
    = { NUX_OBJECT_CAMERA, NUX_OBJECT_MODEL };

static nux_id_t
node_add (nux_env_t env, nux_id_t scene, nux_id_t parent)
{
    nux_scene_t *s = nux_object_get(env, scene, NUX_OBJECT_SCENE);
    NU_CHECK(s, return NU_NULL);

    nux_id_t id = nux_pool_add(env, s->pool, NUX_OBJECT_NODE);
    NU_CHECK(id, return NU_NULL);
    nux_node_t *n = nux_object_get_unchecked(env, id);
    n->mask       = 0;
    n->flags      = 0;
    n->table      = 0;
    n->parent     = parent;
    n->next       = NU_NULL;
    n->prev       = NU_NULL;
    n->child      = NU_NULL;

    n->table = nux_pool_add(env, s->pool, NUX_OBJECT_NODE_TABLE);
    NU_CHECK(n->table, return NU_NULL);

    if (parent)
    {
        nux_node_t *p = nux_object_get(env, parent, NUX_OBJECT_NODE);
        NU_CHECK(s, return NU_NULL);
        if (p->scene != scene)
        {
            nux_set_error(env, NUX_ERROR_INVALID_PARENT_NODE);
            return NU_NULL;
        }
        nux_id_t child = p->child;
        if (child)
        {
            nux_node_t *nc = nux_object_get_unchecked(env, child);
            nc->prev       = id;
        }
        n->next  = child;
        p->child = id;
    }

    return id;
}

nux_id_t
nux_create_scene (nux_env_t env, nux_id_t stack, nux_u32_t object_capa)
{
    nux_id_t id
        = nux_stack_push(env, stack, NUX_OBJECT_SCENE, sizeof(nux_scene_t));
    NU_CHECK(id, return NU_NULL);
    nux_scene_t *scene = nux_object_get_unchecked(env, id);
    scene->pool
        = nux_pool_new(env, stack, sizeof(nux_scene_object_t), object_capa);
    NU_CHECK(scene->pool, return NU_NULL);

    scene->root = node_add(env, id, NU_NULL);
    NU_CHECK(scene->root, return NU_NULL);

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
nux_node_add (nux_env_t env, nux_id_t scene, nux_id_t parent)
{
    return node_add(env, scene, parent);
}
nux_id_t
nux_node_add_instance (nux_env_t env,
                       nux_id_t  scene,
                       nux_id_t  parent,
                       nux_id_t  instance)
{
    nux_id_t    id = node_add(env, scene, parent);
    nux_node_t *n  = nux_object_get_unchecked(env, id);
    n->flags |= NUX_NODE_INSTANCED;
    n->instance = scene;
    return id;
}
void
nux_node_remove (nux_env_t env, nux_id_t id)
{
    nux_node_t *n = nux_object_get(env, id, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nux_scene_t *s = nux_object_get_unchecked(env, n->scene);
    nux_pool_remove(env, s->pool, n->table);
    nux_pool_remove(env, s->pool, id);
}

nux_id_t
nux_node_add_component (nux_env_t            env,
                        nux_id_t             node,
                        nux_component_type_t component)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return NU_NULL);
    if (n->mask & component)
    {
        nux_node_remove_component(env, node, component);
    }
    nux_scene_t *s     = nux_object_get_unchecked(env, n->scene);
    nux_u32_t    index = COMPONENT_TO_INDEX(component);
    nux_id_t     id
        = nux_pool_add(env, s->pool, component_index_to_object_type[index]);
    NU_CHECK(id, return NU_NULL);
    n->mask |= component;
    nux_node_table_t *table = nux_object_get_unchecked(env, n->table);
    table->indices[index]   = id;
    // TODO: init component ?
    return component;
}
nux_status_t
nux_node_remove_component (nux_env_t            env,
                           nux_id_t             node,
                           nux_component_type_t component)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return NUX_FAILURE);
    if (n->mask & component)
    {
        nux_u32_t         index = COMPONENT_TO_INDEX(component);
        nux_node_table_t *table = nux_object_get_unchecked(env, n->table);
        nux_scene_t      *s     = nux_object_get_unchecked(env, n->scene);
        // TODO: uninit component
        nux_pool_remove(env, s->pool, table->indices[index]);
        table->indices[index] = NU_NULL;
    }
    return NUX_SUCCESS;
}
void
nux_node_get_translation (nux_env_t env, nux_id_t node, nux_f32_t *pos)
{
    nux_node_t *n = nux_object_get(env, node, NUX_OBJECT_NODE);
    NU_CHECK(n, return);
    nu_memcpy(pos, node->translation, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_set_translation (nux_env_t env, nux_nid_t nid, const nux_f32_t *pos)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->slabs[nid].node.translation, pos, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_get_rotation (nux_env_t env, nux_nid_t nid, nux_f32_t *rot)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(rot, env->slabs[nid].node.rotation, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_set_rotation (nux_env_t env, nux_nid_t nid, const nux_f32_t *rot)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->slabs[nid].node.rotation, rot, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_get_scale (nux_env_t env, nux_nid_t nid, nux_f32_t *scale)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(scale, env->slabs[nid].node.scale, sizeof(*scale) * NU_V3_SIZE);
}
void
nux_node_set_scale (nux_env_t env, nux_nid_t nid, const nux_f32_t *scale)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->slabs[nid].node.scale, scale, sizeof(*scale) * NU_V3_SIZE);
}
nux_u32_t
nux_node_get_parent (nux_env_t env, nux_nid_t nid)
{
    NU_CHECK(nux_validate_node(env, nid), return NU_NULL);
    return env->slabs[nid].node.parent;
}

nux_nid_t
nux_scene_iter_dfs (const nux_scene_slab_t *slabs,
                    nux_nid_t             **iter,
                    nux_nid_t              *stack,
                    nux_u32_t               stack_size)
{
    // Initialization case
    if (!*iter)
    {
        stack[0] = NUX_NODE_ROOT;
        *iter    = &stack[1];
    }

    // Pop current node from stack
    nu_size_t top = (*iter) - stack;
    if (top == 0)
    {
        return NU_NULL; // End of iteration
    }
    --top;
    nux_nid_t current_nid = stack[top];

    // Insert childs
    for (nux_nid_t child = slabs[current_nid].node.child; child != NU_NULL;
         child           = slabs[child].node.next)
    {
        if (top >= stack_size)
        {
            return NU_NULL;
        }
        stack[top] = child;
        ++top;
    }
    *iter = stack + top;

    // Process current node
    return current_nid;
}
// void *
// nux_scene_get_component (nux_scene_slab_t    *slabs,
//                          nux_nid_t            node,
//                          nux_component_type_t component)
// {
//     if (slabs[node].node.mask & component)
//     {
//         const nux_node_table_t *table = &slabs[node].table;
//         return
//         &slabs[table->indices[COMPONENT_TO_INDEX(component)]].component;
//     }
//     return NU_NULL;
// }
