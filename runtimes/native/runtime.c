#include "internal.h"

runtime_t runtime;

static void
apply_viewport_mode (struct nk_vec2i window_size)
{
    struct nk_rect viewport = runtime.viewport_ui;

    struct nk_vec2 global_pos  = { viewport.x, viewport.y };
    struct nk_vec2 global_size = { viewport.w, viewport.h };

    struct nk_vec2i screen       = { NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT };
    float           aspect_ratio = (float)screen.x / screen.y;

    struct nk_vec2 vsize = { 0, 0 };
    switch (runtime.viewport_mode)
    {
        case VIEWPORT_FIXED: {
            vsize.x = screen.x;
            vsize.y = screen.y;
        };
        break;
        case VIEWPORT_FIXED_BEST_FIT: {
            float w_factor = global_size.x / screen.x;
            float h_factor = global_size.y / screen.y;
            float min      = w_factor < h_factor ? w_factor : h_factor;
            if (min < 1)
            {
                // 0.623 => 0.5
                // 0,432 => 0.25
                // 0.115 => 0,125
                float n = 2;
                while (min < (1 / n))
                {
                    n *= 2;
                }
                min = 1 / n;
            }
            else
            {
                min = floorf(min);
            }
            vsize.x = screen.x * min;
            vsize.y = screen.y * min;
        }
        break;
        case VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                vsize.x = floorf(global_size.y * aspect_ratio);
                vsize.y = floorf(global_size.y);
            }
            else
            {
                vsize.x = floorf(global_size.x);
                vsize.y = floorf(global_size.x / aspect_ratio);
            }
        }
        break;
        case VIEWPORT_STRETCH:
            vsize = global_size;
            break;
        default:
            break;
    }

    struct nk_vec2 vpos;
    vpos.x = global_pos.x + (global_size.x - vsize.x) * 0.5;
    vpos.y = global_pos.y + (global_size.y - vsize.y) * 0.5;

    // Patch pos (bottom left in opengl)
    vpos.y = window_size.y - (vpos.y + vsize.y);

    runtime.viewport.x = vpos.x;
    runtime.viewport.y = vpos.y;
    runtime.viewport.w = vsize.x;
    runtime.viewport.h = vsize.y;
}
static void
runtime_close (void)
{
    if (runtime.instance)
    {
        nux_instance_free(runtime.instance);
        runtime.instance = NULL;
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
    if (config->path)
    {
        runtime_open(config->path);
    }

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
        struct nk_vec2i window_size = runtime.size;

        // Clear background
        renderer_clear();

        // Update instance
        if (runtime.instance)
        {
            // Compute viewport
            apply_viewport_mode(window_size);

            // Begin renderer
            renderer_begin(nk_rect(0, 0, window_size.x, window_size.y),
                           window_size);

            // Update
            nux_instance_update(runtime.instance);

            // End renderer
            renderer_end();
        }

        // Update GUI
        gui_update();

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

    strncpy(runtime.path, path ? path : ".", PATH_MAX_LEN - 1);
    runtime.instance      = NULL;
    runtime.viewport_ui   = nk_rect(0, 0, 10, 10);
    runtime.viewport_mode = VIEWPORT_STRETCH_KEEP_ASPECT;

    runtime.instance = nux_instance_init(NULL, path);
    if (!runtime.instance)
    {
        fprintf(stderr, "failed to init instance\n");
        goto cleanup0;
    }

    return NUX_SUCCESS;
cleanup0:
    runtime_close();
    return NUX_FAILURE;
}
void
runtime_reset (void)
{
    if (runtime.instance)
    {
        char path[PATH_MAX_LEN];
        memcpy(path, runtime.path, PATH_MAX_LEN);
        runtime_close();
        runtime_open(path);
    }
}

void *
nux_os_alloc (void *userdata, void *p, nux_u32_t o, nux_u32_t n)
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
nux_os_stats_update (void *userdata, nux_u64_t *stats)
{
    stats[NUX_STAT_FPS]           = runtime.fps;
    stats[NUX_STAT_SCREEN_WIDTH]  = runtime.viewport.w;
    stats[NUX_STAT_SCREEN_HEIGHT] = runtime.viewport.h;
    stats[NUX_STAT_TIMESTAMP]     = time(NULL);
}
