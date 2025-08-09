#include "nux_internal.h"

void
nux_error (nux_ctx_t *ctx, const nux_c8_t *fmt, ...)
{
    if (ctx->error_enable)
    {
        va_list args;
        va_start(args, fmt);
        nux_vsnprintf(
            ctx->error_message, sizeof(ctx->error_message), fmt, args);
        va_end(args);
#ifdef NUX_BUILD_DEBUG
        NUX_ERROR("%s", nux_error_get_message(ctx));
        NUX_ASSERT(NUX_FALSE);
#endif
        ctx->error_status = NUX_FAILURE;
    }
}
void
nux_error_enable (nux_ctx_t *ctx)
{
    ctx->error_enable = NUX_TRUE;
}
void
nux_error_disable (nux_ctx_t *ctx)
{
    ctx->error_enable = NUX_FALSE;
}
void
nux_error_reset (nux_ctx_t *ctx)
{
    ctx->error_status = NUX_SUCCESS;
    nux_memset(ctx->error_message, 0, sizeof(ctx->error_message));
}
const nux_c8_t *
nux_error_get_message (nux_ctx_t *ctx)
{
    return ctx->error_message;
}
nux_status_t
nux_error_get_status (nux_ctx_t *ctx)
{
    return ctx->error_status;
}

nux_ctx_t *
nux_instance_init (const nux_init_info_t *info)
{
    // Allocate core memory
    nux_arena_t core_arena;
    core_arena.capa = NUX_MEM_8M;
    core_arena.size = 0;
    core_arena.data
        = nux_os_alloc(info->userdata, NUX_NULL, 0, core_arena.capa);
    core_arena.first_resource = NUX_NULL;
    core_arena.last_resource  = NUX_NULL;
    nux_strncpy(core_arena.name, "core_arena", sizeof(core_arena.name) - 1);
    if (!core_arena.data)
    {
        return NUX_NULL;
    }
    nux_memset(core_arena.data, 0, core_arena.capa);

    // Initialize context
    nux_ctx_t *ctx = nux_arena_alloc_raw(NUX_NULL, &core_arena, sizeof(*ctx));
    NUX_ASSERT(ctx);
    ctx->core_arena = core_arena;

    // Initialize mandatory modules
    NUX_CHECK(nux_base_init(ctx, info), goto cleanup);
    NUX_CHECK(nux_io_init(ctx, info), goto cleanup);
    NUX_CHECK(nux_lua_init(ctx), goto cleanup);

    // Detect entry point type
    NUX_ASSERT(info->entry);
    nux_c8_t normpath[NUX_PATH_BUF_SIZE];
    nux_path_normalize(normpath, info->entry);
    const nux_c8_t *entry_script;
    if (nux_path_endswith(normpath, ".lua"))
    {
        // Direct script loading
        entry_script = normpath;
    }
    else
    {
        // Expect cartridge
        NUX_CHECK(nux_io_mount(ctx, normpath), goto cleanup);
        entry_script = "init.lua";
    }

    // Get program configuration
    ctx->config.hotreload              = NUX_FALSE;
    ctx->config.arena.global_capacity  = NUX_MEM_1M;
    ctx->config.arena.frame_capacity   = NUX_MEM_1M;
    ctx->config.arena.scratch_capacity = NUX_MEM_1M;
    ctx->config.window.enable          = NUX_TRUE;
    ctx->config.window.width           = 900;
    ctx->config.window.height          = 400;
    ctx->config.ecs.enable             = NUX_TRUE;
    ctx->config.physics.enable         = NUX_TRUE;
    NUX_CHECK(nux_lua_configure(ctx, entry_script, &ctx->config), goto cleanup);

    // Initialize optional modules
    NUX_CHECK(nux_graphics_init(ctx), goto cleanup);
    if (ctx->config.ecs.enable)
    {
        NUX_CHECK(nux_ecs_init(ctx), goto cleanup);
    }
    if (ctx->config.physics.enable)
    {
        NUX_CHECK(nux_physics_init(ctx), goto cleanup);
    }

    return ctx;

cleanup:
    if (!nux_error_get_status(ctx))
    {
        NUX_ERROR("%s", nux_error_get_message(ctx));
    }
    nux_instance_free(ctx);
    return NUX_NULL;
}
void
nux_instance_free (nux_ctx_t *ctx)
{
    // Cleanup all resources
    nux_arena_reset_raw(ctx, &ctx->core_arena);

    // Reset runtime
    nux_physics_free(ctx);
    nux_graphics_free(ctx);
    nux_ecs_free(ctx);
    nux_lua_free(ctx);
    nux_io_free(ctx);
    nux_base_free(ctx);

    // Free core memory
    if (ctx->core_arena.data)
    {
        nux_os_alloc(ctx->userdata, ctx->core_arena.data, 0, 0);
    }
}
void
nux_instance_tick (nux_ctx_t *ctx)
{
    // Init
    if (ctx->frame == 0)
    {
        if (ctx->init_callback)
        {
            ctx->init_callback(ctx);
        }
        nux_lua_call_init(ctx);
    }

    // Update stats
    nux_os_stats_update(ctx->userdata, ctx->stats);

    // Update inputs
    nux_input_update(ctx);

    // Update
    if (ctx->tick_callback)
    {
        ctx->tick_callback(ctx);
    }
    nux_lua_call_tick(ctx);

    // Error handling
    if (!nux_error_get_status(ctx))
    {
        NUX_ERROR("%s", nux_error_get_message(ctx));
    }
    nux_error_reset(ctx);

    // Render
    nux_graphics_render(ctx);

    // Reset frame arena
    nux_arena_reset(ctx, ctx->frame_arena);

    // Frame integration
    ctx->time_elapsed += nux_time_delta(ctx);
    ++ctx->frame;
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
