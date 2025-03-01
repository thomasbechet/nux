#include "runtime.h"
#include "core/vm.h"

nu_status_t
runtime_run (nu_sv_t path, nu_bool_t debug)
{
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);
#ifdef NUX_BUILD_GUI
    status = gui_init();
    NU_CHECK(status, goto cleanup2);
#endif
    status = wamr_init(debug);
    NU_CHECK(status, goto cleanup3);

#ifdef NUX_BUILD_GUI
    struct nk_font_atlas *atlas;
    gui_font_stash_begin(&atlas);
    gui_font_stash_end();
#endif

    vm_t        vm;
    vm_config_t config;
    vm_config_default(&config);
    nu_byte_t *save_state = native_malloc(vm_config_state_memsize(&config));
    NU_ASSERT(save_state);

    status = vm_init(&vm, &config);
    NU_CHECK(status, goto cleanup4);

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    status = vm_load(&vm, name);
    NU_CHECK(status, goto cleanup5);

    while (vm.running)
    {
        window_poll_events();

#ifdef NUX_BUILD_GUI
        struct nk_context *ctx = gui_new_frame();
        /* GUI */
        nu_v2u_t size = window_get_size();
        if (nk_begin(
                ctx, "main", nk_rect(0, 0, size.x, size.y), NK_WINDOW_BORDER))
        {
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
            {
                fprintf(stdout, "button pressed\n");
            }
        }
        nk_end(ctx);
        gui_render(NK_ANTI_ALIASING_ON, NU_MEM_1M, NU_MEM_1M);
#endif
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

cleanup5:
    vm_free(&vm);
cleanup4:
    wamr_free();
cleanup3:
#ifdef NUX_BUILD_GUI
    gui_free();
cleanup2:
#endif
    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
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
