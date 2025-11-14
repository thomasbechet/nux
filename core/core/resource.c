#include "internal.h"

#define RID_INDEX(rid)   (((nux_rid_t)(rid) >> 0) & 0xFFFFFF)
#define RID_VERSION(rid) (((nux_rid_t)(rid) >> 24) & 0xFF)
#define RID_BUILD(old, type, index) \
    (nux_rid_t)(((nux_rid_t)(RID_VERSION(old) + 1) << (nux_rid_t)24) | (index))

static nux_resource_entry_t *
get_entry (nux_rid_t rid, nux_u32_t type)
{
    nux_resource_pool_t *resources = nux_core_resources();
    nux_u32_t            index     = RID_INDEX(rid);
    nux_check(index < resources->size && resources->data[index].rid == rid,
              return nullptr);
    nux_resource_entry_t *entry = resources->data + index;
    if (type && entry->type_index != type)
    {
        return nullptr;
    }
    return entry;
}
static nux_resource_entry_t *
get_entry_from_data (void *data)
{
    nux_check(data, return nullptr);
    nux_rid_t                  rid       = nux_resource_rid(data);
    const nux_resource_pool_t *resources = nux_core_resources();
    nux_resource_entry_t      *entry     = &resources->data[RID_INDEX(rid)];
    nux_assert(entry->data == data);
    return entry;
}
static nux_resource_entry_t *
check_entry (nux_rid_t rid, nux_u32_t type)
{
    nux_resource_entry_t *entry = get_entry(rid, type);
    nux_ensure(entry, return nullptr, "invalid resource 0x%X", rid);
    return entry;
}
static nux_resource_entry_t *
resource_next (nux_u32_t type, const nux_resource_entry_t *entry)
{
    nux_resource_type_t *resource_types = nux_core_resource_types();
    nux_resource_pool_t *resources      = nux_core_resources();
    nux_u32_t            index;
    if (entry)
    {
        index = entry->next_entry_index;
    }
    else
    {
        index = resource_types[type].first_entry_index;
    }
    nux_check(index, return nullptr);
    nux_resource_entry_t *next = &resources->data[index];
    nux_assert(next->type_index == type);
    return next;
}
static void
resource_set_path (nux_resource_entry_t *entry, const nux_c8_t *path)
{
    nux_resource_type_t *resource_types = nux_core_resource_types();
    nux_check(entry, return);
    nux_resource_type_t *type = resource_types + entry->type_index;
    nux_assert(type->info.reload);
    entry->path = nux_strdup(entry->arena, path);
    nux_check(entry->path, return);
    if (nux_config_get()->hotreload)
    {
        nux_os_hotreload_add(entry->path, entry->rid);
    }
}
void
resource_set_name (nux_resource_entry_t *entry, const nux_c8_t *name)
{
    nux_ensure(!nux_resource_find(name),
               return,
               "duplicated resource name '%s'",
               name);
    nux_check(entry, return);
    entry->name = nux_strdup(entry->arena, name);
    nux_assert(entry->name);
}
nux_resource_entry_t *
resource_find (const nux_c8_t *name)
{
    nux_resource_pool_t *resources = nux_core_resources();
    for (nux_u32_t i = 0; i < resources->size; ++i)
    {
        nux_resource_entry_t *entry = resources->data + i;
        if (entry->type_index && entry->name)
        {
            if (nux_strncmp(entry->name, name, NUX_PATH_MAX) == 0)
            {
                return entry;
            }
        }
    }
    return nullptr;
}

void
nux_resource_register (nux_u32_t index, nux_resource_info_t info)
{
    nux_resource_type_t *resource_types = nux_core_resource_types();
    nux_assert(index < NUX_RESOURCE_MAX);
    nux_assert(resource_types[index].info.name == nullptr);
    nux_resource_type_t *type = resource_types + index;
    nux_memset(type, 0, sizeof(*type));
    type->info              = info;
    type->first_entry_index = 0;
}

nux_resource_entry_t *
nux_resource_add (nux_resource_pool_t *resources, nux_u32_t type)
{
    nux_resource_entry_t *entry = nux_pool_add(resources);
    nux_u32_t             index = entry - resources->data;
    entry->rid                  = RID_BUILD(entry->rid, type, index);
    entry->arena                = nullptr;
    entry->type_index           = type;
    entry->data                 = nullptr;
    entry->path                 = nullptr;
    entry->name                 = nullptr;
    entry->next_entry_index     = 0;
    nux_resource_type_t *t      = nux_core_resource_types() + type;
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
    nux_debug("new resource type '%s' rid 0x%08X", t->info.name, entry->rid);
    return entry;
}
static void
resource_finalizer (void *p)
{
    nux_resource_header_t *header = p;
    nux_assert(p);
    nux_assert(header->rid);

    nux_rid_t             rid            = header->rid;
    nux_resource_pool_t  *resources      = nux_core_resources();
    nux_resource_type_t  *resource_types = nux_core_resource_types();
    nux_resource_entry_t *entry          = check_entry(rid, NUX_RESOURCE_NULL);
    nux_check(entry, return);
    nux_u32_t index = RID_INDEX(rid);

    // Remove from hotreload
    if (nux_config_get()->hotreload && entry->path)
    {
        nux_os_hotreload_remove(rid);
    }

    // Cleanup resource
    nux_resource_type_t *t = resource_types + entry->type_index;
    nux_debug("cleanup type:%s rid:0x%08X name:%s path:%s",
              t->info.name,
              rid,
              entry->name ? entry->name : "null",
              entry->path ? entry->path : "null");
    if (t->info.cleanup)
    {
        nux_assert(entry->data);
        t->info.cleanup(entry->data);
    }

    // Remove entry
    entry->data       = nullptr;
    entry->arena      = nullptr;
    entry->rid        = NUX_NULL;
    entry->type_index = 0;
    entry->path       = nullptr;
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
    nux_pool_remove(resources, entry);
}

nux_u32_t
nux_resource_header_size (nux_u32_t size)
{
    return size
           + sizeof(
               nux_resource_header_t); // TODO: handle proper memory alignment
}
void
nux_resource_header_init (nux_resource_header_t *header, nux_rid_t rid)
{
    nux_assert(rid);
    header->rid = rid;
}
void *
nux_resource_header_to_data (nux_resource_header_t *header)
{
    return header + 1; // TODO: handle proper memory alignment
}
nux_resource_header_t *
nux_resource_header_from_data (void *data)
{
    return ((nux_resource_header_t *)data)
           - 1; // TODO: handle proper memory alignment
}

void *
nux_resource_new (nux_arena_t *a, nux_u32_t type)
{
    nux_ensure(a, return nullptr, "invalid null arena");

    nux_resource_pool_t *resources = nux_core_resources();
    nux_resource_type_t *t         = nux_core_resource_types() + type;

    // Add entry
    nux_resource_entry_t *entry = nux_resource_add(resources, type);
    nux_check(entry, return nullptr);
    entry->arena = a;

    // Allocate header + resource
    nux_resource_header_t *header = nux_arena_new_object(
        a, nux_resource_header_size(t->info.size), resource_finalizer);
    nux_check(header, return nullptr);
    nux_resource_header_init(header, entry->rid);
    void *data  = nux_resource_header_to_data(header);
    entry->data = data;

    return data;
}
void *
nux_resource_get (nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = get_entry(rid, type);
    nux_check(entry, return nullptr);
    return entry->data;
}
void *
nux_resource_check (nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(rid, type);
    nux_check(entry, return nullptr);
    return entry->data;
}
nux_status_t
nux_resource_reload (nux_rid_t rid)
{
    nux_resource_type_t  *resource_types = nux_core_resource_types();
    nux_resource_entry_t *entry          = check_entry(rid, NUX_NULL);
    nux_check(entry, return NUX_FAILURE);
    nux_resource_type_t *type = resource_types + entry->type_index;
    if (type->info.reload)
    {
        type->info.reload(entry->data, entry->path);
    }
    nux_info("resource 0x%08X '%s' successfully reloaded", rid, entry->path);
    return NUX_SUCCESS;
}
void
nux_resource_set_path (void *data, const nux_c8_t *path)
{
    nux_resource_type_t  *resource_types = nux_core_resource_types();
    nux_resource_entry_t *entry          = get_entry_from_data(data);
    resource_set_path(entry, path);
}
const nux_c8_t *
nux_resource_path (void *data)
{
    nux_resource_entry_t *entry = get_entry_from_data(data);
    return entry->path;
}
void
nux_resource_set_name_rid (nux_rid_t rid, const nux_c8_t *name)
{
    nux_resource_entry_t *entry = check_entry(rid, NUX_NULL);
    resource_set_name(entry, name);
}
void
nux_resource_set_name (void *data, const nux_c8_t *name)
{
    nux_resource_entry_t *entry = get_entry_from_data(data);
    resource_set_name(entry, name);
}
const nux_c8_t *
nux_resource_name (void *data)
{
    nux_resource_entry_t *entry = get_entry_from_data(data);
    nux_check(entry, return nullptr);
    return entry->name;
}
void *
nux_resource_next (nux_u32_t type, void *p)
{
    nux_resource_entry_t *entry = resource_next(type, get_entry_from_data(p));
    nux_check(entry, return nullptr);
    return entry->data;
}
nux_rid_t
nux_resource_rid (const void *data)
{
    nux_check(data, return NUX_NULL);
    return nux_resource_header_from_data((void *)data)->rid;
}
nux_arena_t *
nux_resource_arena (void *data)
{
    nux_resource_entry_t *entry = get_entry_from_data(data);
    nux_check(entry, return nullptr);
    return entry->arena;
}
void *
nux_resource_find (const nux_c8_t *name)
{
    nux_resource_entry_t *entry = resource_find(name);
    nux_check(entry, return nullptr);
    return entry->data;
}
