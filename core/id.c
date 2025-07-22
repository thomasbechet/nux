#include "internal.h"

#define BUILD_REF(index, version) ((nux_u32_t)((version)) << 24 | (index))
#define ID_VERSION(id)            ((id) >> 24)
#define ID_INDEX(id)              ((id) & 0xFFFFFF)

nux_u32_t
nux_id_create (nux_ctx_t *ctx, nux_u32_t type, void *data)
{
    nux_id_entry_t *id_entry = nux_id_pool_add(&ctx->ids);
    NUX_CHECKM(id_entry, return NUX_NULL, "out of ids");
    id_entry->type = type;
    id_entry->data = data;
    id_entry->version += 1;
    nux_u32_t index = id_entry - ctx->ids.data;
    nux_u32_t id    = BUILD_REF(index, id_entry->version);
    return id;
}
void
nux_id_delete (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_u32_t index   = ID_INDEX(id);
    nux_u8_t  version = ID_VERSION(id);
    NUX_CHECKM(index < ctx->ids.size && ctx->ids.data[index].version == version,
               return,
               "failed to delete id %d",
               id);
    nux_id_entry_t *entry = ctx->ids.data + index;
    entry->type           = NUX_NULL;
    entry->data           = NUX_NULL;
    entry->version        = NUX_NULL;
    nux_id_pool_remove(&ctx->ids, entry);
}
void
nux_id_update (nux_ctx_t *ctx, nux_u32_t id, void *data)
{
    // TODO
}
void *
nux_id_get (nux_ctx_t *ctx, nux_u32_t type, nux_u32_t id)
{
    nux_u32_t index   = ID_INDEX(id);
    nux_u8_t  version = ID_VERSION(id);
    NUX_CHECKM(index < ctx->ids.size && ctx->ids.data[index].type == type
                   && ctx->ids.data[index].version == version,
               return NUX_NULL,
               "invalid object type (expect \"%s\", got \"%s\")",
               ctx->types[type].name,
               ctx->types[ctx->ids.data[index].type].name);
    return ctx->ids.data[index].data;
}
