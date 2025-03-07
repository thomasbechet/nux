#include "runtime.h"
#include "core/vm.h"

static struct
{
    runtime_instance_t instances[4];
} runtime;

static void
instance_free (runtime_instance_t *instance)
{
    if (!instance->active)
    {
        return;
    }
    if (instance->save_state)
    {
        native_free(instance->save_state);
    }
    if (instance->vm.mem)
    {
        vm_free(&instance->vm);
    }
    instance->active = NU_FALSE;
}
static nu_status_t
instance_init (runtime_instance_t *instance,
               const vm_config_t  *config,
               nu_sv_t             path)
{
    instance_free(instance);

    instance->active              = NU_TRUE;
    instance->config              = *config;
    instance->vm.mem              = NU_NULL;
    instance->save_state          = NU_NULL;
    instance->pause               = NU_FALSE;
    instance->viewport            = nk_rect(0, 0, 10, 10);
    instance->viewport_mode       = VIEWPORT_STRETCH_KEEP_ASPECT;
    instance->inspect_value_count = 0;

    nu_byte_t *save_state = native_malloc(vm_config_state_memsize(config));
    NU_ASSERT(save_state);

    nu_status_t status = vm_init(&instance->vm, config);
    NU_CHECK(status, goto cleanup0);
    instance->vm.userdata = instance;

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    status = vm_load(&instance->vm, name);
    NU_CHECK(status, goto cleanup0);

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

    nu_v2_t  screen       = nu_v2(SYS_SCREEN_WIDTH, SYS_SCREEN_HEIGHT);
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
runtime_start (const runtime_config_t *config)
{
    // Initialize runtime
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);
    status = gui_init(config);
    NU_CHECK(status, goto cleanup2);
    status = wamr_init(config->debug);
    NU_CHECK(status, goto cleanup3);

    // Initialize base
    runtime_init_instance(0, config->path);

    // Initialize views
    for (nu_size_t i = 0; i < config->views_count; ++i)
    {
        if (config->views[i].init)
        {
            config->views[i].init();
        }
    }

    // Main loop
    nu_bool_t running = NU_TRUE;
    while (running)
    {
        // Retrieve window events
        window_poll_events();

        // Update active instances
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(runtime.instances); ++i)
        {
            runtime_instance_t *instance = runtime.instances + i;
            if (instance->active && !instance->pause)
            {
                // Tick
                vm_tick(&instance->vm);
            }
        }

        // Update GUI
        gui_update();

        // Clear window
        nu_v2u_t size = window_get_size();
        renderer_clear(nu_b2i_xywh(0, 0, size.x, size.y), size);

        // Draw instances
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(runtime.instances); ++i)
        {
            runtime_instance_t *instance = runtime.instances + i;
            if (instance->active)
            {
                if (instance->viewport_mode != VIEWPORT_HIDDEN)
                {
                    nu_b2i_t viewport = nu_b2i_xywh(instance->viewport.x,
                                                    instance->viewport.y,
                                                    instance->viewport.w,
                                                    instance->viewport.h);
                    viewport          = apply_viewport_mode(viewport,
                                                   instance->viewport_mode);
                    renderer_render_instance(viewport, size);
                }
            }
        }

        // Draw GUI
        gui_render();

        // Swap buffers
        window_swap_buffers();

        // Process runtime events
        runtime_command_t cmd;
        while (window_poll_command(&cmd))
        {
            switch (cmd)
            {
                case COMMAND_EXIT:
                    running = NU_FALSE;
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

    wamr_free();
cleanup3:
    gui_free();
cleanup2:
    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}
nu_status_t
runtime_init_instance (nu_u32_t index, nu_sv_t path)
{
    NU_ASSERT(index < NU_ARRAY_SIZE(runtime.instances));
    vm_config_t config;
    vm_config_default(&config);
    return instance_init(&runtime.instances[index], &config, path);
}
runtime_instance_t *
runtime_instance (void)
{
    return &runtime.instances[0];
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
