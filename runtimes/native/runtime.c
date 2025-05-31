#include "internal.h"
#include "nux.h"

static struct
{
    runtime_instance_t instances[4];
    nu_bool_t          running;
    nu_u32_t           fps;
} runtime;

static void
instance_free (runtime_instance_t *instance)
{
    if (!instance->active)
    {
        return;
    }
    if (instance->instance)
    {
        nux_instance_free(instance->instance);
    }
    instance->active = NU_FALSE;
}
static nu_status_t
instance_init (runtime_instance_t *instance, nu_sv_t path)
{
    instance_free(instance);

    nux_instance_config_t config = {
        .userdata = instance,
        .init     = loop_init,
        .update   = loop_update,
    };

    nu_sv_to_cstr(path, instance->path, NU_PATH_MAX);
    instance->active            = NU_TRUE;
    instance->config            = config;
    instance->instance          = NU_NULL;
    instance->pause             = NU_FALSE;
    instance->viewport          = nk_rect(0, 0, 10, 10);
    instance->viewport_mode     = VIEWPORT_STRETCH_KEEP_ASPECT;
    instance->debug_value_count = 0;

    // instance->save_state = native_malloc(vm_config_state_memsize(config));
    // NU_ASSERT(instance->save_state);

    instance->instance = nux_instance_init(&config);
    NU_CHECK(instance->instance, goto cleanup0);

    nux_status_t status
        = nux_instance_load(instance->instance,
                            instance->path,
                            nu_strnlen(instance->path, NU_PATH_MAX));
    if (!status)
    {
        logger_log(NU_LOG_ERROR, "Failed to load cartridge.");
        goto cleanup0;
    }

    return NU_SUCCESS;
cleanup0:
    instance_free(instance);
    return NU_FAILURE;
}
static nu_b2i_t
apply_viewport_mode (nu_b2i_t viewport, viewport_mode_t mode)
{
    nu_v2_t global_pos  = nu_v2(viewport.min.x, viewport.min.y);
    nu_v2_t global_size = nu_v2_v2u(nu_b2i_size(viewport));

    nu_v2_t  screen       = nu_v2(NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    nu_f32_t aspect_ratio = screen.x / screen.y;

    nu_v2_t vsize = NU_V2_ZEROS;
    switch (mode)
    {
        case VIEWPORT_FIXED: {
            vsize = nu_v2(screen.x, screen.y);
        };
        break;
        case VIEWPORT_FIXED_BEST_FIT: {
            nu_f32_t w_factor = global_size.x / screen.x;
            nu_f32_t h_factor = global_size.y / screen.y;
            nu_f32_t min      = NU_MIN(w_factor, h_factor);
            if (min < 1)
            {
                // 0.623 => 0.5
                // 0,432 => 0.25
                // 0.115 => 0,125
                nu_f32_t n = 2;
                while (min < (1 / n))
                {
                    n *= 2;
                }
                min = 1 / n;
            }
            else
            {
                min = nu_floor(min);
            }
            vsize.x = screen.x * min;
            vsize.y = screen.y * min;
        }
        break;
        case VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                vsize.x = nu_floor(global_size.y * aspect_ratio);
                vsize.y = nu_floor(global_size.y);
            }
            else
            {
                vsize.x = nu_floor(global_size.x);
                vsize.y = nu_floor(global_size.x / aspect_ratio);
            }
        }
        break;
        case VIEWPORT_STRETCH:
            vsize = global_size;
            break;
        default:
            break;
    }

    nu_v2_t vpos = nu_v2_sub(global_size, vsize);
    vpos         = nu_v2_divs(vpos, 2);
    vpos         = nu_v2_add(vpos, global_pos);
    return nu_b2i_xywh(vpos.x, vpos.y, vsize.x, vsize.y);
}

nu_status_t
runtime_run (const runtime_config_t *config)
{
    // Initialize runtime
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);

    runtime.running = NU_TRUE;

    // Initialize base
    if (config->path.len)
    {
        runtime_open(0, config->path);
    }

    // Main loop
    while (runtime.running)
    {
        // Retrieve window events
        window_begin_frame();

        // Clear window
        nu_v2u_t size = window_get_size();
        renderer_clear(nu_b2i_xywh(0, 0, size.x, size.y), size);

        // Update active instances
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(runtime.instances); ++i)
        {
            runtime_instance_t *instance = runtime.instances + i;
            if (instance->active && !instance->pause)
            {
                nu_b2i_t viewport = nu_b2i_xywh(instance->viewport.x,
                                                instance->viewport.y,
                                                instance->viewport.w,
                                                instance->viewport.h);
                viewport
                    = apply_viewport_mode(viewport, instance->viewport_mode);
                renderer_render_begin(instance->instance, viewport, size);

                // Tick
                nux_instance_tick(instance->instance);

                renderer_render_end(instance->instance, viewport, size);
            }
        }

        // Update GUI
        gui_update();

        // Swap buffers
        runtime.fps = window_end_frame();

        // Process runtime events
        runtime_command_t cmd;
        while (window_poll_command(&cmd))
        {
            switch (cmd)
            {
                case COMMAND_EXIT:
                    runtime.running = NU_FALSE;
                    break;
                case COMMAND_SAVE_STATE:
                    break;
                case COMMAND_LOAD_STATE:
                    break;
            }
        }
    }

    // Free instances
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(runtime.instances); ++i)
    {
        instance_free(runtime.instances + i);
    }

    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}
nu_status_t
runtime_open (nu_u32_t index, nu_sv_t path)
{
    NU_ASSERT(index < NU_ARRAY_SIZE(runtime.instances));
    return instance_init(&runtime.instances[index], path);
}
void
runtime_close (nu_u32_t index)
{
    instance_free(runtime.instances + index);
}
void
runtime_reset (nu_u32_t index)
{
    runtime_instance_t *instance = runtime_instance();
    if (instance->active)
    {
        nu_char_t path[NU_PATH_MAX];
        nu_strncpy(path, instance->path, NU_PATH_MAX);
        runtime_close(0);
        runtime_open(0, nu_sv(path, NU_PATH_MAX));
    }
}
runtime_instance_t *
runtime_instance (void)
{
    return &runtime.instances[0];
}
void
runtime_quit (void)
{
    runtime.running = NU_FALSE;
}

void *
native_malloc (nu_size_t n)
{
    return malloc(n);
}
void
native_free (void *p)
{
    free(p);
}
void *
native_realloc (void *p, nu_size_t n)
{
    return realloc(p, n);
}

void *
nux_os_malloc (void *userdata, nux_memory_usage_t usage, nux_u32_t n)
{
    return native_malloc(n);
}
void
nux_os_free (void *userdata, void *p)
{
    native_free(p);
}
void *
nux_os_realloc (void *userdata, void *p, nux_u32_t n)
{
    return native_realloc(p, n);
}
void
nux_os_log (void *userdata, const nux_c8_t *log, nux_u32_t n)
{
    logger_log(NU_LOG_INFO, "%.*s", n, log);
}
void
nux_os_debug (void            *userdata,
              const nux_c8_t  *name,
              nux_u32_t        n,
              nux_debug_type_t type,
              void            *p)
{
    // Find existing value at address
    intptr_t            addr     = (intptr_t)p;
    runtime_instance_t *instance = userdata;
    debug_value_t      *value    = NU_NULL;
    for (nu_size_t i = 0; i < instance->debug_value_count; ++i)
    {
        // if (instance->debug_values[i].addr == addr)
        // {
        //     value = instance->debug_values + i;
        // }
        if (nu_strneq(instance->debug_values[i].name, name, NUX_NAME_MAX))
        {
            value = instance->debug_values + i;
        }
    }
    // Register new value
    if (!value)
    {
        if (instance->debug_value_count
            >= NU_ARRAY_SIZE(instance->debug_values))
        {
            logger_log(NU_LOG_ERROR, "Max inspect value count reach");
            return;
        }
        value       = &instance->debug_values[instance->debug_value_count++];
        value->type = type;
        value->addr = addr;
        value->override = NU_FALSE;
        nu_strncpy(value->name, name, sizeof(value->name));
    }
    // Read / Write value
    if (value->override)
    {
        switch (type)
        {
            case NUX_DEBUG_I32:
                *((nu_i32_t *)p) = value->value.i32;
                break;
            case NUX_DEBUG_F32:
                *((nu_f32_t *)p) = value->value.f32;
                break;
        }
        value->override = NU_FALSE;
    }
    else
    {
        switch (type)
        {
            case NUX_DEBUG_I32:
                value->value.i32 = *(const nu_i32_t *)p;
                break;
            case NUX_DEBUG_F32:
                value->value.f32 = *(const nu_f32_t *)p;
                break;
        }
    }
}
void
nux_os_update_stats (void *userdata, nux_u32_t *stats)
{
    runtime_instance_t *inst = userdata;
    stats[NUX_STAT_FPS]      = runtime.fps;
}
