#include <base/internal.h>
#include <io/internal.h>
#include <lua/internal.h>
#include <ecs/internal.h>
#include <graphics/internal.h>
#include <physics/internal.h>
#include <debug/internal.h>

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
nux_instance_init (void *userdata, const nux_c8_t *entry)
{
    // Initialize context
    nux_ctx_t *ctx = nux_os_alloc(userdata, NUX_NULL, 0, sizeof(*ctx));
    NUX_ASSERT(ctx);
    nux_memset(ctx, 0, sizeof(*ctx));
    ctx->userdata = userdata;

    // Initialize mandatory modules
    NUX_CHECK(nux_base_init(ctx), goto cleanup);
    NUX_CHECK(nux_io_init(ctx), goto cleanup);
    NUX_CHECK(nux_lua_init(ctx), goto cleanup);

    // Detect entry point type
    NUX_ASSERT(entry);
    nux_c8_t normpath[NUX_PATH_BUF_SIZE];
    nux_path_normalize(normpath, entry);
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
        entry_script = NUX_INIT_FILE;
    }

    // Get program configuration
    ctx->config.hotreload                       = NUX_FALSE;
    ctx->config.log.level                       = NUX_LOG_INFO;
    ctx->config.window.enable                   = NUX_TRUE;
    ctx->config.window.width                    = 900;
    ctx->config.window.height                   = 400;
    ctx->config.ecs.enable                      = NUX_TRUE;
    ctx->config.physics.enable                  = NUX_TRUE;
    ctx->config.graphics.transforms_buffer_size = 8192;
    ctx->config.graphics.batches_buffer_size    = 8192;
    ctx->config.graphics.vertices_buffer_size   = 1 << 18;
    ctx->config.graphics.encoder_size           = 8192;
    ctx->config.graphics.immediate_encoder_size = 8192;
    ctx->config.debug.enable                    = NUX_TRUE;
    ctx->config.debug.console                   = NUX_TRUE;
    NUX_CHECK(nux_lua_configure(ctx, &ctx->config), goto cleanup);

    // Initialize optional modules
    NUX_CHECK(nux_ecs_init(ctx), goto cleanup);
    NUX_CHECK(nux_graphics_init(ctx), goto cleanup);
    NUX_CHECK(nux_physics_init(ctx), goto cleanup);
    NUX_CHECK(nux_debug_init(ctx), goto cleanup);

    // Register entry script
    nux_rid_t rid = nux_lua_load(ctx, ctx->core_arena_rid, entry_script);
    NUX_CHECK(rid, goto cleanup);

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
    nux_arena_reset(ctx, ctx->core_arena_rid);

    // Reset runtime
    nux_debug_free(ctx);
    nux_physics_free(ctx);
    nux_graphics_free(ctx);
    nux_ecs_free(ctx);
    nux_lua_free(ctx);
    nux_io_free(ctx);
    nux_base_free(ctx);

    // Free core memory
    nux_arena_free(ctx, &ctx->core_arena);
    nux_os_alloc(ctx->userdata, ctx, 0, 0);
}
void
nux_instance_tick (nux_ctx_t *ctx)
{
    // Update stats
    nux_os_stats_update(ctx->userdata, ctx->stats);

    // Prepare render
    nux_graphics_pre_update(ctx);

    // Update inputs
    nux_input_update(ctx);

    // Update physics
    nux_physics_update(ctx);

    // Update
    nux_rid_t rid = NUX_NULL;
    while ((rid = nux_resource_next(ctx, NUX_RESOURCE_LUA, rid)))
    {
        nux_lua_call_module(ctx, rid, NUX_FUNC_TICK);
    }

    // Update debug
    nux_debug_update(ctx);

    // Error handling
    if (!nux_error_get_status(ctx))
    {
        NUX_ERROR("%s", nux_error_get_message(ctx));
    }
    nux_error_reset(ctx);

    // Render
    nux_graphics_update(ctx);

    // Hotreload
    if (ctx->config.hotreload)
    {
        nux_u32_t count;
        nux_rid_t handles[256];
        nux_os_hotreload_pull(ctx->userdata, handles, &count);
        for (nux_u32_t i = 0; i < count; ++i)
        {
            nux_resource_reload(ctx, handles[i]);
        }
    }

    // Frame integration
    ctx->time_elapsed += nux_time_delta(ctx);
    ++ctx->frame;
}
