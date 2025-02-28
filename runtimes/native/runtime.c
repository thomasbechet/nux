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
    status = gui_init(window_get_win());
    NU_CHECK(status, goto cleanup2);
#endif
    status = wamr_init(debug);
    NU_CHECK(status, goto cleanup3);

#ifdef NUX_BUILD_GUI
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas *atlas;
        gui_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        gui_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &droid->handle);*/}
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

        vm_tick(&vm);

#ifdef NUX_BUILD_GUI
        struct nk_context *ctx = gui_new_frame();
        /* GUI */
        if (nk_begin(ctx,
                     "Demo",
                     nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE
                         | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum
            {
                EASY,
                HARD
            };
            static int op       = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
            {
                fprintf(stdout, "button pressed\n");
            }

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY))
            {
                op = EASY;
            }
            if (nk_option_label(ctx, "hard", op == HARD))
            {
                op = HARD;
            }

            static nu_char_t text[64];
            static nu_int_t  text_len;
            nk_edit_string(ctx,
                           NK_EDIT_FIELD | NK_EDIT_READ_ONLY,
                           text,
                           &text_len,
                           sizeof(text),
                           nk_filter_float);

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
        }
        nk_end(ctx);
        gui_render(NK_ANTI_ALIASING_ON, NU_MEM_32K, NU_MEM_32K);
#endif

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
