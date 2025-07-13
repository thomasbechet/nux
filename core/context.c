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
    NUX_ASSERT(config->max_ref_count);
    NUX_ASSERT(config->memory_size);

    // Allocate core memory
    nux_arena_t core_arena;
    core_arena.capa = config->memory_size;
    core_arena.size = 0;
    core_arena.data = nux_os_malloc(config->userdata, config->memory_size);
    core_arena.first_type = NUX_NULL;
    core_arena.last_type  = NUX_NULL;
    NUX_CHECK(core_arena.data, return NUX_NULL);
    nux_memset(core_arena.data, 0, config->memory_size);

    // Allocate instance
    nux_ctx_t *ctx = nux_arena_alloc_raw(&core_arena, sizeof(*ctx));
    NUX_ASSERT(ctx);
    ctx->active_arena = &core_arena;
    ctx->userdata     = config->userdata;
    ctx->running      = NUX_TRUE;
    ctx->init         = config->init;
    ctx->update       = config->update;

    // Register base types
    // Must be coherent with nux_type_base_t
    ctx->types_count = 0;
    nux_type_t *type;
    type = nux_type_register(ctx, "null");

    type = nux_type_register(ctx, "arena");

    type = nux_type_register(ctx, "lua");

    type          = nux_type_register(ctx, "texture");
    type->cleanup = nux_texture_cleanup;

    type = nux_type_register(ctx, "mesh");

    type          = nux_type_register(ctx, "scene");
    type->cleanup = nux_scene_cleanup;

    type = nux_type_register(ctx, "node");

    type                 = nux_type_register(ctx, "transform");
    type->component_type = NUX_COMPONENT_TRANSFORM;

    type                 = nux_type_register(ctx, "camera");
    type->component_type = NUX_COMPONENT_CAMERA;

    type                 = nux_type_register(ctx, "staticmesh");
    type->component_type = NUX_COMPONENT_STATICMESH;

    // Register base component types
    // Must be coherent with nux_component_type_base_t
    ctx->types_count = 0;
    nux_component_register(ctx, NUX_TYPE_TRANSFORM);
    nux_component_register(ctx, NUX_TYPE_CAMERA);
    nux_component_register(ctx, NUX_TYPE_STATICMESH);

    // Create references pool
    NUX_CHECK(nux_ref_pool_alloc(ctx, config->max_ref_count, &ctx->refs),
              goto cleanup);

    // Reserve index 0 for null reference
    nux_ref_pool_add(&ctx->refs);

    // Register core arena
    NUX_CHECK(nux_arena_pool_alloc(ctx, 32, &ctx->arenas), goto cleanup);
    ctx->core_arena   = nux_arena_pool_add(&ctx->arenas);
    *ctx->core_arena  = core_arena; // copy by value
    ctx->active_arena = ctx->core_arena;

    // Register core arena object
    ctx->core_arena->ref = nux_ref_create(ctx, NUX_TYPE_ARENA, ctx->core_arena);

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
    nux_arena_rewind(ctx, ctx->core_arena);

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
