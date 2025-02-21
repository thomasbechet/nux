#include "logger.h"
#include "window.h"
#include "renderer.h"
#include "wamr.h"
#include "core/vm.h"

nu_status_t
runtime_run (const runtime_info_t *info)
{
    nu_status_t status;
    logger_init(info->log_callback);
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);
    status = wamr_init(info->debug);
    NU_CHECK(status, goto cleanup2);

    vm_t        vm;
    vm_config_t config;
    vm_config_default(&config);
    nu_byte_t *save_state = runtime_malloc(vm_config_state_memsize(&config));
    NU_ASSERT(save_state);

    status = vm_init(&vm, &config);
    NU_CHECK(status, goto cleanup3);

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(info->path, name, NU_PATH_MAX);
    status = vm_load(&vm, name);
    NU_CHECK(status, goto cleanup4);

    while (vm.running)
    {
        window_poll_events();
        vm_tick(&vm);
        window_swap_buffers();

        window_command_t cmd;
        while (window_poll_command(&cmd))
        {
            switch (cmd)
            {
                case COMMAND_EXIT:
                    vm.running = NU_FALSE;
                    break;
                case COMMAND_SAVE_STATE:
                    vm_log(&vm, NU_LOG_INFO, "SAVING STATE");
                    vm_save_state(&vm, save_state);
                    break;
                case COMMAND_LOAD_STATE:
                    vm_log(&vm, NU_LOG_INFO, "LOADING STATE");
                    vm_load_state(&vm, save_state);
                    break;
            }
        }
    }

cleanup4:
    vm_free(&vm);
cleanup3:
    wamr_free();
cleanup2:
    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}

void *
runtime_malloc (nu_size_t n)
{
    return malloc(n);
}
void
runtime_free (void *p)
{
    free(p);
}
