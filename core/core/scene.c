#include "internal.h"

static void
node_detach (nux_node_t *node)
{
    if (node->parent)
    {
        if (node->prev)
        {
            nux_node_t *prev = nux_object_get(NUX_OBJECT_NODE, node->prev);
            if (prev)
            {
                prev->next = node->next;
            }
            nux_node_t *next = nux_object_get(NUX_OBJECT_NODE, node->next);
            if (next)
            {
                next->prev = node->prev;
            }
        }

        nux_node_t *parent = nux_object_get(NUX_OBJECT_NODE, node->parent);
        if (parent && parent->child == nux_object_id(node))
        {
            parent->child = node->next;
        }
    }
}
static void
node_attach (nux_node_t *node, nux_node_t *parent)
{
}

nux_status_t
nux_scene_init (void)
{
    nux_scene_t *default_scene = nux_scene_new(nux_arena_core());
    nux_core()->default_scene  = default_scene;
    nux_core()->active_scene   = nux_core()->default_scene;
    return NUX_SUCCESS;
}

nux_scene_t *
nux_scene_new (nux_arena_t *arena)
{
    nux_scene_t *scene = nux_object_new(arena, NUX_OBJECT_SCENE);
    nux_check(scene, return nullptr);

    nux_node_t *root = nux_object_new(arena, NUX_OBJECT_NODE);
    nux_memset(root, 0, sizeof(*root));
    nux_transform_init(root);
    root->scene = scene;
    scene->root = root;

    return scene;
}
void
nux_scene_cleanup (void *data)
{
    nux_scene_t *scene = data;
    if (nux_scene_active() == scene && scene != nux_core()->default_scene)
    {
        nux_warning("cleanup active scene, default scene has been set");
        nux_scene_set_active(nullptr);
    }
}
nux_status_t
nux_scene_set_active (nux_scene_t *scene)
{
    if (scene)
    {
        nux_core()->active_scene = scene;
    }
    else
    {
        nux_core()->active_scene = nux_core()->default_scene;
    }
    return NUX_SUCCESS;
}
nux_scene_t *
nux_scene_active (void)
{
    return nux_core()->active_scene;
}
nux_u32_t
nux_scene_count (void)
{
    return 0;
}
void
nux_scene_clear (void)
{
    // TODO
}
