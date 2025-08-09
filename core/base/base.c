#include "nux_internal.h"

nux_status_t
nux_base_init (nux_ctx_t *ctx, const nux_init_info_t *info)
{
    ctx->userdata      = info->userdata;
    ctx->running       = NUX_TRUE;
    ctx->init_callback = info->hooks.init;
    ctx->tick_callback = info->hooks.tick;
    nux_error_reset(ctx);

    // Register base types
    // Must be coherent with nux_type_base_t
    ctx->resources_types_count = 0;
    nux_resource_type_t *type;
    type            = nux_res_register(ctx, "null");
    type            = nux_res_register(ctx, "arena");
    type            = nux_res_register(ctx, "lua");
    type->hotreload = nux_lua_hotreload;
    type            = nux_res_register(ctx, "texture");
    type->cleanup   = nux_texture_cleanup;
    type            = nux_res_register(ctx, "mesh");
    type            = nux_res_register(ctx, "file");
    type->cleanup   = nux_file_cleanup;
    type            = nux_res_register(ctx, "ecs");
    type->cleanup   = nux_ecs_cleanup;
    type            = nux_res_register(ctx, "ecs_iter");

    // Create resource pool
    NUX_CHECK(
        nux_resource_pool_alloc(ctx, &ctx->core_arena, 1024, &ctx->resources),
        return NUX_FAILURE);

    // Reserve index 0 for null id
    NUX_ASSERT(nux_resource_pool_add(&ctx->resources));

    // Allocate arena pool
    NUX_CHECK(nux_arena_pool_alloc(ctx, &ctx->core_arena, 32, &ctx->arenas),
              return NUX_FAILURE);

    // Initialize system state
    ctx->log_level    = NUX_LOG_DEBUG;
    ctx->error_enable = NUX_TRUE;
    ctx->pcg          = nux_pcg(10243124, 1823719241);

    // Register core arena
    ctx->core_arena.self = nux_res_create(ctx, NUX_RES_ARENA, &ctx->core_arena);
    NUX_ASSERT(ctx->core_arena.self);

    // Register frame arena
    ctx->frame_arena = nux_arena_new(ctx, "frame_arena", NUX_MEM_16M);
    NUX_CHECK(ctx->frame_arena, return NUX_FAILURE);

    // Initialize controllers
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller      = ctx->controllers + i;
        controller->mode                  = NUX_CONTROLLER_MODE_MOTION;
        controller->cursor_motion_axis[0] = NUX_AXIS_LEFTX;
        controller->cursor_motion_axis[1] = NUX_AXIS_LEFTY;
        controller->cursor_motion_speed   = 100;
    }

    return NUX_SUCCESS;
}
void
nux_base_free (nux_ctx_t *ctx)
{
}
