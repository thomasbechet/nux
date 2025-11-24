#include "internal.h"

#define id_index(id)   (((nux_id_t)(id) >> 0) & 0xFFFFFF)
#define id_version(id) (((nux_id_t)(id) >> 24) & 0xFF)
#define id_build(old, type, index) \
    (nux_id_t)(((nux_id_t)(id_version(old) + 1) << (nux_id_t)24) | (index))

static nux_object_entry_t *
get_entry (nux_id_t id, nux_u32_t type)
{
    nux_object_pool_t *objects = &nux_core()->objects;
    nux_u32_t          index   = id_index(id);
    nux_check(index < objects->size && objects->data[index].id == id,
              return nullptr);
    nux_object_entry_t *entry = objects->data + index;
    if (type && entry->type != type)
    {
        return nullptr;
    }
    return entry;
}
static nux_object_entry_t *
get_entry_from_data (void *data)
{
    nux_check(data, return nullptr);
    nux_id_t            id      = nux_object_id(data);
    nux_object_pool_t  *objects = &nux_core()->objects;
    nux_object_entry_t *entry   = &objects->data[id_index(id)];
    nux_assert(entry->data == data);
    return entry;
}
static nux_object_entry_t *
check_entry (nux_id_t id, nux_u32_t type)
{
    nux_object_entry_t *entry = get_entry(id, type);
    nux_ensure(entry, return nullptr, "invalid object 0x%X", id);
    return entry;
}
static nux_object_entry_t *
object_next (nux_u32_t type, const nux_object_entry_t *entry)
{
    const nux_object_type_t *t = nux_object_type(type);
    nux_check(t, return nullptr);
    nux_object_pool_t *objects = &nux_core()->objects;
    nux_u32_t          index;
    if (entry)
    {
        index = entry->next_entry_index;
    }
    else
    {
        index = t->first_entry_index;
    }
    nux_check(index, return nullptr);
    nux_object_entry_t *next = &objects->data[index];
    nux_assert(next->type == type);
    return next;
}
static void
object_set_path (nux_object_entry_t *entry, const nux_c8_t *path)
{
    const nux_object_type_t *t = nux_object_type(entry->type);
    nux_assert(t);
    nux_assert(t->reload);
    entry->path = nux_strdup(entry->arena, path);
    nux_check(entry->path, return);
    if (nux_config_get()->hotreload)
    {
        nux_os_hotreload_add(entry->path, entry->id);
    }
}
void
object_set_name (nux_object_entry_t *entry, const nux_c8_t *name)
{
    nux_ensure(
        !nux_object_find(name), return, "duplicated object name '%s'", name);
    nux_check(entry, return);
    entry->name = nux_strdup(entry->arena, name);
    nux_assert(entry->name);
}
nux_object_entry_t *
object_find (const nux_c8_t *name)
{
    nux_object_pool_t *objects = &nux_core()->objects;
    for (nux_u32_t i = 0; i < objects->size; ++i)
    {
        nux_object_entry_t *entry = objects->data + i;
        if (entry->type && entry->name)
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
nux_object_register (nux_u32_t index, nux_object_info_t info)
{
    nux_object_type_t *types = nux_core()->object_types;
    nux_assert(index < NUX_OBJECT_MAX);
    nux_assert(types[index].name == nullptr);
    nux_object_type_t *type = types + index;
    nux_memset(type, 0, sizeof(*type));
    type->name              = info.name;
    type->size              = info.size;
    type->cleanup           = info.cleanup;
    type->reload            = info.reload;
    type->first_entry_index = 0;
    type->last_entry_index  = 0;
    type->is_component      = info.is_component;
    if (type->is_component)
    {
        type->component_index = nux_core()->component_count;
        ++nux_core()->component_count;
    }
}
nux_object_type_t *
nux_object_type (nux_u32_t type)
{
    nux_object_type_t *types = nux_core()->object_types;
    nux_check(type < NUX_OBJECT_MAX, return nullptr);
    nux_object_type_t *t = types + type;
    nux_check(t->name, return nullptr);
    return t;
}

nux_object_entry_t *
nux_object_add (nux_object_pool_t *objects, nux_u32_t type)
{
    nux_u32_t           index;
    nux_object_entry_t *entry = nux_pool_add(objects, &index);
    entry->type               = type;
    entry->id                 = id_build(entry->id, type, index);
    entry->arena              = nullptr;
    entry->data               = nullptr;
    entry->path               = nullptr;
    entry->name               = nullptr;
    entry->next_entry_index   = 0;
    nux_object_type_t *t      = nux_object_type(type);
    if (t->last_entry_index)
    {
        entry->prev_entry_index = t->first_entry_index;
        objects->data[t->last_entry_index].next_entry_index = index;
        t->last_entry_index                                 = index;
    }
    else
    {
        t->first_entry_index = index;
        t->last_entry_index  = index;
    }
    nux_debug("new object type '%s' id 0x%08X", t->name, entry->id);
    return entry;
}
static void
object_finalizer (void *p)
{
    nux_object_header_t *header = p;
    nux_assert(p);
    nux_assert(header->id);

    nux_id_t            id      = header->id;
    nux_object_pool_t  *objects = &nux_core()->objects;
    nux_object_entry_t *entry   = check_entry(id, NUX_OBJECT_NULL);
    nux_check(entry, return);
    nux_u32_t index = id_index(id);

    // Remove from hotreload
    if (nux_config_get()->hotreload && entry->path)
    {
        nux_os_hotreload_remove(id);
    }

    // Cleanup object
    nux_object_type_t *t = nux_object_type(entry->type);
    nux_assert(t);
    nux_debug("cleanup type:%s id:0x%08X name:%s path:%s",
              t->name,
              id,
              entry->name ? entry->name : "null",
              entry->path ? entry->path : "null");
    if (t->cleanup)
    {
        nux_assert(entry->data);
        t->cleanup(entry->data);
    }

    // Remove entry
    entry->data  = nullptr;
    entry->arena = nullptr;
    entry->id    = NUX_NULL;
    entry->type  = 0;
    entry->path  = nullptr;
    if (entry->prev_entry_index)
    {
        objects->data[entry->prev_entry_index].next_entry_index
            = entry->next_entry_index;
    }
    if (entry->next_entry_index)
    {
        objects->data[entry->next_entry_index].prev_entry_index
            = entry->prev_entry_index;
    }
    if (t->first_entry_index == index)
    {
        t->first_entry_index = entry->next_entry_index;
    }
    nux_pool_remove(objects, index);
}

nux_u32_t
nux_object_header_size (nux_u32_t size)
{
    return size
           + sizeof(
               nux_object_header_t); // TODO: handle proper memory alignment
}
void
nux_object_header_init (nux_object_header_t *header, nux_id_t id)
{
    nux_assert(id);
    header->id = id;
}
void *
nux_object_header_to_data (nux_object_header_t *header)
{
    return header + 1; // TODO: handle proper memory alignment
}
nux_object_header_t *
nux_object_header_from_data (void *data)
{
    return ((nux_object_header_t *)data)
           - 1; // TODO: handle proper memory alignment
}

void *
nux_object_new (nux_arena_t *a, nux_u32_t type)
{
    nux_ensure(a, return nullptr, "invalid null arena");

    nux_object_pool_t *objects = &nux_core()->objects;
    nux_object_type_t *t       = nux_object_type(type);

    // Add entry
    nux_object_entry_t *entry = nux_object_add(objects, type);
    nux_check(entry, return nullptr);
    entry->arena = a;

    // Allocate header + object
    nux_object_header_t *header
        = nux_mallocf(a, nux_object_header_size(t->size), object_finalizer);
    nux_check(header, return nullptr);
    nux_object_header_init(header, entry->id);
    void *data  = nux_object_header_to_data(header);
    entry->data = data;

    return data;
}
void *
nux_object_get (nux_u32_t type, nux_id_t id)
{
    nux_object_entry_t *entry = get_entry(id, type);
    nux_check(entry, return nullptr);
    return entry->data;
}
void *
nux_object_check (nux_u32_t type, nux_id_t id)
{
    nux_object_entry_t *entry = check_entry(id, type);
    nux_check(entry, return nullptr);
    return entry->data;
}
nux_status_t
nux_object_reload (nux_id_t id)
{
    nux_object_pool_t  *objects = &nux_core()->objects;
    nux_object_entry_t *entry   = check_entry(id, NUX_NULL);
    nux_check(entry, return NUX_FAILURE);
    nux_object_type_t *t = nux_object_type(entry->type);
    nux_check(t, return NUX_FAILURE);
    if (t->reload)
    {
        t->reload(entry->data, entry->path);
    }
    nux_info("object 0x%08X '%s' successfully reloaded", id, entry->path);
    return NUX_SUCCESS;
}
void
nux_object_set_path (void *data, const nux_c8_t *path)
{
    nux_object_entry_t *entry = get_entry_from_data(data);
    object_set_path(entry, path);
}
const nux_c8_t *
nux_object_path (void *data)
{
    nux_object_entry_t *entry = get_entry_from_data(data);
    return entry->path;
}
void
nux_object_set_name (void *data, const nux_c8_t *name)
{
    nux_object_entry_t *entry = get_entry_from_data(data);
    object_set_name(entry, name);
}
const nux_c8_t *
nux_object_name (void *data)
{
    nux_object_entry_t *entry = get_entry_from_data(data);
    nux_check(entry, return nullptr);
    return entry->name;
}
void *
nux_object_next (nux_u32_t type, void *p)
{
    nux_object_entry_t *entry = object_next(type, get_entry_from_data(p));
    nux_check(entry, return nullptr);
    return entry->data;
}
nux_id_t
nux_object_id (const void *data)
{
    nux_check(data, return NUX_NULL);
    return nux_object_header_from_data((void *)data)->id;
}
nux_arena_t *
nux_object_arena (void *data)
{
    nux_object_entry_t *entry = get_entry_from_data(data);
    nux_check(entry, return nullptr);
    return entry->arena;
}
void *
nux_object_find (const nux_c8_t *name)
{
    nux_object_entry_t *entry = object_find(name);
    nux_check(entry, return nullptr);
    return entry->data;
}
