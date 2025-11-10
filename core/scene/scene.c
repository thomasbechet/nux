#include "internal.h"

static nux_scene_module_t _module;

NUX_VEC_IMPL(nux_scene_bitset, nux_scene_mask_t);
NUX_VEC_IMPL(nux_scene_chunk_vec, void *);
NUX_VEC_IMPL(nux_scene_container_vec, nux_scene_container_t);
NUX_VEC_IMPL(nux_node_vec, nux_node_t);

static void
mask_set (nux_scene_mask_t *mask, nux_u32_t n)
{
    *mask = *mask | (1 << n);
}
static void
mask_unset (nux_scene_mask_t *mask, nux_u32_t n)
{
    *mask = *mask & ~(1 << n);
}
static nux_b32_t
mask_isset (nux_scene_mask_t mask, nux_u32_t n)
{
    return (mask >> n) & 1;
}
static nux_u32_t
mask_count (nux_scene_mask_t mask)
{
    nux_u32_t c;
    for (c = 0; mask; mask >>= 1)
    {
        c += mask & 1;
    }
    return c;
}
static nux_b32_t
bitset_isset (const nux_scene_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / NUX_NODE_PER_MASK;
    nux_u32_t offset = index % NUX_NODE_PER_MASK;
    return (mask < bitset->size && mask_isset(bitset->data[mask], offset));
}
static nux_status_t
bitset_set (nux_scene_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / NUX_NODE_PER_MASK;
    nux_u32_t offset = index % NUX_NODE_PER_MASK;
    while (bitset->size <= mask)
    {
        if (!nux_scene_bitset_pushv(bitset, 0x0))
        {
            return NUX_FAILURE;
        }
    }
    mask_set(&bitset->data[mask], offset);
    return NUX_SUCCESS;
}
static nux_b32_t
bitset_unset (nux_scene_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / NUX_NODE_PER_MASK;
    nux_u32_t offset = index % NUX_NODE_PER_MASK;
    if (bitset->size > mask)
    {
        mask_unset(&bitset->data[mask], offset);
        return NUX_TRUE;
    }
    return NUX_FALSE;
}
static nux_scene_mask_t
bitset_mask (const nux_scene_bitset_t *bitset, nux_u32_t mask_index)
{
    if (mask_index >= bitset->size)
    {
        return 0;
    }
    return bitset->data[mask_index];
}
static nux_u32_t
bitset_count (const nux_scene_bitset_t *bitset)
{
    nux_u32_t n = 0;
    for (nux_u32_t i = 0; i < bitset->size; ++i)
    {
        n += mask_count(bitset->data[i]);
    }
    return n;
}
static nux_b32_t
bitset_find_unset (const nux_scene_bitset_t *bitset, nux_u32_t *found)
{
    for (nux_u32_t i = 0; i < bitset->size; ++i)
    {
        nux_scene_mask_t mask = bitset->data[i];
        if (bitset->data[i] != 0xFFFFFFFF)
        {
            nux_u32_t index = 0;
            while (mask & 0x1)
            {
                mask >>= 1;
                ++index;
            }
            *found = i * NUX_NODE_PER_MASK + index;
            return NUX_TRUE;
        }
    }
    return NUX_FALSE;
}
static nux_b32_t
node_valid (const nux_scene_t *scene, nux_nid_t e)
{
    nux_u32_t index = NUX_NID_INDEX(e);
    return bitset_isset(&scene->bitset, index);
}
static nux_node_t *
node_check (const nux_scene_t *scene, nux_nid_t e)
{
    NUX_ENSURE(e, return NUX_NULL, "invalid null nid", e);
    nux_u32_t index = NUX_NID_INDEX(e);
    NUX_ENSURE(bitset_isset(&scene->bitset, index),
               return NUX_NULL,
               "invalid nid 0x%X",
               e);
    return scene->nodes.data + index;
}
static void
node_detach (nux_node_t *nodes, nux_nid_t e)
{
    nux_u32_t ei = NUX_NID_INDEX(e);
    if (nodes[ei].parent)
    {
        if (nodes[ei].prev)
        {
            nodes[NUX_NID_INDEX(nodes[ei].prev)].next = nodes[ei].next;
        }
        if (nodes[ei].next)
        {
            nodes[NUX_NID_INDEX(nodes[ei].next)].prev = nodes[ei].prev;
        }
        nux_node_t *parent = nodes + nodes[ei].parent;
        if (parent->child == e)
        {
            parent->child = nodes[ei].next;
        }
    }
}
static void
node_attach (nux_node_t *entities, nux_nid_t e, nux_nid_t p)
{
    NUX_CHECK(e != p, return);
    node_detach(entities, e);
    nux_node_t *entity = entities + NUX_NID_INDEX(e);
    nux_node_t *parent = entities + NUX_NID_INDEX(p);
    entity->parent     = p;
    entity->next       = entity->child;
    if (parent->child)
    {
        nux_node_t *child = entities + NUX_NID_INDEX(parent->child);
        child->prev       = e;
        entity->next      = parent->child;
    }
    parent->child = e;
    parent->prev  = NUX_NULL;
}

static nux_status_t
module_init (void)
{
    // Register types
    nux_register_resource(NUX_RESOURCE_SCENE,
                          (nux_resource_info_t) {
                              .name    = "scene",
                              .size    = sizeof(nux_scene_t),
                              .cleanup = nux_scene_cleanup,
                          });
    nux_register_resource(NUX_RESOURCE_QUERY,
                          (nux_resource_info_t) {
                              .name = "query",
                              .size = sizeof(nux_query_t),
                          });

    // Initialize values
    nux_memset(_module.components, 0, sizeof(_module.components));
    _module.components_max     = 0;
    nux_scene_t *default_scene = nux_scene_new(nux_core_arena());
    NUX_CHECK(default_scene, return NUX_FAILURE);
    _module.default_scene = default_scene;
    _module.active        = _module.default_scene;

    // Register components
    nux_register_component(
        NUX_COMPONENT_TRANSFORM, "transform", sizeof(nux_transform_t));
    nux_component_set_add(NUX_COMPONENT_TRANSFORM, nux_transform_add);
    nux_component_set_write(NUX_COMPONENT_TRANSFORM, nux_transform_write);
    nux_component_set_read(NUX_COMPONENT_TRANSFORM, nux_transform_read);

    return NUX_SUCCESS;
}
void
nux_scene_module_register (void)
{
    NUX_REGISTER_MODULE("scene", &_module, module_init, NUX_NULL);
}
nux_scene_module_t *
nux_scene_module (void)
{
    return &_module;
}

nux_query_t *
nux_query_new (nux_arena_t *arena,
               nux_u32_t    include_count,
               nux_u32_t    exclude_count)
{
    nux_query_t *it = nux_new_resource(arena, NUX_RESOURCE_QUERY);
    NUX_CHECK(it, return NUX_NULL);
    if (include_count)
    {
        NUX_CHECK(nux_u32_vec_init_capa(arena, include_count, &it->includes),
                  return NUX_NULL);
    }
    if (exclude_count)
    {
        NUX_CHECK(nux_u32_vec_init_capa(arena, exclude_count, &it->excludes),
                  return NUX_NULL);
    }
    return it;
}
nux_query_t *
nux_query_new_any (nux_arena_t *arena)
{
    return nux_query_new(arena, 0, 0);
}
void
nux_query_includes (nux_query_t *it, nux_u32_t c)
{
    nux_u32_vec_pushv(&it->includes, c);
}
void
nux_query_excludes (nux_query_t *it, nux_u32_t c)
{
    nux_u32_vec_pushv(&it->excludes, c);
}
static nux_nid_t
query_next (const nux_scene_t *ins, nux_query_t *it)
{
    // find next non empty mask
    while (!it->mask)
    {
        ++it->mask_index;
        const nux_u32_t max_mask_count = ins->bitset.size;
        if (it->mask_index >= max_mask_count)
        {
            // reach last mask
            return NUX_NULL;
        }

        // initialize mask with existing entities
        it->mask = bitset_mask(&ins->bitset, it->mask_index);
        // filter on required components
        for (nux_u32_t i = 0; i < it->includes.size; ++i)
        {
            nux_u32_t index = it->includes.data[i];
            if (ins->containers.data && index < ins->containers.size)
            {
                nux_scene_container_t *container = ins->containers.data + index;
                it->mask &= bitset_mask(&container->bitset, it->mask_index);
            }
            else
            {
                return NUX_NULL; // missing container, no matching entities
            }
        }
        // must be processed after includes
        for (nux_u32_t i = 0; i < it->excludes.size; ++i)
        {
            nux_u32_t index = it->excludes.data[i];
            if (ins->containers.data && index < ins->containers.size)
            {
                nux_scene_container_t *container = ins->containers.data + index;
                it->mask &= ~bitset_mask(&container->bitset, it->mask_index);
            }
        }

        it->mask_offset = 0;
    }

    // consume mask (at this step, the mask has at least one bit set)
    NUX_ASSERT(it->mask);
    while (!(it->mask & 1))
    {
        it->mask >>= 1;
        ++it->mask_offset;
    }
    it->mask >>= 1;
    nux_u32_t offset = it->mask_offset++;
    return NUX_NID_MAKE(it->mask_index * NUX_NODE_PER_MASK + offset);
}
nux_u32_t
nux_query_next (nux_query_t *it, nux_nid_t e)
{
    if (!e) // initialize iterator
    {
        it->scene       = _module.active;
        it->mask        = 0;
        it->mask_offset = 0;
        it->mask_index  = (nux_u32_t)-1; // trick for first iteration
    }
    return query_next(it->scene, it);
}

nux_scene_t *
nux_scene_new (nux_arena_t *arena)
{
    nux_scene_t *scene = nux_new_resource(arena, NUX_RESOURCE_SCENE);
    NUX_CHECK(scene, return NUX_NULL);
    scene->arena = arena;
    NUX_CHECK(nux_scene_container_vec_init_capa(
                  arena, _module.components_max, &scene->containers),
              return NUX_NULL);
    NUX_CHECK(nux_scene_bitset_init(arena, &scene->bitset), return NUX_NULL);
    NUX_CHECK(nux_node_vec_init(arena, &scene->nodes), return NUX_NULL);
    // create root node
    nux_node_t *root = nux_node_vec_push(&scene->nodes);
    NUX_CHECK(root, return NUX_NULL);
    nux_memset(root, 0, sizeof(*root));
    NUX_CHECK(bitset_set(&scene->bitset, 0), return NUX_NULL);
    scene->root = NUX_NID_MAKE(0);
    return scene;
}
void
nux_scene_cleanup (void *data)
{
    nux_scene_t *scene = data;
    if (_module.active == scene && scene != _module.default_scene)
    {
        NUX_WARNING("cleanup active scene, default scene has been set");
        nux_scene_set_active(NUX_NULL);
    }
}
nux_status_t
nux_scene_set_active (nux_scene_t *scene)
{
    if (scene)
    {
        _module.active = scene;
    }
    else
    {
        _module.active = _module.default_scene;
    }
    return NUX_SUCCESS;
}
nux_scene_t *
nux_scene_active (void)
{
    return _module.active;
}
nux_u32_t
nux_scene_count (void)
{
    nux_scene_t *scene = nux_scene_active();
    return bitset_count(&scene->bitset);
}
void
nux_scene_clear (void)
{
    nux_scene_t *scene = nux_scene_active();
    for (nux_u32_t i = 0; i < scene->containers.size; ++i)
    {
        nux_scene_container_t *container = scene->containers.data + i;
        nux_scene_bitset_clear(&container->bitset);
    }
    nux_scene_bitset_clear(&scene->bitset);
}

nux_nid_t
nux_node_create (nux_nid_t parent)
{
    nux_scene_t *scene = _module.active;
    NUX_CHECK(scene, return NUX_NULL);
    NUX_CHECK(node_check(scene, parent), return NUX_NULL);
    nux_u32_t index;
    if (!bitset_find_unset(&scene->bitset, &index))
    {
        index = scene->bitset.size * NUX_NODE_PER_MASK;
    }
    NUX_CHECK(bitset_set(&scene->bitset, index), return NUX_NULL);
    while (index >= scene->nodes.size)
    {
        // ensure growth factor with resize
        NUX_CHECK(nux_node_vec_push(&scene->nodes), return NUX_NULL);
    }
    nux_nid_t nid = NUX_NID_MAKE(index);
    nux_memset(&scene->nodes.data[index], 0, sizeof(nux_node_t));
    node_attach(scene->nodes.data, nid, parent);
    return nid;
}
void
nux_node_delete (nux_nid_t e)
{
    nux_scene_t *scene = nux_scene_active();
    nux_u32_t    index = NUX_NID_INDEX(e);

    // cannot remove root node
    NUX_CHECK(e != scene->root, return);

    // remove from components
    for (nux_u32_t i = 0; i < scene->containers.size; ++i)
    {
        nux_node_remove(e, NUX_NID_MAKE(i));
    }

    // mask node as invalid
    if (bitset_unset(&scene->bitset, index))
    {
        node_detach(scene->nodes.data, e);
    }
}
nux_b32_t
nux_node_valid (nux_nid_t e)
{
    nux_scene_t *scene = nux_scene_active();
    return node_valid(scene, e);
}
nux_nid_t
nux_node_root (void)
{
    return NUX_NID_MAKE(0);
}
nux_nid_t
nux_node_parent (nux_nid_t e)
{
    nux_scene_t *scene = nux_scene_active();
    NUX_CHECK(node_check(scene, e), return NUX_NULL);
    return scene->nodes.data[NUX_NID_INDEX(e)].parent;
}
void
nux_node_set_parent (nux_nid_t e, nux_nid_t p)
{
    nux_scene_t *scene = nux_scene_active();
    NUX_CHECK(node_check(scene, e) && node_check(scene, p), return);
    node_attach(scene->nodes.data, e, p);
}
nux_nid_t
nux_node_sibling (nux_nid_t e)
{
    nux_scene_t *scene = nux_scene_active();
    NUX_CHECK(node_check(scene, e), return NUX_NULL);
    return scene->nodes.data[NUX_NID_INDEX(e)].next;
}
nux_nid_t
nux_node_child (nux_nid_t e)
{
    nux_scene_t *scene = nux_scene_active();
    NUX_CHECK(node_check(scene, e), return NUX_NULL);
    return scene->nodes.data[NUX_NID_INDEX(e)].child;
}
static void *
component_get (const nux_scene_t *scene, nux_nid_t e, nux_u32_t c)
{
    nux_u32_t              index     = NUX_NID_INDEX(e);
    nux_u32_t              mask      = index / NUX_NODE_PER_MASK;
    nux_u32_t              offset    = index % NUX_NODE_PER_MASK;
    nux_scene_container_t *container = scene->containers.data + c;
    return (void *)((nux_intptr_t)container->chunks.data[mask]
                    + container->component_size * offset);
}
void
nux_register_component (nux_u32_t index, const nux_c8_t *name, nux_u32_t size)
{
    NUX_ASSERT(index != 0);
    NUX_ASSERT(index < NUX_COMPONENT_MAX);
    NUX_ASSERT(_module.components[index].size == 0);
    nux_component_t *comp  = &_module.components[index];
    comp->name             = nux_strdup(nux_core_arena(), name);
    comp->size             = size;
    comp->read             = NUX_NULL;
    comp->write            = NUX_NULL;
    _module.components_max = NUX_MAX(_module.components_max, index + 1);
}
void
nux_component_set_add (nux_u32_t index, nux_component_add_callback_t callback)
{
}
void
nux_component_set_remove (nux_u32_t                       index,
                          nux_component_remove_callback_t callback)
{
}
void
nux_component_set_read (nux_u32_t index, nux_component_read_callback_t callback)
{
}
void
nux_component_set_write (nux_u32_t                      index,
                         nux_component_write_callback_t callback)
{
}
void *
nux_component_get (nux_nid_t e, nux_u32_t c)
{
    NUX_ASSERT(e);
    NUX_ASSERT(c);
    if (!nux_node_has(e, c))
    {
        return NUX_NULL;
    }
    return component_get(nux_scene_active(), e, c);
}
static void *
component_add (nux_scene_t *scene, nux_nid_t e, nux_u32_t c)
{
    nux_u32_t index = NUX_NID_INDEX(e);

    NUX_CHECK(node_check(scene, e), return NUX_NULL);
    NUX_ENSURE(c < _module.components_max,
               return NUX_NULL,
               "invalid scene component id");
    const nux_component_t *component = _module.components + c;

    // initialize pool if component missing
    while (c >= scene->containers.size)
    {
        nux_u32_t              index = scene->containers.size;
        nux_scene_container_t *container
            = nux_scene_container_vec_push(&scene->containers);
        NUX_CHECK(container, return NUX_NULL);
        container->component_size = _module.components[index].size;
        NUX_CHECK(nux_scene_chunk_vec_init_capa(
                      scene->arena, scene->bitset.capa, &container->chunks),
                  return NUX_NULL);
        NUX_CHECK(nux_scene_bitset_init_capa(
                      scene->arena, scene->bitset.capa, &container->bitset),
                  return NUX_NULL);
    }

    nux_scene_container_t *container = scene->containers.data + c;

    // check existing component
    if (!bitset_isset(&container->bitset, index))
    {
        // update bitset
        bitset_set(&container->bitset, index);

        // initialize chunks
        nux_u32_t mask = index / NUX_NODE_PER_MASK;
        while (mask >= container->chunks.size)
        {
            if (!nux_scene_chunk_vec_pushv(&container->chunks, NUX_NULL))
            {
                return NUX_NULL;
            }
        }

        // check if chunk exists
        if (!container->chunks.data[mask])
        {
            // allocate new chunk
            container->chunks.data[mask] = nux_arena_malloc(
                scene->arena, container->component_size * NUX_NODE_PER_MASK);
            // expect zero memory by default
            nux_memset(container->chunks.data[mask],
                       0,
                       container->component_size * NUX_NODE_PER_MASK);
        }
    }

    void *data = component_get(scene, e, c);
    NUX_ASSERT(data);
    if (component->add)
    {
        component->add(e, data);
    }
    return data;
}

void
nux_node_add (nux_nid_t n, nux_u32_t c)
{
    component_add(nux_scene_active(), n, c);
}
void
nux_node_remove (nux_nid_t e, nux_u32_t c)
{
    nux_scene_t           *scene     = nux_scene_active();
    nux_u32_t              index     = NUX_NID_INDEX(e);
    nux_scene_container_t *container = scene->containers.data + c;
    bitset_unset(&container->bitset, index);
    node_detach(scene->nodes.data, e);
}
nux_b32_t
nux_node_has (nux_nid_t e, nux_u32_t c)
{
    NUX_ASSERT(e);
    NUX_ASSERT(c);
    nux_scene_t *scene = nux_scene_active();
    nux_u32_t    index = NUX_NID_INDEX(e);
    NUX_CHECK(c < scene->containers.size, return NUX_FALSE);
    nux_scene_container_t *container = scene->containers.data + c;
    return bitset_isset(&container->bitset, index);
}
static nux_status_t
node_clone (nux_scene_t *scene,
            nux_nid_t    dst_root,
            nux_nid_t    dst_nid,
            nux_nid_t    src_nid,
            nux_nid_t    parent)
{
    nux_u32_t index = NUX_NID_INDEX(src_nid);

    // Duplicate components
    for (nux_u32_t c = 0; c < scene->containers.size; ++c)
    {
        nux_component_t *comp = _module.components + c;
        if (scene->containers.data[c].component_size
            && bitset_isset(&scene->containers.data[c].bitset, index))
        {
            void *src = component_get(scene, src_nid, c);
            NUX_ASSERT(src);
            void *dst = component_add(nux_scene_active(), dst_nid, c);
            NUX_CHECK(dst, return NUX_FAILURE);
            nux_memcpy(dst, src, comp->size);

            // TODO: remap entity references
        }
    }

    // Duplicate children
    nux_nid_t child = scene->nodes.data[index].child;
    while (child)
    {
        if (child != dst_root)
        {
            nux_nid_t child_nid = nux_node_create(dst_nid);
            NUX_CHECK(child_nid, return NUX_FAILURE);
            NUX_CHECK(node_clone(scene, dst_root, child_nid, child, dst_nid),
                      return NUX_FAILURE);
        }
        child = scene->nodes.data[NUX_NID_INDEX(child)].next;
    }

    return NUX_SUCCESS;
}
nux_nid_t
nux_node_instantiate (nux_scene_t *scene, nux_nid_t parent)
{
    nux_nid_t nid = nux_node_create(parent);
    NUX_CHECK(nid, return NUX_NULL);
    NUX_CHECK(node_clone(scene, nid, nid, scene->root, parent),
              return NUX_NULL);
    return nid;
}
