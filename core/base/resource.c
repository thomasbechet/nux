#include "internal.h"

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
nux_res_create (nux_ctx_t *ctx, nux_u32_t type, void *data)
{
    nux_resource_t *entry = nux_resource_pool_add(&ctx->resources);
    NUX_ENSURE(entry, return NUX_NULL, "out of resources");
    nux_u32_t index = entry - ctx->resources.data;
    entry->self     = RES_BUILD(entry->self, type, index);
    entry->type     = type;
    entry->data     = data;
    return entry->self;
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
    nux_resource_t *entry = ctx->resources.data + index;
    entry->data           = NUX_NULL;
    entry->self           = NUX_NULL;
    entry->type           = NUX_NULL;
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
