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

    // Register base types
    nux_resource_type_t *type;
    type = nux_resource_register(NUX_RESOURCE_NULL, 0, "null");
    type = nux_resource_register(
        NUX_RESOURCE_ARENA, sizeof(nux_arena_t), "arena");
    type->cleanup = nux_arena_cleanup;
    type          = nux_resource_register(
        NUX_RESOURCE_EVENT, sizeof(nux_event_t), "event");
    type = nux_resource_register(NUX_RESOURCE_FILE, sizeof(nux_file_t), "file");
    type->cleanup = nux_file_cleanup;
    type = nux_resource_register(NUX_RESOURCE_DISK, sizeof(nux_disk_t), "disk");
    type->cleanup = nux_disk_cleanup;
    type          = nux_resource_register(
        NUX_RESOURCE_INPUTMAP, sizeof(nux_inputmap_t), "inputmap");

    // Initialize resources with core arena
    NUX_CHECK(nux_resource_init(), return NUX_FAILURE);

    // Create frame arena
    module->frame_arena = nux_arena_new(module->core_arena);
    NUX_ASSERT(module->frame_arena);

    // Initialize system state
    module->error_enable = NUX_TRUE;
    module->pcg          = nux_pcg(10243124, 1823719241);

    // Initialize configuration
    module->config.hotreload                       = NUX_FALSE;
    module->config.log.level                       = NUX_LOG_INFO;
    module->config.window.enable                   = NUX_TRUE;
    module->config.window.width                    = 900;
    module->config.window.height                   = 400;
    module->config.scene.enable                    = NUX_TRUE;
    module->config.physics.enable                  = NUX_TRUE;
    module->config.graphics.transforms_buffer_size = 8192;
    module->config.graphics.batches_buffer_size    = 8192;
    module->config.graphics.vertices_buffer_size   = (1 << 18) * 5;
    module->config.debug.enable                    = NUX_TRUE;
    module->config.debug.console                   = NUX_TRUE;

    // Initialize files
    NUX_CHECK(nux_u32_vec_init_capa(
                  nux_arena_core(), NUX_IO_FILE_MAX, &module->free_file_slots),
              return NUX_FAILURE);

    // Initialize values
    nux_u32_vec_fill_reversed(&module->free_file_slots);

    // Allocate events queue
    NUX_CHECK(nux_os_event_vec_init_capa(module->core_arena,
                                         NUX_BASE_DEFAULT_EVENT_SIZE,
                                         &module->events),
              return NUX_FAILURE);

    // Add OS disk
    module->first_disk = nux_resource_new(nux_arena_core(), NUX_RESOURCE_DISK);
    module->first_disk->type = NUX_DISK_OS;

    // Initialize controllers
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller = module->controllers + i;
        controller->inputmap         = NUX_NULL;
        nux_f32_vec_init_capa(module->core_arena,
                              NUX_BASE_DEFAULT_CONTROLLER_INPUT_SIZE,
                              &controller->inputs);
        nux_f32_vec_init_capa(module->core_arena,
                              NUX_BASE_DEFAULT_CONTROLLER_INPUT_SIZE,
                              &controller->prev_inputs);
    }

    return NUX_SUCCESS;
}
void
nux_base_free (void)
{
    nux_base_module_t *module = nux_base_module();
    NUX_ASSERT(module->free_file_slots.size == NUX_IO_FILE_MAX);
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
