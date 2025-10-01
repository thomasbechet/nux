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
    module->config.graphics.vertices_buffer_size   = 1 << 18;
    module->config.debug.enable                    = NUX_TRUE;
    module->config.debug.console                   = NUX_TRUE;

    // Initialize files
    NUX_CHECK(nux_u32_vec_init_capa(
                  nux_arena_core(), NUX_IO_FILE_MAX, &module->free_file_slots),
              return NUX_FAILURE);

    // Initialize values
    nux_u32_vec_fill_reversed(&module->free_file_slots);
    module->disks_count = 0;

    // Add OS disk
    nux_disk_t *disk = module->disks + module->disks_count;
    disk->type       = NUX_DISK_OS;
    ++module->disks_count;

    // Initialize controllers
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller      = module->controllers + i;
        controller->mode                  = NUX_CONTROLLER_MODE_MOTION;
        controller->cursor_motion_axis[0] = NUX_AXIS_LEFTX;
        controller->cursor_motion_axis[1] = NUX_AXIS_LEFTY;
        controller->cursor_motion_speed   = 100;
    }

    return NUX_SUCCESS;
}
void
nux_base_free (void)
{
    nux_base_module_t *module = nux_base_module();

    // Unmount disks
    for (nux_u32_t i = 0; i < module->disks_count; ++i)
    {
        nux_disk_t *disk = module->disks + i;
        if (disk->type == NUX_DISK_CART)
        {
            // close_os_file(disk->cart.slot);
        }
    }

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
