#include "internal.h"

static struct
{
    instance_t instance;
    bool       running;
    int        fps;
} runtime;

static void
instance_free (instance_t *instance)
{
    if (!instance->active)
    {
        return;
    }
    if (instance->ctx)
    {
        nux_instance_free(instance->ctx);
    }
    instance->active = false;
}
static nux_status_t
instance_init (instance_t *instance, const char *path)
{
    instance_free(instance);

    nux_config_t config = { .userdata     = instance,
                            .init         = NULL,
                            .update       = NULL,
                            .memory_size  = (1 << 26), // 16Mb
                            .max_id_count = 4096,
                            .boot_device  = path,
                            .init_script  = "init.lua" };

    strncpy(instance->path, path ? path : ".", PATH_MAX_LEN - 1);
    instance->active        = true;
    instance->config        = config;
    instance->ctx           = NULL;
    instance->viewport_ui   = nk_rect(0, 0, 10, 10);
    instance->viewport_mode = VIEWPORT_STRETCH_KEEP_ASPECT;

    instance->ctx = nux_instance_init(&config);
    if (!instance->ctx)
    {
        logger_log(NUX_LOG_ERROR, "failed to init instance");
        goto cleanup0;
    }

    return NUX_SUCCESS;
cleanup0:
    instance_free(instance);
    return NUX_FAILURE;
}
static void
apply_viewport_mode (instance_t *instance, struct nk_vec2i window_size)
{
    struct nk_rect viewport = instance->viewport_ui;

    struct nk_vec2 global_pos  = { viewport.x, viewport.y };
    struct nk_vec2 global_size = { viewport.w, viewport.h };

    struct nk_vec2i screen       = { NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT };
    float           aspect_ratio = (float)screen.x / screen.y;

    struct nk_vec2 vsize = { 0, 0 };
    switch (instance->viewport_mode)
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

    instance->viewport.x = vpos.x;
    instance->viewport.y = vpos.y;
    instance->viewport.w = vsize.x;
    instance->viewport.h = vsize.y;
}

nux_status_t
runtime_run (const config_t *config)
{
    // Initialize runtime
    nux_status_t status;
    status = window_init();
    if (!status)
    {
        goto cleanup0;
    }
    status = renderer_init();
    if (!status)
    {
        goto cleanup1;
    }

    runtime.running = true;

    // Initialize base
    runtime_open(config->path);

    // Main loop
    while (runtime.running)
    {
        // Retrieve window events
        window_begin_frame();
        struct nk_vec2i window_size = window_get_size();

        // Clear background
        renderer_clear();

        // Update instance
        if (runtime.instance.active)
        {
            // Compute viewport
            apply_viewport_mode(&runtime.instance, window_size);

            // Begin renderer
            renderer_begin(nk_rect(0, 0, window_size.x, window_size.y),
                           window_size);

            // Tick
            nux_instance_tick(runtime.instance.ctx);

            // End renderer
            renderer_end();
        }

        // Update GUI
        gui_update();

        // Swap buffers
        runtime.fps = window_end_frame();

        // Process runtime events
        command_t cmd;
        while (command_poll(&cmd))
        {
            switch (cmd.type)
            {
                case COMMAND_EXIT:
                    runtime.running = false;
                    break;
                case COMMAND_SAVE_STATE:
                    break;
                case COMMAND_LOAD_STATE:
                    break;
                case COMMAND_CHANGE_VIEW:
                    gui_set_view(cmd.view);
                    break;
            }
        }
    }

    // Free instance
    instance_free(&runtime.instance);

    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}
nux_status_t
runtime_open (const char *path)
{
    return instance_init(&runtime.instance, path);
}
void
runtime_close (void)
{
    instance_free(&runtime.instance);
}
void
runtime_reset (void)
{
    if (runtime.instance.active)
    {
        char path[PATH_MAX_LEN];
        memcpy(path, runtime.instance.path, PATH_MAX_LEN);
        runtime_close();
        runtime_open(path);
    }
}
instance_t *
runtime_instance (void)
{
    return &runtime.instance;
}
void
runtime_quit (void)
{
    runtime.running = false;
}

void *
native_malloc (size_t n)
{
    return malloc(n);
}
void
native_free (void *p)
{
    free(p);
}
void *
native_realloc (void *p, size_t n)
{
    return realloc(p, n);
}

void *
nux_os_alloc (void *userdata, void *p, nux_u32_t o, nux_u32_t n)
{
    if (!p)
    {
        return native_malloc(n);
    }
    else if (!n)
    {
        native_free(p);
        return NULL;
    }
    else
    {
        return native_realloc(p, n);
    }
}
void
nux_os_log (void           *userdata,
            nux_log_level_t level,
            const nux_c8_t *log,
            nux_u32_t       n)
{
    logger_log(level, "%.*s", n, log);
}
void
nux_os_stats_update (void *userdata, nux_u32_t *stats)
{
    instance_t *inst              = userdata;
    stats[NUX_STAT_FPS]           = runtime.fps;
    stats[NUX_STAT_SCREEN_WIDTH]  = inst->viewport.w;
    stats[NUX_STAT_SCREEN_HEIGHT] = inst->viewport.h;
}
