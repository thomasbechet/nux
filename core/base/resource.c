#include "internal.h"

#define RID_INDEX(rid)   (((nux_rid_t)(rid) >> 0) & 0xFFFFFF)
#define RID_VERSION(rid) (((nux_rid_t)(rid) >> 24) & 0xFF)
#define RID_BUILD(old, type, index) \
    (nux_rid_t)(((nux_rid_t)(RID_VERSION(old) + 1) << (nux_rid_t)24) | (index))

static nux_resource_entry_t *
get_entry (nux_rid_t rid, nux_u32_t type)
{
    nux_resource_pool_t *resources = nux_base_resources();
    nux_u32_t            index     = RID_INDEX(rid);
    NUX_CHECK(index < resources->size && resources->data[index].self == rid,
              return NUX_NULL);
    nux_resource_entry_t *entry = resources->data + index;
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
    nux_resource_type_t *resource_types = nux_base_resource_types();
    NUX_ASSERT(index < NUX_RESOURCE_MAX);
    NUX_ASSERT(resource_types[index].name == NUX_NULL);
    nux_resource_type_t *type = resource_types + index;
    nux_memset(type, 0, sizeof(*type));
    type->name              = name;
    type->size              = size;
    type->first_entry_index = NUX_NULL;
    return type;
}

void *
nux_resource_new (nux_arena_t *arena, nux_u32_t type)
{
    nux_resource_pool_t   *resources = nux_base_resources();
    nux_resource_type_t   *t         = nux_base_resource_types() + type;
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
    nux_resource_entry_t *entry = nux_resource_pool_add(resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index         = entry - resources->data;
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
        resources->data[t->last_entry_index].next_entry_index = index;
        t->last_entry_index                                   = index;
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
    nux_resource_pool_t  *resources      = nux_base_resources();
    nux_resource_type_t  *resource_types = nux_base_resource_types();
    nux_resource_entry_t *entry          = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_u32_t index = RID_INDEX(rid);

    // Remove from hotreload
    if (nux_config()->hotreload && entry->path)
    {
        nux_os_hotreload_remove(nux_userdata(), rid);
    }

    // Cleanup resource
    nux_resource_type_t *t = resource_types + entry->type;
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
        resources->data[entry->prev_entry_index].next_entry_index
            = entry->next_entry_index;
    }
    if (entry->next_entry_index)
    {
        resources->data[entry->next_entry_index].prev_entry_index
            = entry->prev_entry_index;
    }
    if (t->first_entry_index == index)
    {
        t->first_entry_index = entry->next_entry_index;
    }
    nux_resource_pool_remove(resources, entry);
}
void
nux_resource_set_path (nux_rid_t rid, const nux_c8_t *path)
{
    nux_resource_type_t  *resource_types = nux_base_resource_types();
    nux_resource_entry_t *entry          = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_resource_type_t *type = resource_types + entry->type;
    NUX_ASSERT(type->reload);
    entry->path = nux_arena_alloc_string(entry->arena, path);
    NUX_CHECK(entry->path, return);
    if (nux_config()->hotreload)
    {
        nux_os_hotreload_add(nux_userdata(), entry->path, rid);
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
    nux_resource_pool_t *resources = nux_base_resources();
    for (nux_u32_t i = 0; i < resources->size; ++i)
    {
        nux_resource_entry_t *entry = resources->data + i;
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
    nux_resource_type_t  *resource_types = nux_base_resource_types();
    nux_resource_entry_t *entry          = check_entry(rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_FAILURE);
    nux_resource_type_t *type = resource_types + entry->type;
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
    nux_resource_type_t *resource_types = nux_base_resource_types();
    nux_resource_pool_t *resources      = nux_base_resources();
    nux_resource_type_t *t              = &resource_types[type];
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
    nux_resource_entry_t *entry = &resources->data[index];
    NUX_ASSERT(entry->type == type);
    return entry->self;
}
void *
nux_resource_nextp (nux_u32_t type, const void *p)
{
    nux_rid_t rid  = p ? nux_resource_rid(p) : NUX_NULL;
    nux_rid_t next = nux_resource_next(type, rid);
    return next ? nux_resource_get(type, next) : NUX_NULL;
}
nux_rid_t
nux_resource_rid (const void *data)
{
    const nux_resource_header_t *header
        = ((const nux_resource_header_t *)data) - 1;
    return header->rid;
}
