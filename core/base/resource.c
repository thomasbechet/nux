#include "nux_internal.h"

#define RES_INDEX(res)   (((nux_res_t)(res) >> 0) & 0xFFFFFF)
#define RES_VERSION(res) (((nux_res_t)(res) >> 24) & 0xFF)
#define RES_BUILD(old, type, index) \
    (nux_res_t)(((nux_res_t)(RES_VERSION(old) + 1) << (nux_res_t)24) | (index))

nux_resource_type_t *
nux_res_register (nux_ctx_t *ctx, const nux_c8_t *name)
{
    nux_resource_type_t *resource
        = ctx->resources_types + ctx->resources_types_count;
    nux_memset(resource, 0, sizeof(*resource));
    resource->name = name;
    ++ctx->resources_types_count;
    return resource;
}

nux_res_t
nux_res_add (nux_ctx_t *ctx, nux_res_t arena, nux_u32_t type, void *data)
{
    nux_resource_entry_t *entry = nux_resource_pool_add(&ctx->resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index = entry - ctx->resources.data;
    entry->self     = RES_BUILD(entry->self, type, index);
    entry->arena    = arena;
    entry->type     = type;
    entry->data     = data;
    entry->path     = NUX_NULL;
    return entry->self;
}
void *
nux_res_new (nux_ctx_t *ctx,
             nux_res_t  arena,
             nux_u32_t  type,
             nux_u32_t  size,
             nux_res_t *id)
{
    nux_arena_t *a = nux_res_check(ctx, NUX_RES_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    nux_resource_finalizer_t *finalizer
        = nux_arena_alloc_raw(ctx, a, sizeof(nux_resource_finalizer_t) + size);
    NUX_CHECK(finalizer, return NUX_NULL);
    finalizer->prev = a->last_finalizer;
    finalizer->next = NUX_NULL;
    if (!a->first_finalizer)
    {
        a->first_finalizer = finalizer;
    }
    a->last_finalizer = finalizer;
    void     *data    = finalizer + 1; // TODO: handle proper memory alignment
    nux_res_t res     = nux_res_add(ctx, arena, type, data);
    if (!id)
    {
        // TODO: rewind arena ?
        return NUX_NULL;
    }
    finalizer->res = res;
    if (id)
    {
        *id = res;
    }
    return data;
}
void
nux_res_delete (nux_ctx_t *ctx, nux_res_t res)
{
    nux_u32_t index = RES_INDEX(res);
    NUX_ENSURE(index < ctx->resources.size
                   && ctx->resources.data[index].self == res,
               return,
               "failed to delete resource %d",
               res);
    nux_resource_entry_t *entry = ctx->resources.data + index;

    // Remove from hotreload
    if (ctx->config.hotreload && entry->path)
    {
        nux_os_hotreload_remove(ctx->userdata, res);
    }

    // Cleanup resource
    nux_resource_type_t *type = ctx->resources_types + entry->type;
    NUX_DEBUG("cleanup '%s' 0x%08X", type->name, res);
    if (type->cleanup)
    {
        type->cleanup(ctx, res);
    }

    // Remove entry
    entry->data  = NUX_NULL;
    entry->arena = NUX_NULL;
    entry->self  = NUX_NULL;
    entry->type  = NUX_NULL;
    entry->path  = NUX_NULL;
    nux_resource_pool_remove(&ctx->resources, entry);
}
void *
nux_res_check (nux_ctx_t *ctx, nux_u32_t type, nux_res_t res)
{
    nux_u32_t index = RES_INDEX(res);
    NUX_ENSURE(index < ctx->resources.size
                   && ctx->resources.data[index].self == res
                   && ctx->resources.data[index].type == type,
               return NUX_NULL,
               "invalid resource 0x%X",
               res);
    return ctx->resources.data[index].data;
}
void
nux_res_watch (nux_ctx_t *ctx, nux_res_t res, const nux_c8_t *path)
{
    nux_u32_t             index = RES_INDEX(res);
    nux_resource_entry_t *entry = ctx->resources.data + index;
    nux_resource_type_t  *type  = ctx->resources_types + entry->type;
    NUX_ASSERT(type->reload);
    entry->path = nux_arena_alloc_path(ctx, entry->arena, path);
    NUX_CHECK(entry->path, return);
    if (ctx->config.hotreload)
    {
        nux_os_hotreload_add(ctx->userdata, entry->path, res);
    }
}
nux_status_t
nux_res_reload (nux_ctx_t *ctx, nux_res_t res)
{
    nux_u32_t index = RES_INDEX(res);
    NUX_CHECK(index < ctx->resources.size
                  && ctx->resources.data[index].self == res,
              return NUX_FAILURE);
    nux_resource_entry_t *entry = ctx->resources.data + index;
    nux_resource_type_t  *type  = ctx->resources_types + entry->type;
    if (type->reload)
    {
        type->reload(ctx, res, entry->path);
    }
    return NUX_SUCCESS;
}
nux_res_t
nux_res_next (nux_ctx_t *ctx, nux_u32_t type, nux_res_t res)
{
    nux_u32_t start = 0;
    if (res)
    {
        start = RES_INDEX(res) + 1;
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
