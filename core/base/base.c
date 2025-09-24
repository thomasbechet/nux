#include "internal.h"

nux_status_t
nux_base_init (void *userdata)
{
    nux_base_module_t *module = nux_base_module();

    module->userdata = userdata;
    module->config.log.level
        = NUX_LOG_DEBUG; // see errors during initialization
    module->running = NUX_TRUE;
    nux_error_reset();

    // Create core arena
    nux_arena_t core_arena;
    nux_arena_init(&core_arena);

    // Register base types
    nux_resource_type_t *type;
    type = nux_resource_register(NUX_RESOURCE_NULL, 0, "null");
    type = nux_resource_register(
        NUX_RESOURCE_ARENA, sizeof(nux_arena_t), "arena");
    type->cleanup = nux_arena_cleanup;
    type          = nux_resource_register(
        NUX_RESOURCE_EVENT, sizeof(nux_event_t), "event");

    // Create resource pool
    NUX_CHECK(nux_resource_pool_init(&core_arena, &module->resources),
              return NUX_FAILURE);
    // Reserve index 0 for null id
    nux_resource_pool_add(&module->resources);

    // Register core arena
    module->core_arena = nux_resource_new(&core_arena, NUX_RESOURCE_ARENA);
    NUX_ASSERT(module->core_arena);
    nux_memcpy(module->core_arena, &core_arena, sizeof(core_arena));
    nux_resource_set_name(nux_resource_get_rid(module->core_arena),
                          "core_arena");

    // Create frame arena
    module->frame_arena = nux_arena_new(module->core_arena);
    NUX_ASSERT(module->frame_arena);
    nux_resource_set_name(nux_resource_get_rid(module->frame_arena),
                          "frame_arena");

    // Initialize system state
    module->error_enable = NUX_TRUE;
    module->pcg          = nux_pcg(10243124, 1823719241);

    // Initialize configuration
    module->config.hotreload                       = NUX_FALSE;
    module->config.log.level                       = NUX_LOG_INFO;
    module->config.window.enable                   = NUX_TRUE;
    module->config.window.width                    = 900;
    module->config.window.height                   = 400;
    module->config.ecs.enable                      = NUX_TRUE;
    module->config.physics.enable                  = NUX_TRUE;
    module->config.graphics.transforms_buffer_size = 8192;
    module->config.graphics.batches_buffer_size    = 8192;
    module->config.graphics.vertices_buffer_size   = 1 << 18;
    module->config.debug.enable                    = NUX_TRUE;
    module->config.debug.console                   = NUX_TRUE;

    return NUX_SUCCESS;
}
void
nux_base_free (void)
{
}
const nux_config_t *
nux_config (void)
{
    return &nux_base_module()->config;
}
void *
nux_userdata (void)
{
    return nux_base_module()->userdata;
}

nux_u32_t
nux_stat (nux_stat_t stat)
{
    return nux_base_module()->stats[stat];
}
nux_f32_t
nux_time_elapsed (void)
{
    return nux_base_module()->time_elapsed;
}
nux_f32_t
nux_time_delta (void)
{
    return 1. / 60;
}
nux_u32_t
nux_time_frame (void)
{
    return nux_base_module()->frame;
}
nux_u64_t
nux_time_timestamp (void)
{
    return nux_base_module()->stats[NUX_STAT_TIMESTAMP];
}
