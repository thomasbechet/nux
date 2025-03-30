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
add_scene_slab (nux_env_t env)
{
    nux_u32_t slab = NU_NULL;
    if (env->scene->free)
    {
        slab             = env->scene->free;
        env->scene->free = env->slabs[slab].free;
    }
    if (!slab)
    {
        if (env->scene->size >= env->scene->capa)
        {
            nux_set_error(env, NUX_ERROR_OUT_OF_SCENE_SLAB);
            return NU_NULL;
        }
        slab = env->scene->size++;
    }
    return slab;
}
static void
remove_scene_slab (nux_scene_t *scene, nux_scene_slab_t *slabs, nux_nid_t slab)
{
    NU_ASSERT(slab);
    slabs[slab].free = scene->free;
    scene->free      = slab;
}
static void
init_scene_empty (nux_env_t env)
{
    env->scene->size = 1; // first index reserved for NUX_NODE_NULL
    env->scene->free = 0;
    nu_memset(env->slabs, 0, sizeof(*env->slabs) * env->scene->capa);
    nux_nid_t root       = add_scene_slab(env);
    nux_nid_t root_table = add_scene_slab(env);
    init_node(&env->slabs[root].node, NUX_NODE_ROOT, NU_NULL);
}

static nux_status_t
nux_validate_node (nux_env_t env, nux_nid_t nid)
{
    if (nid >= env->scene->size || nid < NUX_NODE_ROOT
        || env->slabs[nid].node.nid != nid)
    {
        nux_set_error(env, NUX_ERROR_INVALID_NODE_ID);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_create_scene (nux_env_t env, nux_oid_t oid, nux_u32_t slab_capa)
{
    nux_object_t *object = nux_object_set(env, oid, NUX_OBJECT_SCENE);
    NU_CHECK(object, return NUX_FAILURE);
    if (slab_capa < NUX_NODE_ROOT)
    {
        nux_set_error(env, NUX_ERROR_OUT_OF_SCENE_SLAB);
        return NUX_FAILURE;
    }
    NU_CHECK(nux_malloc(env,
                        sizeof(nux_scene_slab_t) * slab_capa,
                        &object->scene.slabs),
             return NUX_FAILURE);
    object->scene.capa = slab_capa;
    nux_bind_scene(env, oid);
    init_scene_empty(env);
    return NUX_SUCCESS;
}

nux_status_t
nux_bind_scene (nux_env_t env, nux_oid_t oid)
{
    nux_object_t *object = nux_validate_object(env, NUX_OBJECT_SCENE, oid);
    NU_CHECK(object, return NUX_FAILURE);
    env->scene = &object->scene;
    env->slabs = nux_instance_get_memory(env->inst, object->scene.slabs);
    return NUX_SUCCESS;
}
static nux_nid_t
node_add (nux_env_t env, nux_nid_t parent)
{
    nux_nid_t node = add_scene_slab(env);
    NU_CHECK(node, return NU_NULL);
    init_node(&env->slabs[node].node, node, parent);

    if (parent)
    {
        nux_nid_t child = env->slabs[parent].node.child;
        if (child)
        {
            env->slabs[child].node.prev = node;
        }
        env->slabs[node].node.next    = child;
        env->slabs[parent].node.child = node;
    }
    return node;
}
nux_nid_t
nux_node_add (nux_env_t env, nux_nid_t parent)
{
    NU_CHECK(nux_validate_node(env, parent), return NU_NULL);
    NU_CHECK(env->scene, return NU_NULL);
    nux_scene_slab_t *slabs = env->slabs;
    nux_nid_t         node  = node_add(env, parent);
    NU_CHECK(node, return NU_NULL);

    nux_nid_t table = add_scene_slab(env);
    NU_CHECK(table, return NU_NULL);
    nu_memset(&slabs[table].table.indices, 0, sizeof(nux_node_table_t));
    slabs[node].node.table = table;
    slabs[node].node.mask  = 0;

    return node;
}
nux_nid_t
nux_node_add_instance (nux_env_t env, nux_nid_t parent, nux_oid_t scene)
{
    NU_CHECK(nux_validate_node(env, parent), return NU_NULL);
    NU_CHECK(nux_validate_object(env, NUX_OBJECT_SCENE, scene), return NU_NULL);
    NU_CHECK(env->scene, return NU_NULL);
    nux_scene_slab_t *slabs   = env->slabs;
    nux_nid_t         node    = node_add(env, parent);
    slabs[node].node.instance = scene;
    slabs[node].node.flags |= NUX_NODE_INSTANCED;
    return node;
}
void
nux_node_remove (nux_env_t env, nux_nid_t nid)
{
    NU_CHECK(nux_validate_node(env, nid) && nid != NUX_NODE_ROOT, return);
    remove_scene_slab(env->scene, env->slabs, env->slabs[nid].node.table);
    remove_scene_slab(env->scene, env->slabs, nid);
}

nux_component_t *
nux_node_add_component (nux_env_t            env,
                        nux_nid_t            node,
                        nux_component_type_t component)
{
    NU_CHECK(nux_validate_node(env, node), return NU_NULL);
    if (env->slabs[node].node.mask & component)
    {
        nux_node_remove_component(env, node, component);
    }
    nux_nid_t comp_node = add_scene_slab(env);
    NU_CHECK(comp_node, return NU_NULL);
    env->slabs[node].node.mask |= component;
    nux_node_table_t *table = &env->slabs[env->slabs[node].node.table].table;
    table->indices[COMPONENT_TO_INDEX(component)] = comp_node;
    // TODO: init component ?
    return &env->slabs[comp_node].component;
}
nux_status_t
nux_node_remove_component (nux_env_t            env,
                           nux_nid_t            node,
                           nux_component_type_t component)
{
    NU_CHECK(env->scene, return NUX_FAILURE);
    NU_CHECK(nux_validate_node(env, node), return NUX_FAILURE);
    if (env->slabs[node].node.mask & component)
    {
        nux_node_table_t *table
            = &env->slabs[env->slabs[node].node.table].table;
        nu_size_t comp_index = COMPONENT_TO_INDEX(component);
        // TODO: uninit component
        remove_scene_slab(env->scene, env->slabs, table->indices[comp_index]);
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
    return nux_scene_get_component(env->slabs, node, component);
}
void
nux_node_get_translation (nux_env_t env, nux_nid_t nid, nux_f32_t *pos)
{
    NU_CHECK(nux_validate_node(env, nid), return);
    nu_memcpy(pos, env->slabs[nid].node.translation, sizeof(*pos) * NU_V3_SIZE);
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
nux_component_t *
nux_scene_get_component (nux_scene_slab_t    *slabs,
                         nux_nid_t            node,
                         nux_component_type_t component)
{
    if (slabs[node].node.mask & component)
    {
        const nux_node_table_t *table = &slabs[node].table;
        return &slabs[table->indices[COMPONENT_TO_INDEX(component)]].component;
    }
    return NU_NULL;
}
