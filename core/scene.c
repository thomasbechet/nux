#include "internal.h"

#define COMPONENT_TO_INDEX(component) nu_log2(component)

static void
init_node (nux_node_t *node, nux_nid_t nid, nux_nid_t parent)
{
    node->mask   = 0;
    node->flags  = 0;
    node->table  = 0;
    node->parent = parent;
    node->next   = NU_NULL;
    node->prev   = NU_NULL;
    node->child  = NU_NULL;
    node->nid    = nid;
}
static nux_nid_t
add_node (nux_scene_t *scene, nux_scene_node_t *nodes)
{
    nux_u32_t node = NU_NULL;
    if (scene->free)
    {
        node        = scene->free;
        scene->free = nodes[node].free;
    }
    if (!node)
    {
        if (scene->size >= scene->capa)
        {
            return NU_NULL;
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
init_scene_empty (nux_instance_t inst, nux_scene_t *scene)
{
    scene->size             = 1; // first index reserved for NUX_NODE_NULL
    scene->free             = 0;
    nux_scene_node_t *nodes = nux_instance_get_memory(inst, scene->nodes);
    nu_memset(nodes, 0, sizeof(*nodes) * scene->capa);
    nux_nid_t root       = add_node(scene, nodes);
    nux_nid_t root_table = add_node(scene, nodes);
    init_node(&nodes[root].node, NUX_NODE_ROOT, NU_NULL);
}

static nux_status_t
nux_validate_node (const struct nux_env *env, nux_nid_t nid)
{
    if (nid >= env->scene->size || nid < NUX_NODE_ROOT
        || env->nodes[nid].node.nid != nid)
    {
        nux_set_error(env->inst, NUX_ERROR_INVALID_ID);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_create_scene (nux_env_t env, nux_oid_t oid, nux_u32_t node_capa)
{
    nux_object_t *object = nux_object_set(env->inst, oid, NUX_OBJECT_SCENE);
    NU_CHECK(object, return NUX_FAILURE);
    if (node_capa < NUX_NODE_ROOT)
    {
        nux_set_error(env->inst, NUX_ERROR_OUT_OF_NODE);
        return NUX_FAILURE;
    }
    NU_CHECK(nux_malloc(env->inst,
                        sizeof(nux_scene_node_t) * node_capa,
                        &object->scene.nodes),
             return NUX_FAILURE);
    object->scene.capa = node_capa;
    init_scene_empty(env->inst, &object->scene);
    return NUX_SUCCESS;
}

nux_status_t
nux_bind_scene (nux_env_t env, nux_oid_t oid)
{
    nux_object_t *object
        = nux_instance_get_object(env->inst, NUX_OBJECT_SCENE, oid);
    NU_CHECK(object, return NUX_FAILURE);
    env->scene = &object->scene;
    env->nodes = nux_instance_get_memory(env->inst, object->scene.nodes);
    return NUX_SUCCESS;
}
nux_nid_t
nux_node_add (nux_env_t env, nux_nid_t parent)
{
    NU_CHECK(nux_validate_node(env, parent), return NU_NULL);
    NU_CHECK(env->scene, return NU_NULL);
    nux_scene_node_t *nodes = env->nodes;
    nux_nid_t         node  = add_node(env->scene, nodes);
    nux_nid_t         table = add_node(env->scene, nodes);
    NU_CHECK(node && table, return NU_NULL);
    init_node(&nodes[node].node, node, parent);
    nu_memset(&nodes[table].table.indices, 0, sizeof(nux_node_table_t));
    if (parent)
    {
        nux_nid_t child = nodes[parent].node.child;
        if (child)
        {
            nodes[child].node.prev = node;
        }
        nodes[node].node.next    = child;
        nodes[parent].node.child = node;
    }
    return node;
}
void
nux_node_remove (nux_env_t env, nux_nid_t nid)
{
    NU_CHECK(nux_validate_node(env, nid) && nid != NUX_NODE_ROOT, return);
    remove_node(env->scene, env->nodes, env->nodes[nid].node.table);
    remove_node(env->scene, env->nodes, nid);
}

nux_component_t *
nux_node_add_component (nux_env_t            env,
                        nux_nid_t            node,
                        nux_component_type_t component)
{
    NU_CHECK(nux_validate_node(env, node), return NU_NULL);
    if (env->nodes[node].node.mask & component)
    {
        nux_node_remove_component(env, node, component);
    }
    nux_nid_t comp_node = add_node(env->scene, env->nodes);
    if (!comp_node)
    {
        return NU_NULL;
    }
    env->nodes[node].node.mask |= component;
    nux_node_table_t *table = &env->nodes[env->nodes[node].node.table].table;
    table->indices[COMPONENT_TO_INDEX(component)] = comp_node;
    // TODO: init component ?
    return &env->nodes[comp_node].component;
}
nux_status_t
nux_node_remove_component (nux_env_t            env,
                           nux_nid_t            node,
                           nux_component_type_t component)
{
    NU_CHECK(env->scene, return NUX_FAILURE);
    NU_CHECK(nux_validate_node(env, node), return NUX_FAILURE);
    if (env->nodes[node].node.mask & component)
    {
        nux_node_table_t *table
            = &env->nodes[env->nodes[node].node.table].table;
        nu_size_t comp_index = COMPONENT_TO_INDEX(component);
        // TODO: uninit component
        remove_node(env->scene, env->nodes, table->indices[comp_index]);
        table->indices[comp_index] = 0;
    }
    return NUX_SUCCESS;
}
nux_component_t *
nux_node_get_component (nux_env_t            env,
                        nux_nid_t            node,
                        nux_component_type_t component)
{
    NU_CHECK(nux_validate_node(env, node), return NU_NULL);
    return nux_scene_get_component(env->nodes, node, component);
}
void
nux_node_get_translation (nux_env_t env, nux_nid_t nid, nux_f32_t *pos)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(pos, env->nodes[nid].node.translation, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_set_translation (nux_env_t env, nux_nid_t nid, const nux_f32_t *pos)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->nodes[nid].node.translation, pos, sizeof(*pos) * NU_V3_SIZE);
}
void
nux_node_get_rotation (nux_env_t env, nux_nid_t nid, nux_f32_t *rot)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(rot, env->nodes[nid].node.rotation, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_set_rotation (nux_env_t env, nux_nid_t nid, const nux_f32_t *rot)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->nodes[nid].node.rotation, rot, sizeof(*rot) * NU_Q4_SIZE);
}
void
nux_node_get_scale (nux_env_t env, nux_nid_t nid, nux_f32_t *scale)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(scale, env->nodes[nid].node.scale, sizeof(*scale) * NU_V3_SIZE);
}
void
nux_node_set_scale (nux_env_t env, nux_nid_t nid, const nux_f32_t *scale)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(env->nodes[nid].node.scale, scale, sizeof(*scale) * NU_V3_SIZE);
}
nux_u32_t
nux_node_get_parent (nux_env_t env, nux_nid_t nid)
{
    NU_CHECK(nux_validate_node(env, nid), return NU_NULL);
    return env->nodes[nid].node.parent;
}

nux_nid_t
nux_scene_iter_dfs (const nux_scene_node_t *nodes,
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
    for (nux_nid_t child = nodes[current_nid].node.child; child != NU_NULL;
         child           = nodes[child].node.next)
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
nux_component_t *
nux_scene_get_component (nux_scene_node_t    *nodes,
                         nux_nid_t            node,
                         nux_component_type_t component)
{
    if (nodes[node].node.mask & component)
    {
        const nux_node_table_t *table = &nodes[node].table;
        return &nodes[table->indices[COMPONENT_TO_INDEX(component)]].component;
    }
    return NU_NULL;
}
