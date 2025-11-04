#include <base/internal.h>
#include <io/internal.h>
#include <input/internal.h>
#include <lua/internal.h>
#include <scene/internal.h>
#include <graphics/internal.h>
#include <physics/internal.h>
#include <gui/internal.h>
#include <debug/internal.h>

nux_status_t
nux_core_init (void *userdata, const nux_c8_t *entry)
{
    // Initialize base
    NUX_CHECK(nux_base_init(userdata), goto cleanup);

    // Register modules
    nux_io_module_register();
    nux_input_module_register();
    nux_lua_module_register();
    nux_scene_module_register();
    nux_graphics_module_register();
    nux_physics_module_register();
    nux_gui_module_register();
    nux_debug_module_register();

    NUX_CHECK(nux_modules_init(), goto cleanup);

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
        NUX_CHECK(nux_disk_mount(normpath), goto cleanup);
        entry_script = NUX_LUA_INIT_FILE;
    }

    // Get program configuration
    nux_config_t *config = (nux_config_t *)nux_config();
    NUX_CHECK(nux_lua_configure(config), goto cleanup);

    // Apply configuration
    nux_log_set_level(nux_config()->log.level);

    // Register entry script
    nux_lua_t *lua = nux_lua_load(nux_arena_core(), entry_script);
    NUX_CHECK(lua, goto cleanup);

    return NUX_SUCCESS;

cleanup:
    if (!nux_error_get_status())
    {
        NUX_ERROR("%s", nux_error_get_message());
    }
    nux_core_free();
    return NUX_NULL;
}
void
nux_core_free (void)
{
    // Cleanup all resources
    nux_arena_clear(nux_arena_core());

    // Free modules
    nux_modules_free();
}
void
nux_core_update (void)
{
    // Pre update
    nux_modules_pre_update();

    // Update
    nux_modules_update();
    if (!nux_error_get_status())
    {
        NUX_ERROR("%s", nux_error_get_message());
    }

    // Hotreload
    if (nux_config()->hotreload)
    {
        nux_u32_t count;
        nux_rid_t handles[256];
        nux_os_hotreload_pull(nux_userdata(), handles, &count);
        for (nux_u32_t i = 0; i < count; ++i)
        {
            nux_resource_reload(handles[i]);
        }
    }

    // Post update
    nux_modules_post_update();
}
void
nux_core_push_event (nux_os_event_t *event)
{
    nux_modules_on_event(event);
}
