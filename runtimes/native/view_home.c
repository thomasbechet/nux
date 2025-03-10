#include "nuklear/src/nuklear.h"
#include "runtime.h"

typedef struct
{
    nu_char_t dir[NU_PATH_MAX];
    nu_char_t edit_dir[NU_PATH_MAX];
    nu_int_t  edit_dir_len;
    nu_char_t edit_filename[NU_PATH_MAX];
    nu_int_t  edit_filename_len;
    nu_int_t  file_selected;
    nu_int_t  filetype;
} file_dialog_t;

static void
file_dialog_change_dir (file_dialog_t *fd, nu_sv_t dir)
{
    nu_sv_to_cstr(dir, fd->dir, NU_PATH_MAX);
    nu_sv_to_cstr(dir, fd->edit_dir, NU_PATH_MAX);
    fd->edit_dir_len      = dir.len;
    fd->edit_filename_len = 0;
    fd->file_selected     = -1;
}

static void
file_dialog_init (file_dialog_t *fd)
{
    nu_char_t dir[NU_PATH_MAX];
    nu_path_getcwd(dir, NU_PATH_MAX);
    file_dialog_change_dir(fd, nu_sv(dir, NU_PATH_MAX));
}

static nu_bool_t
file_dialog (file_dialog_t *fd, struct nk_context *ctx, struct nk_rect bounds)
{
    nu_bool_t exit = NU_FALSE;
    if (nk_begin(ctx,
                 "File Dialog",
                 bounds,
                 NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        const nu_size_t row = 30;
        nu_char_t       files[64][NU_PATH_MAX];
        nu_size_t       count;
        NU_ASSERT(nu_path_list_files(
            nu_sv(fd->dir, NU_PATH_MAX), files, NU_ARRAY_SIZE(files), &count));
        count = NU_MIN(NU_ARRAY_SIZE(files), count);

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 30);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        if (nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP))
        {
            nu_sv_t parent = nu_path_parent(nu_sv(fd->dir, NU_PATH_MAX));
            if (parent.len)
            {
                nu_char_t temp[NU_PATH_MAX];
                nu_sv_to_cstr(parent, temp, NU_PATH_MAX);
                file_dialog_change_dir(fd, nu_sv(temp, NU_PATH_MAX));
            }
        }
        nk_edit_string(ctx,
                       NK_EDIT_FIELD,
                       fd->edit_dir,
                       &fd->edit_dir_len,
                       NU_PATH_MAX,
                       nk_filter_default);

        nk_layout_row_dynamic(ctx, bounds.h - row * 5, 1);
        if (nk_group_begin(ctx, "Files", NK_WINDOW_BORDER))
        {
            nk_layout_row_dynamic(ctx, row, 1);
            for (nu_size_t i = 0; i < count; ++i)
            {
                nk_bool   selected = fd->file_selected == (nu_int_t)i;
                nu_sv_t   file     = nu_sv(files[i], NU_PATH_MAX);
                nu_char_t basename[NU_PATH_MAX];
                nu_sv_to_cstr(nu_path_basename(file), basename, NU_PATH_MAX);
                if (nk_selectable_label(ctx, basename, NK_TEXT_LEFT, &selected))
                {
                    if (selected)
                    {
                        fd->file_selected = i;
                        nu_strncpy(fd->edit_filename, basename, NU_PATH_MAX);
                        fd->edit_filename_len
                            = nu_strnlen(basename, NU_PATH_MAX);
                    }
                }
                if (selected && gui_is_double_click() && nu_path_isdir(file))
                {
                    file_dialog_change_dir(fd, file);
                }
            }
            nk_group_end(ctx);
        }

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        nk_label(ctx, "File name:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_edit_string(ctx,
                       NK_EDIT_FIELD,
                       fd->edit_filename,
                       &fd->edit_filename_len,
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
                    &fd->filetype,
                    row,
                    nk_vec2(100, 100));
        nk_spacer(ctx);
        if (nk_button_label(ctx, "Ok"))
        {
            exit = NU_TRUE;
        }
        if (nk_button_label(ctx, "Cancel"))
        {
            exit = NU_TRUE;
        }
    }
    nk_end(ctx);
    return exit;
}

static file_dialog_t filedialog;
static nu_bool_t     filedialog_open;

void
view_home (struct nk_context *ctx, struct nk_rect bounds)
{
    // Inspector
    runtime_instance_t *instance  = runtime_instance();
    const nu_size_t     row       = 30;
    struct nk_rect left_bounds    = nk_rect(bounds.x, bounds.y, 200, bounds.h);
    struct nk_rect central_bounds = nk_rect(
        left_bounds.w, bounds.y, NU_MAX(0, bounds.w - left_bounds.w), bounds.h);
    if (nk_begin(ctx, "Games", left_bounds, 0))
    {
        nk_layout_row_dynamic(ctx, row, 1);

        nk_label(ctx, "Cartridge:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);

        if (nk_button_label(ctx, "Open File"))
        {
            if (!filedialog_open)
            {
                file_dialog_init(&filedialog);
                filedialog_open = NU_TRUE;
            }
        }

        if (instance)
        {
            nk_label(
                ctx, "Actions:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);

            if (nk_button_label(ctx, "Save"))
            {
            }
            if (nk_button_label(ctx, "Load"))
            {
            }
            if (nk_button_label(ctx, instance->pause ? "Resume" : "Pause"))
            {
                instance->pause = !instance->pause;
            }
            if (nk_button_label(ctx, "Close"))
            {
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
    }
    nk_end(ctx);

    if (filedialog_open)
    {
        if (file_dialog(&filedialog, ctx, central_bounds))
        {
            filedialog_open = NU_FALSE;
        }
    }

    instance->viewport = central_bounds;
}
