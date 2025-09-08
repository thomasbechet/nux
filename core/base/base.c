#include "internal.h"

nux_status_t
nux_base_init (nux_ctx_t *ctx)
{
    ctx->running = NUX_TRUE;
    nux_error_reset(ctx);

    // Create core arena
    nux_arena_init(ctx, &ctx->core_arena, "core_arena");

    // Register base types
    nux_resource_type_t *type;
    type          = nux_resource_register(ctx, NUX_RESOURCE_NULL, "null");
    type          = nux_resource_register(ctx, NUX_RESOURCE_ARENA, "arena");
    type->cleanup = nux_arena_cleanup;
    type          = nux_resource_register(ctx, NUX_RESOURCE_EVENT, "event");

    // Create resource pool
    NUX_CHECK(nux_resource_pool_alloc(&ctx->core_arena, 1024, &ctx->resources),
              return NUX_FAILURE);
    // Reserve index 0 for null id
    NUX_ASSERT(nux_resource_pool_add(&ctx->resources));

    // Register core arena
    ctx->core_arena_rid
        = nux_resource_add(ctx, NUX_NULL, NUX_RESOURCE_ARENA, &ctx->core_arena);

    // Create frame arena
    ctx->frame_arena = nux_arena_new(ctx, ctx->core_arena_rid, "frame_arena");
    NUX_ASSERT(ctx->frame_arena);

    // Initialize system state
    ctx->error_enable = NUX_TRUE;
    ctx->pcg          = nux_pcg(10243124, 1823719241);

    return NUX_SUCCESS;
}
void
nux_base_free (nux_ctx_t *ctx)
{
}

nux_u32_t
nux_stat (nux_ctx_t *ctx, nux_stat_t stat)
{
    return ctx->stats[stat];
}
nux_f32_t
nux_time_elapsed (nux_ctx_t *ctx)
{
    return ctx->time_elapsed;
}
nux_f32_t
nux_time_delta (nux_ctx_t *ctx)
{
    return 1. / 60;
}
nux_u32_t
nux_time_frame (nux_ctx_t *ctx)
{
    return ctx->frame;
}
nux_u64_t
nux_time_timestamp (nux_ctx_t *ctx)
{
    return ctx->stats[NUX_STAT_TIMESTAMP];
}
