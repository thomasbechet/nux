#include "internal.h"

#define RID_INDEX(rid)   (((nux_rid_t)(rid) >> 0) & 0xFFFFFF)
#define RID_VERSION(rid) (((nux_rid_t)(rid) >> 24) & 0xFF)
#define RID_BUILD(old, type, index) \
    (nux_rid_t)(((nux_rid_t)(RID_VERSION(old) + 1) << (nux_rid_t)24) | (index))

static nux_resource_entry_t *
get_entry (nux_rid_t rid, nux_u32_t type)
{
    nux_base_module_t *module = nux_base_module();
    nux_u32_t          index  = RID_INDEX(rid);
    NUX_CHECK(index < module->resources.size
                  && module->resources.data[index].self == rid,
              return NUX_NULL);
    nux_resource_entry_t *entry = module->resources.data + index;
    if (type && entry->type != type)
    {
        return NUX_NULL;
    }
    return entry;
}
static nux_resource_entry_t *
check_entry (nux_rid_t rid, nux_u32_t type)
{
    nux_resource_entry_t *entry = get_entry(rid, type);
    NUX_ENSURE(entry, return NUX_NULL, "invalid resource 0x%X", rid);
    return entry;
}

nux_resource_type_t *
nux_resource_register (nux_u32_t index, nux_u32_t size, const nux_c8_t *name)
{
    nux_base_module_t *module = nux_base_module();
    NUX_ASSERT(index < NUX_RESOURCE_MAX);
    NUX_ASSERT(module->resources_types[index].name == NUX_NULL);
    nux_resource_type_t *resource = module->resources_types + index;
    nux_memset(resource, 0, sizeof(*resource));
    resource->name              = name;
    resource->size              = size;
    resource->first_entry_index = NUX_NULL;
    return resource;
}

nux_status_t
nux_resource_init (void)
{
    nux_base_module_t   *module = nux_base_module();
    nux_resource_pool_t *pool   = &module->resources;
    // Create core arena
    nux_arena_t core_arena;
    nux_arena_init(&core_arena);
    // Create resource pool
    NUX_CHECK(nux_resource_pool_init(&core_arena, pool), return NUX_FAILURE);
    // Reserve index 0 for null id
    nux_resource_pool_add(pool);
    // Reserve index 1 for core arena
    module->core_arena = nux_resource_new(&core_arena, NUX_RESOURCE_ARENA);
    NUX_CHECK(module->core_arena, return NUX_FAILURE);
    *module->core_arena = core_arena;
    // Patch core arena resource to reference itself
    pool->data[1].arena = module->core_arena;
    // Patch resource pool freelist to reference new core arena
    module->resources.freelist.arena = module->core_arena;
    return NUX_SUCCESS;
}

void *
nux_resource_new (nux_arena_t *arena, nux_u32_t type)
{
    nux_base_module_t     *module = nux_base_module();
    nux_resource_type_t   *t      = module->resources_types + type;
    nux_resource_header_t *header
        = nux_arena_malloc(arena, sizeof(nux_resource_header_t) + t->size);
    NUX_CHECK(header, return NUX_NULL);
    header->prev = arena->last_header;
    header->next = NUX_NULL;
    if (!arena->first_header)
    {
        arena->first_header = header;
    }
    arena->last_header = header;
    void *data         = header + 1; // TODO: handle proper memory alignment

    // Add new entry
    nux_resource_entry_t *entry = nux_resource_pool_add(&module->resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index         = entry - module->resources.data;
    entry->self             = RID_BUILD(entry->self, type, index);
    entry->arena            = arena;
    entry->type             = type;
    entry->data             = data;
    entry->path             = NUX_NULL;
    entry->name             = NUX_NULL;
    entry->next_entry_index = NUX_NULL;
    if (t->last_entry_index)
    {
        entry->prev_entry_index = t->first_entry_index;
        module->resources.data[t->last_entry_index].next_entry_index = index;
        t->last_entry_index                                          = index;
    }
    else
    {
        t->first_entry_index = index;
        t->last_entry_index  = index;
    }

    header->rid = entry->self;
    return data;
}
void
nux_resource_delete (nux_rid_t rid)
{
    nux_base_module_t    *module = nux_base_module();
    nux_resource_entry_t *entry  = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_u32_t index = RID_INDEX(rid);

    // Remove from hotreload
    if (module->config.hotreload && entry->path)
    {
        nux_os_hotreload_remove(module->userdata, rid);
    }

    // Cleanup resource
    nux_resource_type_t *t = module->resources_types + entry->type;
    NUX_DEBUG("cleanup type:%s rid:0x%08X name:%s path:%s",
              t->name,
              rid,
              entry->name ? entry->name : "null",
              entry->path ? entry->path : "null");
    if (t->cleanup)
    {
        t->cleanup(entry->data);
    }

    // Remove entry
    entry->data  = NUX_NULL;
    entry->arena = NUX_NULL;
    entry->self  = NUX_NULL;
    entry->type  = NUX_NULL;
    entry->path  = NUX_NULL;
    if (entry->prev_entry_index)
    {
        module->resources.data[entry->prev_entry_index].next_entry_index
            = entry->next_entry_index;
    }
    if (entry->next_entry_index)
    {
        module->resources.data[entry->next_entry_index].prev_entry_index
            = entry->prev_entry_index;
    }
    if (t->first_entry_index == index)
    {
        t->first_entry_index = entry->next_entry_index;
    }
    nux_resource_pool_remove(&module->resources, entry);
}
void
nux_resource_set_path (nux_rid_t rid, const nux_c8_t *path)
{
    nux_base_module_t    *module = nux_base_module();
    nux_resource_entry_t *entry  = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_resource_type_t *type = module->resources_types + entry->type;
    NUX_ASSERT(type->reload);
    entry->path = nux_arena_alloc_string(entry->arena, path);
    NUX_CHECK(entry->path, return);
    if (module->config.hotreload)
    {
        nux_os_hotreload_add(module->userdata, entry->path, rid);
    }
}
const nux_c8_t *
nux_resource_path (nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->path;
}
void
nux_resource_set_name (nux_rid_t rid, const nux_c8_t *name)
{
    NUX_ENSURE(!nux_resource_find(name),
               return,
               "duplicated resource name '%s'",
               name);
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);
    entry->name = nux_arena_alloc_string(entry->arena, name);
    NUX_ASSERT(entry->name);
}
const nux_c8_t *
nux_resource_name (nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->name;
}
nux_arena_t *
nux_resource_arena (nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->arena;
}
nux_rid_t
nux_resource_find (const nux_c8_t *name)
{
    nux_base_module_t *module = nux_base_module();
    for (nux_u32_t i = 0; i < module->resources.size; ++i)
    {
        nux_resource_entry_t *entry = module->resources.data + i;
        if (entry->type && entry->name)
        {
            if (nux_strncmp(entry->name, name, NUX_PATH_MAX) == 0)
            {
                return entry->self;
            }
        }
    }
    return NUX_NULL;
}
void *
nux_resource_get (nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = get_entry(rid, type);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->data;
}
void *
nux_resource_check (nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, type);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->data;
}
nux_status_t
nux_resource_reload (nux_rid_t rid)
{
    nux_base_module_t    *module = nux_base_module();
    nux_resource_entry_t *entry  = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_FAILURE);
    nux_resource_type_t *type = module->resources_types + entry->type;
    if (type->reload)
    {
        type->reload(entry->data, entry->path);
    }
    NUX_INFO("resource 0x%08X '%s' successfully reloaded", rid, entry->path);
    return NUX_SUCCESS;
}
nux_rid_t
nux_resource_next (nux_u32_t type, nux_rid_t rid)
{
    nux_base_module_t   *module = nux_base_module();
    nux_resource_type_t *t      = &module->resources_types[type];
    nux_u32_t            index;
    if (rid)
    {
        nux_resource_entry_t *entry = check_entry(rid, type);
        index                       = entry->next_entry_index;
    }
    else
    {
        index = t->first_entry_index;
    }
    NUX_CHECK(index, return NUX_NULL);
    nux_resource_entry_t *entry = &module->resources.data[index];
    NUX_ASSERT(entry->type == type);
    return entry->self;
}
void *
nux_resource_nextp (nux_u32_t type, const void *p)
{
    nux_rid_t next = nux_resource_next(type, nux_resource_rid(p));
    return next ? nux_resource_get(type, next) : NUX_NULL;
}
nux_rid_t
nux_resource_rid (const void *data)
{
    const nux_resource_header_t *header
        = ((const nux_resource_header_t *)data) - 1;
    return header->rid;
}
