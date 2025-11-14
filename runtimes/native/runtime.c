#include "internal.h"

runtime_t runtime;

static void
runtime_close (void)
{
    if (runtime.opened)
    {
        nux_core_free();
        runtime.opened = false;
    }
}

nux_status_t
runtime_run (const config_t *config)
{
    // Initialize runtime
    nux_status_t status;
    status = window_init();
    CHECK(status, goto cleanup0);
    status = renderer_init();
    CHECK(status, goto cleanup1);
    status = hotreload_init();
    CHECK(status, goto cleanup2);

    // Initialize base
    runtime_open(config->path);

    // Main loop
    runtime.running = true;
    while (runtime.running)
    {
        if (runtime.reload)
        {
            runtime_close();
            if (config->path)
            {
                CHECK(runtime_open(config->path), runtime.running = false);
            }
            runtime.reload = false;
        }

        // Retrieve window events
        window_begin_frame();

        // Update instance
        if (runtime.running)
        {
            // Begin renderer
            renderer_begin();

            // Update
            nux_core_update();

            // End renderer
            renderer_end();
        }

        // Swap buffers
        runtime.fps = window_end_frame();
    }

    runtime_close();

    hotreload_free();
cleanup2:
    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}

nux_status_t
runtime_open (const char *path)
{
    runtime_close();

    strncpy(runtime.path, path ? path : ".", PATH_MAX_LEN);
    // if (!nux_core_init(NULL, runtime.path))
    if (!nux_core_init())
    {
        fprintf(stderr, "failed to init instance\n");
        goto cleanup0;
    }

    nux_lua_load(nux_arena_core(), "init.lua");
    runtime.opened = true;

    return NUX_SUCCESS;
cleanup0:
    runtime_close();
    return NUX_FAILURE;
}
void
runtime_reset (void)
{
    if (runtime.running)
    {
        char path[PATH_MAX_LEN];
        memcpy(path, runtime.path, PATH_MAX_LEN);
        runtime_close();
        runtime_open(path);
    }
}

void *
nux_os_alloc (void *p, nux_u32_t o, nux_u32_t n)
{
    if (!p)
    {
        return malloc(n);
    }
    else if (!n)
    {
        free(p);
        return NULL;
    }
    else
    {
        return realloc(p, n);
    }
}
void
nux_os_stats_update (nux_u64_t *stats)
{
    stats[NUX_STAT_FPS]           = runtime.fps;
    stats[NUX_STAT_SCREEN_WIDTH]  = runtime.size.x;
    stats[NUX_STAT_SCREEN_HEIGHT] = runtime.size.y;
    stats[NUX_STAT_TIMESTAMP]     = time(NULL);
}
