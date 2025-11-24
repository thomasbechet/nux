#include "internal.h"

nux_node_t *
nux_node_root (void)
{
    nux_check(nux_scene_active(), return nullptr);
    return nux_scene_active()->root;
}
nux_node_t *
nux_node_parent (nux_node_t *node)
{
    return nux_object_get(NUX_OBJECT_NODE, node->parent);
}
void
nux_node_set_parent (nux_node_t *node, nux_node_t *parent)
{
    nux_check(node != parent, return);
    node_detach(node);
    node->parent = nux_object_id(parent);
    node->next   = parent->child;
    if (parent->child)
    {
        nux_node_t *child = nux_object_get(NUX_OBJECT_NODE, parent->child);
        child->prev       = nux_object_id(child);
        node->next        = parent->child;
    }
    parent->child = nux_object_id(node);
    parent->prev  = NUX_NULL;
}
nux_node_t *
nux_node_sibling (nux_node_t *node)
{
    return nux_object_get(NUX_OBJECT_NODE, node->next);
}
nux_node_t *
nux_node_child (nux_node_t *node)
{
    return nux_object_get(NUX_OBJECT_NODE, node->child);
}

nux_node_t *
nux_node_new (nux_node_t *parent)
{
    nux_assert(type < nux_array_size(_module.types));
    nux_scene_t *scene = parent->scene;
    nux_arena_t *a     = nux_object_arena(parent);
    nux_node_t  *node  = nux_object_new(a, NUX_OBJECT_NODE);
    node->parent       = nux_object_id(parent);
    node->scene        = scene;
    nux_transform_init(node);
    node->data = nux_malloc(a, t->info.size);
    if (t->info.add)
    {
        t->info.add(node);
    }

    // Append to type chain
    if (scene->nodes[type])
    {
        scene->nodes[type]->prev_same_type = node;
        node->next_same_type               = scene->nodes[type];
    }
    node->prev_same_type = nullptr;

    return node->data;
}
void *
nux_node_get (nux_node_t *node, nux_u32_t type)
{
    nux_object_type_t *t = nux_object_type(type);
    nux_check(t && t->is_component, return nullptr);
    return nux_object_get(type, node->components[t->component_index]);
}
nux_node_t *
nux_node_next (nux_u32_t type, nux_node_t *it)
{
    if (!it)
    {
        nux_assert(type < nux_array_size(_module.types));
        nux_scene_t *scene = nux_scene_active();
        return scene->nodes[type];
    }
    return it->next_same_type;
}
nux_node_t *
nux_node_next_all (nux_node_t *it)
{
    nux_scene_t *scene = nux_scene_active();
    if (!it)
    {
        // return first node from types list
        for (nux_u32_t i = 0; i < nux_array_size(_module.types); ++i)
        {
            if (scene->nodes[i])
            {
                return scene->nodes[i];
            }
        }
        return nullptr;
    }

    if (!it->next_same_type)
    {
        // find next type chain
        for (nux_u32_t i = it->type; i < nux_array_size(_module.types); ++i)
        {
            if (scene->nodes[i])
            {
                return scene->nodes[i];
            }
        }
        return nullptr;
    }

    return it->next_same_type;
}

static nux_status_t
node_clone (nux_scene_t *scene,
            nux_id_t     dst_root,
            nux_id_t     dst_nid,
            nux_id_t     src_nid,
            nux_id_t     parent)
{
    // nux_u32_t index = nux_nid_index(src_nid);
    //
    // // Duplicate components
    // for (nux_u32_t c = 0; c < scene->containers.size; ++c)
    // {
    //     nux_component_t *comp = _module.components + c;
    //     if (scene->containers.data[c].component_size
    //         && bitset_isset(&scene->containers.data[c].bitset, index))
    //     {
    //         void *src = component_get(scene, src_nid, c);
    //         nux_assert(src);
    //         void *dst = component_add(nux_scene_active(), dst_nid, c);
    //         nux_check(dst, return NUX_FAILURE);
    //         nux_memcpy(dst, src, comp->info.size);
    //
    //         // TODO: remap entity references
    //     }
    // }
    //
    // // Duplicate children
    // nux_id_t child = scene->nodes.data[index].child;
    // while (child)
    // {
    //     if (child != dst_root)
    //     {
    //         nux_id_t child_nid = nux_node_create(dst_nid);
    //         nux_check(child_nid, return NUX_FAILURE);
    //         nux_check(node_clone(scene, dst_root, child_nid, child, dst_nid),
    //                   return NUX_FAILURE);
    //     }
    //     child = scene->nodes.data[nux_nid_index(child)].next;
    // }

    return NUX_SUCCESS;
}
nux_node_t *
nux_node_instantiate (nux_scene_t *scene, nux_node_t *parent)
{
    nux_node_t *node = nux_node_new(NUX_NODE_EMPTY, parent);
    return node;
}
