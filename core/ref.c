#include "internal.h"

#define BUILD_REF(index, version) ((nux_u32_t)((version)) << 24 | (index))
#define REF_VERSION(ref)          ((ref) >> 24)
#define REF_INDEX(ref)            ((ref) & 0xFFFFFF)

nux_u32_t
nux_ref_create (nux_ctx_t *ctx, nux_u32_t type, void *data)
{
    nux_ref_t *ref = nux_ref_pool_add(&ctx->refs);
    NUX_CHECKM(ref, "Out of references", return NUX_NULL);
    ref->type = type;
    ref->data = data;
    ref->version += 1;
    nux_u32_t index = ref - ctx->refs.data;
    nux_u32_t id    = BUILD_REF(index, ref->version);
    return id;
}
void
nux_ref_delete (nux_ctx_t *ctx, nux_u32_t ref)
{
    nux_u32_t index   = REF_INDEX(ref);
    nux_u8_t  version = REF_VERSION(ref);
    if (index >= ctx->refs.size || ctx->refs.data[index].version != version)
    {
        NUX_ERROR("Failed to delete reference %d", ref);
        return;
    }
    nux_ref_t *entry = ctx->refs.data + index;
    entry->type      = NUX_NULL;
    entry->data      = NUX_NULL;
    entry->version   = NUX_NULL;
    nux_ref_pool_remove(&ctx->refs, entry);
}
void
nux_ref_update (nux_ctx_t *ctx, nux_u32_t ref, void *data)
{
    // TODO
}
void *
nux_ref_get (nux_ctx_t *ctx, nux_u32_t type, nux_u32_t ref)
{
    nux_u32_t index   = REF_INDEX(ref);
    nux_u8_t  version = REF_VERSION(ref);
    if (index >= ctx->refs.size || ctx->refs.data[index].type != type
        || ctx->refs.data[index].version != version)
    {
        nux_type_t *got    = &ctx->types[ctx->refs.data[index].type];
        nux_type_t *expect = &ctx->types[type];
        NUX_ERROR("Invalid object type (expect \"%s\", got \"%s\")",
                  expect->name,
                  got->name);
        return NUX_NULL;
    }
    return ctx->refs.data[index].data;
}
