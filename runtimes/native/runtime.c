#include "runtime.h"
#include "core/vm.h"

static struct
{
    instance_t instances[4];
} runtime;

static void
instance_free (instance_t *instance)
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
instance_init (instance_t *instance, const vm_config_t *config, nu_sv_t path)
{
    instance_free(instance);

    instance->active              = NU_TRUE;
    instance->config              = *config;
    instance->vm.mem              = NU_NULL;
    instance->save_state          = NU_NULL;
    instance->pause               = NU_FALSE;
    instance->viewport            = nu_b2i_xywh(0, 0, 10, 10);
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

nu_status_t
runtime_run (runtime_app_t app, nu_bool_t debug)
{
    // Initialize runtime
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);
    status = gui_init();
    NU_CHECK(status, goto cleanup2);
    status = wamr_init(debug);
    NU_CHECK(status, goto cleanup3);

    // Initialization callback
    if (app.init)
    {
        app.init();
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
            instance_t *instance = runtime.instances + i;
            if (instance->active && !instance->pause)
            {
                // Tick
                vm_tick(&instance->vm);
            }
        }

        // Update GUI
        struct nk_context *ctx = gui_new_frame();
        if (app.update)
        {
            app.update(ctx);
        }

        // Draw instances
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(runtime.instances); ++i)
        {
            instance_t *instance = runtime.instances + i;
            if (instance->active)
            {
                nu_v2u_t size = window_get_size();
                renderer_render_instance(instance->viewport,
                                         instance->viewport_mode,
                                         nu_v2u(size.x, size.y));
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
void
runtime_set_instance_viewport (nu_u32_t        index,
                               nu_b2i_t        viewport,
                               viewport_mode_t mode)
{
    NU_ASSERT(index < NU_ARRAY_SIZE(runtime.instances));
    runtime.instances[index].viewport      = viewport;
    runtime.instances[index].viewport_mode = mode;
}
inspect_value_t *
runtime_inspect_values (nu_u32_t index, nu_size_t *count)
{
    NU_ASSERT(index < NU_ARRAY_SIZE(runtime.instances));
    *count = runtime.instances[index].inspect_value_count;
    return runtime.instances[index].inspect_values;
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
