#include "internal.h"

#include <input/internal.h>
#include <lua/internal.h>
#include <scene/internal.h>
#include <graphics/internal.h>
#include <physics/internal.h>
#include <ui/internal.h>
#include <debug/internal.h>
#include <nux.h>

static nux_core_module_t _module;

static nux_status_t
bootstrap_core_arena (void)
{
    // Create core arena
    // NOTE: The core arena is a block arena using the os allocator
    // NOTE: The core arena resource entry doesn't have arena

    // 1. Allocate arena + header
    nux_resource_header_t *core_arena_header = nux_os_alloc(
        nullptr, 0, nux_resource_header_size(sizeof(nux_arena_t)));
    NUX_CHECK(core_arena_header, return NUX_FAILURE);
    // 2. Get arena from the header
    _module.core_arena = nux_resource_header_to_data(core_arena_header);
    // 3. Initialize core arena
    nux_arena_init_core(_module.core_arena);
    // 4. Allocate resource table
    nux_pool_init(&_module.resources, _module.core_arena);
    // 5. Reserve index 0 for null id
    nux_pool_add(&_module.resources);
    // 6. Create core arena resource entry
    nux_resource_entry_t *entry
        = nux_resource_add(&_module.resources, NUX_RESOURCE_ARENA);
    NUX_CHECK(entry, return NUX_FAILURE);
    entry->data = _module.core_arena;
    // 7. Initialize core arena header
    nux_resource_header_init(core_arena_header, entry->rid);

    return NUX_SUCCESS;
}

static void
module_pre_update (void)
{
    nux_os_stats_update(_module.stats);
}
static void
module_post_update (void)
{
    nux_arena_clear(nux_arena_frame());
    _module.time_elapsed += nux_time_delta();
    ++_module.frame;
    nux_os_stats_update(_module.stats);
}

nux_core_module_t *
nux_core (void)
{
    return &_module;
}

nux_status_t
nux_core_init (void)
{
    nux_memset(&_module, 0, sizeof(_module));

    // Bootstrap core arena
    bootstrap_core_arena();

    // Initialize modules
    nux_vec_init_capa(
        &_module.modules, _module.core_arena, DEFAULT_MODULE_CAPACITY);
    // Initialize systems
    nux_vec_init_capa(
        &_module.systems, _module.core_arena, DEFAULT_MODULE_CAPACITY);

    // Initialize system state
    _module.config.log.level
        = NUX_LOG_DEBUG; // see errors during initialization
    _module.running      = true;
    _module.pcg          = nux_pcg(10243124, 1823719241);
    _module.error_enable = true;
    nux_error_reset();

    // Initialize configuration
    _module.config.hotreload                       = false;
    _module.config.log.level                       = NUX_LOG_INFO;
    _module.config.window.enable                   = true;
    _module.config.window.width                    = 900;
    _module.config.window.height                   = 400;
    _module.config.scene.enable                    = true;
    _module.config.physics.enable                  = true;
    _module.config.graphics.transforms_buffer_size = 8192;
    _module.config.graphics.batches_buffer_size    = 8192;
    _module.config.graphics.vertices_buffer_size   = (1 << 18) * 5;
    _module.config.debug.enable                    = true;
    _module.config.debug.console                   = true;

    // Register systems
    nux_system_register(NUX_SYSTEM_PRE_UPDATE, module_pre_update);
    nux_system_register(NUX_SYSTEM_POST_UPDATE, module_post_update);
    // Register types
    nux_resource_register(NUX_RESOURCE_NULL,
                          (nux_resource_info_t) { .name = "null", .size = 0 });
    nux_resource_register(
        NUX_RESOURCE_ARENA,
        (nux_resource_info_t) { .name    = "arena",
                                .size    = sizeof(nux_arena_t),
                                .cleanup = nux_arena_cleanup });
    nux_resource_register(
        NUX_RESOURCE_EVENT,
        (nux_resource_info_t) { .name = "event", .size = sizeof(nux_event_t) });

    // Create frame arena
    _module.frame_arena = nux_arena_new(_module.core_arena);
    NUX_ASSERT(_module.frame_arena);
    nux_resource_set_name(_module.frame_arena, "frame_arena");

    // Initialize IO
    NUX_CHECK(nux_io_init(), goto cleanup);

    // Register modules
    nux_input_module_register();
    nux_scene_module_register();
    nux_graphics_module_register();
    nux_physics_module_register();
    nux_ui_module_register();
    nux_debug_module_register();
    nux_lua_module_register();

    // Init modules
    nux_module_init_all();

    // // Detect entry point type
    // NUX_ASSERT(entry);
    // nux_c8_t normpath[NUX_PATH_BUF_SIZE];
    // nux_path_normalize(normpath, entry);
    // const nux_c8_t *entry_script;
    // if (nux_path_endswith(normpath, ".lua"))
    // {
    //     // Direct script loading
    //     entry_script = normpath;
    // }
    // else
    // {
    //     // Expect cartridge
    //     NUX_CHECK(nux_disk_mount(normpath), goto cleanup);
    //     entry_script = NUX_LUA_INIT_FILE;
    // }
    //
    // // Get program configuration
    // nux_config_t *config = (nux_config_t *)nux_config();
    // NUX_CHECK(nux_lua_configure(config), goto cleanup);
    //
    // // Apply configuration
    // nux_log_set_level(nux_config()->log.level);
    //
    // // Register entry script
    // nux_lua_t *lua = nux_lua_load(nux_arena_core(), entry_script);
    // NUX_CHECK(lua, goto cleanup);

    return NUX_SUCCESS;

cleanup:
    if (!nux_error_status())
    {
        NUX_ERROR("%s", nux_error_message());
    }
    nux_core_free();
    return NUX_FAILURE;
}
void
nux_core_free (void)
{
    // Cleanup all resources
    nux_arena_clear(nux_arena_core());
    // Free all modules
    nux_module_free_all();
    // Free IO
    nux_io_free();
    // Free core memory
    nux_arena_free(_module.core_arena);
    // Free core arena
    nux_resource_header_t *core_arena_header
        = nux_resource_header_from_data(_module.core_arena);
    nux_os_alloc(
        core_arena_header, nux_resource_header_size(sizeof(nux_arena_t)), 0);
}
void
nux_core_update (void)
{
    // Main loop
    nux_system_call(NUX_SYSTEM_PRE_UPDATE);
    nux_system_call(NUX_SYSTEM_UPDATE);
    nux_system_call(NUX_SYSTEM_POST_UPDATE);

    // Check errors
    if (!nux_error_status())
    {
        NUX_ERROR("%s", nux_error_message());
    }

    // Hotreload
    if (nux_config_get()->hotreload)
    {
        nux_u32_t count;
        nux_rid_t handles[256];
        nux_os_hotreload_pull(handles, &count);
        for (nux_u32_t i = 0; i < count; ++i)
        {
            nux_resource_reload(handles[i]);
        }
    }
}

nux_pcg_t *
nux_core_pcg (void)
{
    return &_module.pcg;
}
nux_resource_pool_t *
nux_core_resources (void)
{
    return &_module.resources;
}
nux_resource_type_t *
nux_core_resource_types (void)
{
    return _module.resources_types;
}

nux_config_t *
nux_config_get (void)
{
    return &_module.config;
}

nux_u32_t
nux_stat_get (nux_stat_t stat)
{
    return _module.stats[stat];
}
nux_f32_t
nux_time_elapsed (void)
{
    return _module.time_elapsed;
}
nux_f32_t
nux_time_delta (void)
{
    return 1. / 60;
}
nux_u32_t
nux_time_frame (void)
{
    return _module.frame;
}
nux_u64_t
nux_time_epoch (void)
{
    return _module.stats[NUX_STAT_TIMESTAMP];
}

void
nux_error_report (const nux_c8_t *fmt, ...)
{
    if (_module.error_enable)
    {
        va_list args;
        va_start(args, fmt);
        nux_vsnprintf(
            _module.error_message, sizeof(_module.error_message), fmt, args);
        va_end(args);
#ifdef NUX_BUILD_DEBUG
        NUX_ERROR("%s", nux_error_message());
        NUX_ASSERT(false);
#endif
        _module.error_status = NUX_FAILURE;
    }
}
void
nux_error_enable (void)
{
    _module.error_enable = true;
}
void
nux_error_disable (void)
{
    _module.error_enable = false;
}
void
nux_error_reset (void)
{
    _module.error_status = NUX_SUCCESS;
    nux_memset(_module.error_message, 0, sizeof(_module.error_message));
}
const nux_c8_t *
nux_error_message (void)
{
    return _module.error_message;
}
nux_status_t
nux_error_status (void)
{
    return _module.error_status;
}

nux_arena_t *
nux_arena_core (void)
{
    return _module.core_arena;
}
nux_arena_t *
nux_arena_frame (void)
{
    return _module.frame_arena;
}
