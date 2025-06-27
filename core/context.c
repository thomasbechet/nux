#include "internal.h"

void
nux_error (nux_ctx_t *ctx, nux_error_t error)
{
    ctx->error = error;
    nux_snprintf(ctx->error_message, sizeof(ctx->error_message), "%s", "TODO");
}

nux_ctx_t *
nux_instance_init (const nux_config_t *config)
{
    NUX_ASSERT(config->max_object_count);
    NUX_ASSERT(config->memory_size);

    // Allocate core memory
    nux_arena_t core_arena;
    core_arena.capa = NUX_CORE_MEMORY_SIZE;
    core_arena.size = 0;
    core_arena.data = nux_os_malloc(config->userdata, NUX_CORE_MEMORY_SIZE);
    core_arena.first_object = NUX_NULL;
    core_arena.last_object  = NUX_NULL;
    NUX_CHECK(core_arena.data, return NUX_NULL);
    nux_memset(core_arena.data, 0, NUX_CORE_MEMORY_SIZE);

    // Allocate instance
    nux_ctx_t *ctx = nux_arena_alloc(&core_arena, sizeof(*ctx));
    NUX_ASSERT(ctx);
    ctx->userdata = config->userdata;
    ctx->running  = NUX_TRUE;
    ctx->init     = config->init;
    ctx->update   = config->update;

    // Register base objects
    ctx->object_types_count = 0;
    nux_object_register(ctx, "null", NUX_NULL);
    nux_object_register(ctx, "arena", NUX_NULL);
    nux_object_register(ctx, "lua", NUX_NULL);
    nux_object_register(ctx, "texture", nux_texture_cleanup);
    nux_object_register(ctx, "mesh", NUX_NULL);
    nux_object_register(ctx, "scene", nux_scene_cleanup);
    nux_object_register(ctx, "entity", NUX_NULL);
    nux_object_register(ctx, "transform", NUX_NULL);
    nux_object_register(ctx, "camera", NUX_NULL);

    // Create object pool
    NUX_CHECK(nux_object_pool_alloc(
                  &core_arena, config->max_object_count, &ctx->objects),
              goto cleanup);

    // Reserve index 0 for null object
    nux_object_pool_add(&ctx->objects);

    // Register core arena
    NUX_CHECK(nux_arena_pool_alloc(&core_arena, 32, &ctx->arenas),
              goto cleanup);
    ctx->core_arena   = nux_arena_pool_add(&ctx->arenas);
    *ctx->core_arena  = core_arena; // copy by value
    ctx->active_arena = ctx->core_arena;

    // Register core arena object
    ctx->core_arena->id = nux_object_create(
        ctx, ctx->active_arena, NUX_OBJECT_ARENA, ctx->core_arena);

    // Initialize error state
    ctx->error            = NUX_ERROR_NONE;
    ctx->error_message[0] = '\0';

    // Load configuration
    NUX_CHECK(nux_lua_load_conf(ctx), goto cleanup);

    // Initialize modules
    NUX_CHECK(nux_graphics_init(ctx), goto cleanup);
    NUX_CHECK(nux_io_init(ctx), goto cleanup);
    NUX_CHECK(nux_lua_init(ctx), goto cleanup);

    return ctx;

cleanup:
    nux_instance_free(ctx);
    return NUX_NULL;
}
void
nux_instance_free (nux_ctx_t *ctx)
{
    nux_arena_reset_to(ctx, ctx->core_arena, NUX_NULL);

    // Free modules
    nux_lua_free(ctx);
    nux_io_free(ctx);
    nux_graphics_free(ctx);

    // Free core memory
    if (ctx->core_arena->data)
    {
        nux_os_free(ctx->userdata, ctx->core_arena->data);
    }
}
void
nux_instance_tick (nux_ctx_t *ctx)
{
    // Init draw state
    nux_graphics_cursor(ctx, 0, 0);

    // Init
    if (ctx->frame == 0 && ctx->init)
    {
        ctx->init(ctx);
    }

    // Update stats
    nux_os_update_stats(ctx->userdata, ctx->stats);

    // Keep previous input state
    nux_input_pre_update(ctx);

    // Update inputs
    nux_os_update_inputs(ctx->userdata, ctx->buttons, ctx->axis);

    // Update
    if (ctx->update)
    {
        ctx->update(ctx);
    }

    // Update lua
    nux_lua_tick(ctx);

    // Render
    nux_graphics_render(ctx);

    // Frame integration
    ctx->time += nux_dt(ctx);
    ++ctx->frame;
}
nux_status_t
nux_instance_load (nux_ctx_t *ctx, const nux_c8_t *cart, nux_u32_t n)
{
    return NUX_SUCCESS;
}

void
nux_trace (nux_ctx_t *ctx, const nux_c8_t *text)
{
    nux_log(ctx, NUX_LOG_INFO, text, nux_strnlen(text, 1024));
}

nux_u32_t
nux_stat (nux_ctx_t *ctx, nux_stat_t stat)
{
    return ctx->stats[stat];
}
nux_f32_t
nux_time (nux_ctx_t *ctx)
{
    return ctx->time;
}
nux_f32_t
nux_dt (nux_ctx_t *ctx)
{
    return 1. / 60;
}
nux_u32_t
nux_frame (nux_ctx_t *ctx)
{
    return ctx->frame;
}
