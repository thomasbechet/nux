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
static nux_rid_t
add_entry (nux_arena_t *arena, nux_u32_t type, void *data)
{
    nux_base_module_t    *module = nux_base_module();
    nux_resource_entry_t *entry  = nux_resource_pool_add(&module->resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index = entry - module->resources.data;
    entry->self     = RID_BUILD(entry->self, type, index);
    entry->arena    = arena;
    entry->type     = type;
    entry->data     = data;
    entry->path     = NUX_NULL;
    entry->name     = NUX_NULL;
    return entry->self;
}

nux_resource_type_t *
nux_resource_register (nux_u32_t index, nux_u32_t size, const nux_c8_t *name)
{
    nux_base_module_t *module = nux_base_module();
    NUX_ASSERT(index < NUX_RESOURCE_MAX);
    NUX_ASSERT(module->resources_types[index].name == NUX_NULL);
    nux_resource_type_t *resource = module->resources_types + index;
    nux_memset(resource, 0, sizeof(*resource));
    resource->name = name;
    resource->size = size;
    return resource;
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
    void     *data     = header + 1; // TODO: handle proper memory alignment
    nux_rid_t id       = add_entry(arena, type, data);
    NUX_CHECK(id, return NUX_NULL);
    header->rid = id;
    return data;
}
void
nux_resource_delete (nux_rid_t rid)
{
    nux_base_module_t    *module = nux_base_module();
    nux_resource_entry_t *entry  = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);

    // Remove from hotreload
    if (module->config.hotreload && entry->path)
    {
        nux_os_hotreload_remove(module->userdata, rid);
    }

    // Cleanup resource
    nux_resource_type_t *type = module->resources_types + entry->type;
    NUX_DEBUG("cleanup type:%s rid:0x%08X name:%s path:%s",
              type->name,
              rid,
              entry->name ? entry->name : "null",
              entry->path ? entry->path : "null");
    if (type->cleanup)
    {
        type->cleanup(rid);
    }

    // Remove entry
    entry->data  = NUX_NULL;
    entry->arena = NUX_NULL;
    entry->self  = NUX_NULL;
    entry->type  = NUX_NULL;
    entry->path  = NUX_NULL;
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
nux_resource_get_path (nux_rid_t rid)
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
    nux_arena_t *arena = entry->arena ? entry->arena : nux_arena_core();
    NUX_ASSERT(arena);
    entry->name = nux_arena_alloc_string(arena, name);
    NUX_CHECK(entry->name, return);
}
const nux_c8_t *
nux_resource_get_name (nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->name;
}
nux_arena_t *
nux_resource_get_arena (nux_rid_t rid)
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
        type->reload(rid, entry->path);
    }
    NUX_INFO("resource 0x%08X '%s' successfully reloaded", rid, entry->path);
    return NUX_SUCCESS;
}
nux_rid_t
nux_resource_next (nux_u32_t type, nux_rid_t rid)
{
    nux_base_module_t *module = nux_base_module();
    nux_u32_t          start  = 0;
    if (rid)
    {
        start = RID_INDEX(rid) + 1;
    }
    for (nux_u32_t i = start; i < module->resources.size; ++i)
    {
        nux_resource_entry_t *r = module->resources.data + i;
        if (r->type == type)
        {
            return r->self;
        }
    }
    return NUX_NULL;
}
nux_rid_t
nux_resource_get_rid (void *data)
{
    nux_resource_header_t *header = ((nux_resource_header_t *)data) - 1;
    return header->rid;
}
