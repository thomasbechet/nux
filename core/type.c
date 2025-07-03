#include "internal.h"

nux_type_t *
nux_type_register (nux_ctx_t *ctx, const nux_c8_t *name)
{
    nux_type_t *type = ctx->types + ctx->types_count;
    nux_memset(type, 0, sizeof(*type));
    type->name = name;
    ++ctx->types_count;
    return type;
}
