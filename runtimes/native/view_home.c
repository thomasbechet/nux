#include "nuklear/src/nuklear.h"
#include "runtime.h"

static void
file_dialog (struct nk_context *ctx, struct nk_rect bounds)
{
    if (nk_begin(ctx,
                 "File Dialog",
                 bounds,
                 NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        const nu_size_t  row = 30;
        nu_char_t        files[32][NU_PATH_MAX];
        nu_size_t        count         = nu_list_files(NU_SV("."), files, 32);
        static int       file_selected = 0;
        static nu_char_t filename[NU_PATH_MAX];
        static nu_int_t  filename_len      = 0;
        static nu_int_t  filetype_selected = 0;
        static nu_int_t  old_selected      = -1;

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 30);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        if (nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP))
        {
        }

        nk_layout_row_dynamic(ctx, bounds.h - row * 5, 1);
        if (nk_group_begin(ctx, "Files", NK_WINDOW_BORDER))
        {
            nk_layout_row_dynamic(ctx, row, 1);
            for (nu_size_t i = 0; i < count; ++i)
            {
                if (nk_select_label(ctx,
                                    files[i],
                                    NK_TEXT_LEFT,
                                    file_selected == (nu_int_t)i))
                {
                    file_selected = i;
                    if (gui_is_double_click())
                    {
                        printf("Double click !\n");
                    }
                }
            }
            // if (file_selected != (nu_int_t)old_selected)
            // {
            //     file_selected = old_selected;
            //     nu_sv_to_cstr(
            //         nu_sv_cstr(files[file_selected]), filename, NU_PATH_MAX);
            //     filename_len = nu_strlen(files[i]);
            // }
            // else if (gui_is_double_click())
            // {
            //     printf("Double click !\n");
            // }
            nk_group_end(ctx);
        }

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        nk_label(ctx, "File name:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_edit_string(ctx,
                       NK_EDIT_FIELD,
                       filename,
                       &filename_len,
                       NU_PATH_MAX,
                       nk_filter_default);

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_static(ctx, 150);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_end(ctx);

        nk_label(ctx, "File type:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        const nu_char_t *items[] = { "", ".jpeg" };
        nk_combobox(ctx,
                    items,
                    NU_ARRAY_SIZE(items),
                    &filetype_selected,
                    row,
                    nk_vec2(100, 100));
        nk_spacer(ctx);
        nk_button_label(ctx, "Ok");
        nk_button_label(ctx, "Cancel");
    }
    nk_end(ctx);
}

void
view_home (struct nk_context *ctx, struct nk_rect bounds)
{
    // Inspector
    runtime_instance_t *instance  = runtime_instance();
    const nu_size_t     row       = 30;
    struct nk_rect left_bounds    = nk_rect(bounds.x, bounds.y, 300, bounds.h);
    struct nk_rect central_bounds = nk_rect(
        left_bounds.w, bounds.y, NU_MAX(0, bounds.w - left_bounds.w), bounds.h);
    if (nk_begin(ctx, "Games", left_bounds, 0))
    {
        nk_layout_row_dynamic(ctx, row, 1);

        nk_label(ctx, "Cartridge:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);

        if (nk_button_label(ctx, "Open"))
        {
        }
        if (nk_button_label(ctx, "Open File"))
        {
        }
        if (nk_button_label(ctx, "Close"))
        {
        }

        nk_label(ctx, "Actions:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);

        if (nk_button_label(ctx, "Save"))
        {
        }
        if (nk_button_label(ctx, "Load"))
        {
        }
        if (nk_button_label(ctx, instance->pause ? "Resume" : "Stop"))
        {
            instance->pause = !instance->pause;
        }

        nk_layout_row_dynamic(ctx, row, 1);

        // Viewport mode
        {
            nk_label(ctx, "Viewport mode:", NK_TEXT_LEFT);
            const nu_char_t *modes[] = { "HIDDEN",
                                         "FIXED",
                                         "FIXED BEST FIT",
                                         "STRETCH KEEP ASPECT",
                                         "STRETCH" };
            instance->viewport_mode  = nk_combo(ctx,
                                               modes,
                                               NK_LEN(modes),
                                               instance->viewport_mode,
                                               row,
                                               nk_vec2(200, 200));
        }
    }
    nk_end(ctx);

    file_dialog(ctx, central_bounds);

    instance->viewport = central_bounds;
}
