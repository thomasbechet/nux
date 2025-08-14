#include "nux_internal.h"

#include "lua_code.c.inc"

nux_status_t
nux_base_init (nux_ctx_t *ctx)
{
    ctx->running = NUX_TRUE;
    nux_error_reset(ctx);

    // Create core arena
    nux_arena_init(&ctx->core_arena, "core_arena");

    // Register base types
    // Must be coherent with nux_type_base_t
    ctx->resources_types_count = 0;
    nux_resource_type_t *type;
    type          = nux_res_register(ctx, "null");
    type          = nux_res_register(ctx, "arena");
    type->cleanup = nux_arena_cleanup;
    type          = nux_res_register(ctx, "lua");
    type->reload  = nux_lua_reload;
    type          = nux_res_register(ctx, "texture");
    type->cleanup = nux_texture_cleanup;
    type          = nux_res_register(ctx, "mesh");
    type          = nux_res_register(ctx, "canvas");
    type->cleanup = nux_canvas_cleanup;
    type          = nux_res_register(ctx, "font");
    type->cleanup = nux_font_cleanup;
    type          = nux_res_register(ctx, "file");
    type->cleanup = nux_file_cleanup;
    type          = nux_res_register(ctx, "ecs");
    type->cleanup = nux_ecs_cleanup;
    type          = nux_res_register(ctx, "ecs_iter");

    // Create resource pool
    NUX_CHECK(
        nux_resource_pool_alloc(ctx, &ctx->core_arena, 1024, &ctx->resources),
        return NUX_FAILURE);
    // Reserve index 0 for null id
    NUX_ASSERT(nux_resource_pool_add(&ctx->resources));

    // Register core arena
    ctx->core_arena_res
        = nux_res_add(ctx, NUX_NULL, NUX_RES_ARENA, &ctx->core_arena);

    // Create frame arena
    ctx->frame_arena = nux_arena_new(ctx, ctx->core_arena_res, "frame_arena");
    NUX_ASSERT(ctx->frame_arena);

    // Initialize system state
    ctx->error_enable = NUX_TRUE;
    ctx->pcg          = nux_pcg(10243124, 1823719241);

    // Initialize controllers
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller      = ctx->controllers + i;
        controller->mode                  = NUX_CONTROLLER_MODE_MOTION;
        controller->cursor_motion_axis[0] = NUX_AXIS_LEFTX;
        controller->cursor_motion_axis[1] = NUX_AXIS_LEFTY;
        controller->cursor_motion_speed   = 100;
    }

    // Initialize io
    NUX_CHECK(nux_io_init(ctx), return NUX_FAILURE);

    // Initialize lua
    NUX_CHECK(nux_lua_init(ctx), return NUX_FAILURE);

    // Register base API
    nux_lua_open_base(ctx);
    nux_lua_dostring(ctx, lua_data_code);

    return NUX_SUCCESS;
}
void
nux_base_free (nux_ctx_t *ctx)
{
    nux_lua_free(ctx);
    nux_io_free(ctx);

    NUX_ASSERT(ctx->free_file_slots.size == NUX_IO_FILE_MAX);
}
