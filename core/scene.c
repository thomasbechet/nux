#include "internal.h"

#define COMPONENT_TO_INDEX(component) nu_log2(component)

nu_bool_t
nux_validate_node (nux_nid_t nid)
{
    return nid >= NUX_NODE_ROOT && nid < NUX_NODE_MAX;
}
static nux_scene_t *
active_scene (nux_env_t env)
{
    nux_object_t *object = nux_instance_get_object(
        env->inst, NUX_OBJECT_SCENE, env->active_scene);
    return object ? &object->scene : NU_NULL;
}
static void
init_node (nux_node_t *node, nux_nid_t parent)
{
    node->mask   = 0;
    node->flags  = 0;
    node->table  = 0;
    node->parent = parent;
    node->next   = NUX_NULL;
    node->prev   = NUX_NULL;
    node->child  = NUX_NULL;
}
static nux_nid_t
add_node (nux_scene_t *scene, nux_scene_node_t *nodes)
{
    nux_u32_t node = NUX_NULL;
    if (scene->free)
    {
        node        = scene->free;
        scene->free = nodes[node].free;
    }
    if (!node)
    {
        if (scene->size >= scene->capa)
        {
            return NUX_NULL;
        }
        node = scene->size++;
    }
    return node;
}
static void
remove_node (nux_scene_t *scene, nux_scene_node_t *nodes, nux_nid_t node)
{
    NU_ASSERT(node);
    nodes[node].free = scene->free;
    scene->free      = node;
}
static void
nux_scene_init_empty (nux_instance_t inst,
                      nux_scene_t   *scene,
                      nux_u32_t      node_capa)
{
    NU_ASSERT(node_capa >= NUX_NODE_ROOT);
    scene->capa             = node_capa;
    scene->size             = 1; // first index reserved for NUX_NODE_NULL
    scene->free             = 0;
    nux_scene_node_t *nodes = nux_instance_get_memory(inst, scene->addr);
    nu_memset(nodes, 0, sizeof(*nodes) * scene->capa);
    init_node(&nodes[NUX_NODE_ROOT].node, NUX_NULL);
}

void
nux_bind_scene (nux_env_t env, nux_oid_t oid)
{
    NU_CHECK(nux_instance_get_object(env->inst, NUX_OBJECT_SCENE, oid), return);
    env->active_scene = oid;
}
nux_nid_t
nux_node_add (nux_env_t env, nux_nid_t parent)
{
    nux_scene_t *scene = active_scene(env);
    NU_CHECK(parent && scene, return NUX_NULL);
    nux_scene_node_t *nodes = nux_instance_get_memory(env->inst, scene->addr);
    nux_nid_t         node  = add_node(scene, nodes);
    nux_nid_t         table = add_node(scene, nodes);
    NU_CHECK(node && table, return NUX_NULL);
    init_node(&nodes[node].node, parent);
    nu_memset(&nodes[table].table.indices, 0, sizeof(nux_node_table_t));
    if (parent)
    {
    }
    return node;
}
void
nux_node_remove (nux_env_t env, nux_nid_t nid)
{
    nux_scene_t *scene = active_scene(env);
    NU_CHECK(scene && nux_validate_node(nid) && nid != NUX_NODE_ROOT, return);
    nux_scene_node_t *nodes = nux_instance_get_memory(env->inst, scene->addr);
    remove_node(scene, nodes, nodes[nid].node.table);
    remove_node(scene, nodes, nid);
}

nux_error_t
nux_node_add_component (nux_env_t            env,
                        nux_nid_t            node,
                        nux_component_type_t component)
{
    nux_scene_t *scene = active_scene(env);
    NU_CHECK(scene && nux_validate_node(node), return NUX_ERROR_INVALID_ID);
    nux_scene_node_t *nodes = nux_instance_get_memory(env->inst, scene->addr);
    if (nodes[node].node.mask & component)
    {
        nux_node_remove_component(env, node, component);
    }
    nu_u16_t comp_node = add_node(scene, nodes);
    if (!comp_node)
    {
        return NUX_ERROR_OUT_OF_NODE;
    }
    nodes[node].node.mask |= component;
    nux_node_table_t *table = &nodes[nodes[node].node.table].table;
    table->indices[COMPONENT_TO_INDEX(component)] = comp_node;
    // TODO: init component
    return NUX_ERROR_NONE;
}
nux_error_t
nux_node_remove_component (nux_env_t            env,
                           nux_nid_t            node,
                           nux_component_type_t component)
{
    nux_scene_t *scene = active_scene(env);
    NU_CHECK(scene && nux_validate_node(node), return NUX_ERROR_INVALID_ID);
    nux_scene_node_t *nodes = nux_instance_get_memory(env->inst, scene->addr);
    if (nodes[node].node.mask & component)
    {
        nux_node_table_t *table      = &nodes[nodes[node].node.table].table;
        nu_size_t         comp_index = COMPONENT_TO_INDEX(component);
        // TODO: uninit component
        remove_node(scene, nodes, table->indices[comp_index]);
        table->indices[comp_index] = 0;
    }
    return NUX_ERROR_NONE;
}
nux_component_t *
nux_node_get_component (nux_env_t            env,
                        nux_nid_t            node,
                        nux_component_type_t component)
{
    nux_scene_t *scene = active_scene(env);
    NU_CHECK(scene && nux_validate_node(node), return NU_NULL);
    nux_scene_node_t *nodes = nux_instance_get_memory(env->inst, scene->addr);
    if (nodes[node].node.mask & component)
    {
        nux_node_table_t *table = &nodes[node].table;
        return &nodes[table->indices[COMPONENT_TO_INDEX(component)]].component;
    }
    return NU_NULL;
}
