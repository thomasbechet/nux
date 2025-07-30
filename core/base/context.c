#include "internal.h"

void
nux_error (nux_ctx_t *ctx, const nux_c8_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nux_vsnprintf(ctx->error_message, sizeof(ctx->error_message), fmt, args);
    va_end(args);
    ctx->error_status = NUX_FAILURE;
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
nux_instance_init (const nux_config_t *config)
{
    NUX_ASSERT(config->max_id_count);
    NUX_ASSERT(config->memory_size);

    // Allocate core memory
    nux_arena_t core_arena;
    core_arena.capa = config->memory_size;
    core_arena.size = 0;
    core_arena.data
        = nux_os_alloc(config->userdata, NUX_NULL, 0, config->memory_size);
    core_arena.first_resource = NUX_NULL;
    core_arena.last_resource  = NUX_NULL;
    if (!core_arena.data)
    {
        return NUX_NULL;
    }
    nux_memset(core_arena.data, 0, config->memory_size);

    // Allocate instance
    nux_ctx_t *ctx = nux_arena_alloc_raw(NUX_NULL, &core_arena, sizeof(*ctx));
    NUX_ASSERT(ctx);
    ctx->userdata = config->userdata;
    ctx->running  = NUX_TRUE;
    ctx->init     = config->init;
    ctx->update   = config->update;

    // Initialize state
    nux_error_reset(ctx);

    // Register base types
    // Must be coherent with nux_type_base_t
    ctx->resources_types_count = 0;
    nux_resource_type_t *type;

    type          = nux_res_register(ctx, "null");
    type          = nux_res_register(ctx, "arena");
    type          = nux_res_register(ctx, "lua");
    type          = nux_res_register(ctx, "texture");
    type->cleanup = nux_texture_cleanup;
    type          = nux_res_register(ctx, "mesh");
    type          = nux_res_register(ctx, "scene");
    type->cleanup = nux_scene_cleanup;
    type          = nux_res_register(ctx, "node");
    type          = nux_res_register(ctx, "file");
    type->cleanup = nux_file_cleanup;
    type          = nux_res_register(ctx, "ecs");
    type          = nux_res_register(ctx, "ecs_iter");

    type                 = nux_res_register(ctx, "transform");
    type->component_type = NUX_COMPONENT_TRANSFORM;
    type                 = nux_res_register(ctx, "camera");
    type->component_type = NUX_COMPONENT_CAMERA;
    type                 = nux_res_register(ctx, "staticmesh");
    type->component_type = NUX_COMPONENT_STATICMESH;

    // Register base component types
    // Must be coherent with nux_component_type_base_t
    ctx->resources_types_count = 0;
    nux_component_register(ctx, NUX_RES_TRANSFORM);
    nux_component_register(ctx, NUX_RES_CAMERA);
    nux_component_register(ctx, NUX_RES_STATICMESH);

    nux_ecs_register_component(ctx, "transform", sizeof(nux_transform_t));
    nux_ecs_register_component(ctx, "camera", sizeof(nux_camera_t));
    nux_ecs_register_component(ctx, "staticmesh", sizeof(nux_staticmesh_t));

    // Create resource pool
    NUX_CHECK(nux_resource_pool_alloc(
                  ctx, &core_arena, config->max_id_count, &ctx->resources),
              goto cleanup);

    // Reserve index 0 for null id
    nux_resource_pool_add(&ctx->resources);

    // Allocate arena pool
    NUX_CHECK(nux_arena_pool_alloc(ctx, &core_arena, 32, &ctx->arenas),
              goto cleanup);

    // Register core arena object
    ctx->core_arena       = nux_arena_pool_add(&ctx->arenas);
    *ctx->core_arena      = core_arena; // copy by value
    ctx->core_arena->self = nux_res_create(ctx, NUX_RES_ARENA, ctx->core_arena);

    // Register frame arena
    ctx->frame_arena = nux_arena_new(ctx, NUX_MEM_16M);
    NUX_CHECK(ctx->frame_arena, goto cleanup);

    // Initialize PCG
    ctx->pcg = nux_pcg(10243124, 1823719241);

    // Initialize core modules
    NUX_CHECK(nux_io_init(ctx), goto cleanup);

    // Mount base disk
    if (config->boot_device)
    {
        NUX_CHECK(nux_io_mount(ctx, config->boot_device), goto cleanup);
    }

    // Configure
    NUX_CHECK(nux_lua_configure(ctx), goto cleanup);

    // Initialize modules
    NUX_CHECK(nux_graphics_init(ctx), goto cleanup);
    NUX_CHECK(nux_lua_init(ctx), goto cleanup);

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
    nux_arena_reset(ctx, ctx->core_arena->self);

    // Free modules
    nux_lua_free(ctx);
    nux_io_free(ctx);
    nux_graphics_free(ctx);

    // Free core memory
    if (ctx->core_arena->data)
    {
        nux_os_alloc(ctx->userdata, ctx->core_arena->data, 0, 0);
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
    nux_os_stats_update(ctx->userdata, ctx->stats);

    // Keep previous input state
    nux_input_pre_update(ctx);

    // Update inputs
    nux_os_input_update(ctx->userdata, ctx->buttons, ctx->axis);

    // Update
    if (ctx->update)
    {
        ctx->update(ctx);
    }

    // Update lua
    nux_lua_tick(ctx);
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
