#include "internal.h"

#define RID_INDEX(rid)   (((nux_rid_t)(rid) >> 0) & 0xFFFFFF)
#define RID_VERSION(rid) (((nux_rid_t)(rid) >> 24) & 0xFF)
#define RID_BUILD(old, type, index) \
    (nux_rid_t)(((nux_rid_t)(RID_VERSION(old) + 1) << (nux_rid_t)24) | (index))

static nux_resource_entry_t *
get_entry (nux_ctx_t *ctx, nux_rid_t rid, nux_u32_t type)
{
    nux_u32_t index = RID_INDEX(rid);
    NUX_CHECK(index < ctx->resources.size
                  && ctx->resources.data[index].self == rid,
              return NUX_NULL);
    nux_resource_entry_t *entry = ctx->resources.data + index;
    if (type && entry->type != type)
    {
        return NUX_NULL;
    }
    return entry;
}
static nux_resource_entry_t *
check_entry (nux_ctx_t *ctx, nux_rid_t rid, nux_u32_t type)
{
    nux_resource_entry_t *entry = get_entry(ctx, rid, type);
    NUX_ENSURE(entry, return NUX_NULL, "invalid resource 0x%X", rid);
    return entry;
}

nux_resource_type_t *
nux_resource_register (nux_ctx_t      *ctx,
                       nux_u32_t       index,
                       nux_u32_t       size,
                       const nux_c8_t *name)
{
    NUX_ASSERT(index < NUX_RESOURCE_MAX);
    NUX_ASSERT(ctx->resources_types[index].name == NUX_NULL);
    nux_resource_type_t *resource = ctx->resources_types + index;
    nux_memset(resource, 0, sizeof(*resource));
    resource->name = name;
    resource->size = size;
    return resource;
}

nux_rid_t
nux_resource_add (nux_ctx_t *ctx, nux_rid_t arena, nux_u32_t type, void *data)
{
    nux_resource_entry_t *entry = nux_resource_pool_add(&ctx->resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index = entry - ctx->resources.data;
    entry->self     = RID_BUILD(entry->self, type, index);
    entry->arena    = arena;
    entry->type     = type;
    entry->data     = data;
    entry->path     = NUX_NULL;
    entry->name     = NUX_NULL;
    return entry->self;
}
void *
nux_resource_new (nux_ctx_t *ctx,
                  nux_rid_t  arena,
                  nux_u32_t  type,
                  nux_rid_t *rid)
{
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    nux_resource_type_t      *t = ctx->resources_types + type;
    nux_resource_finalizer_t *finalizer
        = nux_arena_malloc(a, sizeof(nux_resource_finalizer_t) + t->size);
    NUX_CHECK(finalizer, return NUX_NULL);
    finalizer->prev = a->last_finalizer;
    finalizer->next = NUX_NULL;
    if (!a->first_finalizer)
    {
        a->first_finalizer = finalizer;
    }
    a->last_finalizer = finalizer;
    void     *data    = finalizer + 1; // TODO: handle proper memory alignment
    nux_rid_t id      = nux_resource_add(ctx, arena, type, data);
    if (!id)
    {
        // TODO: rewind arena ?
        return NUX_NULL;
    }
    finalizer->res = id;
    if (rid)
    {
        *rid = id;
    }
    return data;
}
void
nux_resource_delete (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return);

    // Remove from hotreload
    if (ctx->config.hotreload && entry->path)
    {
        nux_os_hotreload_remove(ctx->userdata, rid);
    }

    // Cleanup resource
    nux_resource_type_t *type = ctx->resources_types + entry->type;
    NUX_DEBUG("cleanup type:%s rid:0x%08X name:%s path:%s",
              type->name,
              rid,
              entry->name ? entry->name : "null",
              entry->path ? entry->path : "null");
    if (type->cleanup)
    {
        type->cleanup(ctx, rid);
    }

    // Remove entry
    entry->data  = NUX_NULL;
    entry->arena = NUX_NULL;
    entry->self  = NUX_NULL;
    entry->type  = NUX_NULL;
    entry->path  = NUX_NULL;
    nux_resource_pool_remove(&ctx->resources, entry);
}
void
nux_resource_set_path (nux_ctx_t *ctx, nux_rid_t rid, const nux_c8_t *path)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_resource_type_t *type = ctx->resources_types + entry->type;
    NUX_ASSERT(type->reload);
    nux_arena_t *arena
        = nux_resource_check(ctx, NUX_RESOURCE_ARENA, entry->arena);
    NUX_ASSERT(arena);
    entry->path = nux_arena_alloc_string(arena, path);
    NUX_CHECK(entry->path, return);
    if (ctx->config.hotreload)
    {
        nux_os_hotreload_add(ctx->userdata, entry->path, rid);
    }
}
const nux_c8_t *
nux_resource_get_path (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->path;
}
void
nux_resource_set_name (nux_ctx_t *ctx, nux_rid_t rid, const nux_c8_t *name)
{
    NUX_ENSURE(!nux_resource_find(ctx, name),
               return,
               "duplicated resource name '%s'",
               name);
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return);
    nux_arena_t *arena = nux_resource_check(
        ctx,
        NUX_RESOURCE_ARENA,
        entry->arena ? entry->arena
                     : ctx->core_arena_rid); // support null arena for
                                             // core_arena initialization
    NUX_ASSERT(arena);
    entry->name = nux_arena_alloc_string(arena, name);
    NUX_CHECK(entry->name, return);
}
const nux_c8_t *
nux_resource_get_name (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->name;
}
nux_rid_t
nux_resource_get_arena (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->arena;
}
nux_rid_t
nux_resource_find (nux_ctx_t *ctx, const nux_c8_t *name)
{
    for (nux_u32_t i = 0; i < ctx->resources.size; ++i)
    {
        nux_resource_entry_t *entry = ctx->resources.data + i;
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
nux_resource_get (nux_ctx_t *ctx, nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = get_entry(ctx, rid, type);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->data;
}
void *
nux_resource_check (nux_ctx_t *ctx, nux_u32_t type, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, type);
    NUX_CHECK(entry, return NUX_NULL);
    return entry->data;
}
nux_status_t
nux_resource_reload (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_resource_entry_t *entry = check_entry(ctx, rid, NUX_NULL);
    NUX_CHECK(entry, return NUX_FAILURE);
    nux_resource_type_t *type = ctx->resources_types + entry->type;
    if (type->reload)
    {
        type->reload(ctx, rid, entry->path);
    }
    NUX_INFO("resource 0x%08X '%s' successfully reloaded", rid, entry->path);
    return NUX_SUCCESS;
}
nux_rid_t
nux_resource_next (nux_ctx_t *ctx, nux_u32_t type, nux_rid_t rid)
{
    nux_u32_t start = 0;
    if (rid)
    {
        start = RID_INDEX(rid) + 1;
    }
    for (nux_u32_t i = start; i < ctx->resources.size; ++i)
    {
        nux_resource_entry_t *r = ctx->resources.data + i;
        if (r->type == type)
        {
            return r->self;
        }
    }
    return NUX_NULL;
}
