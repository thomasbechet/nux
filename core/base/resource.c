#include "internal.h"

nux_resource_t *
nux_resource_register (nux_ctx_t *ctx, const nux_c8_t *name)
{
    nux_resource_t *resource = ctx->resources + ctx->resources_count;
    nux_memset(resource, 0, sizeof(*resource));
    resource->name = name;
    ++ctx->resources_count;
    return resource;
}
