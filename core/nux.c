#include <base/internal.h>
#include <lua/internal.h>
#include <scene/internal.h>
#include <graphics/internal.h>
#include <physics/internal.h>
#include <gui/internal.h>
#include <debug/internal.h>

static nux_instance_t __thread *_instance;

struct nux_instance_t
{
    nux_base_module_t     base;
    nux_lua_module_t      lua;
    nux_scene_module_t    scene;
    nux_graphics_module_t graphics;
    nux_physics_module_t  physics;
    nux_debug_module_t    debug;
};

static void
setup_thread_context (nux_instance_t *instance)
{
    _instance = instance;
}

nux_base_module_t *
nux_base_module (void)
{
    return &_instance->base;
}
nux_lua_module_t *
nux_lua_module (void)
{
    return &_instance->lua;
}
nux_scene_module_t *
nux_scene_module (void)
{
    return &_instance->scene;
}
nux_graphics_module_t *
nux_graphics_module (void)
{
    return &_instance->graphics;
}
nux_physics_module_t *
nux_physics_module (void)
{
    return &_instance->physics;
}
nux_debug_module_t *
nux_debug_module (void)
{
    return &_instance->debug;
}

nux_instance_t *
nux_instance_init (void *userdata, const nux_c8_t *entry)
{
    // Initialize context
    nux_instance_t *instance
        = nux_os_alloc(userdata, NUX_NULL, 0, sizeof(*instance));
    NUX_ASSERT(instance);
    setup_thread_context(instance);
    nux_memset(instance, 0, sizeof(*instance));

    // Initialize mandatory modules
    NUX_CHECK(nux_base_init(userdata), goto cleanup);
    NUX_CHECK(nux_lua_init(), goto cleanup);

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
    NUX_CHECK(nux_lua_configure(&nux_base_module()->config), goto cleanup);

    // Apply configuration
    nux_log_set_level(nux_base_module()->config.log.level);

    // Initialize optional modules
    NUX_CHECK(nux_scene_init(), goto cleanup);
    NUX_CHECK(nux_graphics_init(), goto cleanup);
    NUX_CHECK(nux_physics_init(), goto cleanup);
    NUX_CHECK(nux_gui_init(), goto cleanup);
    NUX_CHECK(nux_debug_init(), goto cleanup);

    // Register entry script
    nux_lua_t *lua = nux_lua_load(nux_arena_core(), entry_script);
    NUX_CHECK(lua, goto cleanup);

    return instance;

cleanup:
    if (!nux_error_get_status())
    {
        NUX_ERROR("%s", nux_error_get_message());
    }
    nux_instance_free(instance);
    return NUX_NULL;
}
void
nux_instance_free (nux_instance_t *instance)
{
    setup_thread_context(instance);

    // Cleanup all resources
    nux_arena_reset(nux_arena_core());

    // Reset runtime
    nux_debug_free();
    nux_gui_free();
    nux_physics_free();
    nux_graphics_free();
    nux_scene_free();
    nux_lua_free();
    nux_base_free();

    // Free core memory
    nux_arena_free(nux_arena_core());
    nux_os_alloc(nux_base_module()->userdata, instance, 0, 0);
}
void
nux_instance_update (nux_instance_t *instance)
{
    setup_thread_context(instance);

    // Pre update
    nux_os_stats_update(nux_base_module()->userdata, nux_base_module()->stats);
    nux_graphics_pre_update();

    // Update
    nux_input_update();
    nux_physics_update();
    nux_lua_update();
    nux_event_process_all();
    nux_debug_update();
    if (!nux_error_get_status())
    {
        NUX_ERROR("%s", nux_error_get_message());
    }
    nux_error_reset();
    nux_graphics_update();

    // Hotreload
    if (nux_base_module()->config.hotreload)
    {
        nux_u32_t count;
        nux_rid_t handles[256];
        nux_os_hotreload_pull(nux_base_module()->userdata, handles, &count);
        for (nux_u32_t i = 0; i < count; ++i)
        {
            nux_resource_reload(handles[i]);
        }
    }

    // Post update
    nux_arena_reset(nux_arena_frame());
    nux_base_module()->time_elapsed += nux_time_delta();
    ++nux_base_module()->frame;
    nux_os_event_vec_clear(&nux_base_module()->input_events);
}
void
nux_instance_push_event (nux_instance_t *instance, nux_os_event_t *event)
{
    setup_thread_context(instance);
    if (event->type == NUX_OS_EVENT_INPUT)
    {
        nux_os_event_vec_pushv(&nux_base_module()->input_events, *event);
    }
}
