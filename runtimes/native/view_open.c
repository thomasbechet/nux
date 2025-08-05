#include "internal.h"

#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

typedef struct
{
    bool open;
    char dir[PATH_BUF_LEN];
    char edit_dir[PATH_BUF_LEN];
    int  edit_dir_len;
    char edit_filename[PATH_BUF_LEN];
    int  edit_filename_len;
    int  file_selected;
    int  filetype;
} file_dialog_t;

static file_dialog_t filedialog;

static void
path_concat (char *buf, const char *p1, const char *p2)
{
    if (!p1 || !strnlen(p1, PATH_MAX_LEN))
    {
        memcpy(buf, p2, PATH_BUF_LEN);
        return;
    }
    if (!p2)
    {
        memcpy(buf, p1, PATH_BUF_LEN);
        return;
    }

    const char *fmt;
    int         p1n = strnlen(p1, PATH_MAX_LEN);
    int         p2n = strnlen(p2, PATH_MAX_LEN);
    if (p1[p1n - 1] == '/')
    {
        fmt = "%.*s%.*s";
    }
    else
    {
        fmt = "%.*s/%.*s";
    }
    snprintf(buf, PATH_MAX_LEN, fmt, p1n, p1, p2n, p2);
}
static nux_status_t
path_list_files (const char *path,
                 char (*files)[PATH_BUF_LEN],
                 int  capa,
                 int *count)
{
    assert(path);
    *count = 0;
    char s[PATH_BUF_LEN];
    memcpy(s, path, PATH_MAX_LEN);
    DIR *d;
    d = opendir(s);
    if (!d)
    {
        return NUX_FAILURE;
    }
    struct dirent *dir;
    while ((dir = readdir(d)))
    {
        // Apply filter
        if (strncmp(dir->d_name, ".", PATH_MAX_LEN) != 0
            && strncmp(dir->d_name, "..", PATH_MAX_LEN)
            && !(strnlen(dir->d_name, PATH_MAX_LEN) && dir->d_name[0] == '.'))
        {
            if (*count < capa)
            {
                char realpath_buf[PATH_MAX_LEN];
                path_concat(realpath_buf, path, dir->d_name);
                int len = strnlen(realpath_buf, PATH_MAX_LEN);
                strncpy(files[*count], realpath_buf, PATH_MAX_LEN);
                files[*count][len] = 0;
            }
            ++(*count);
        }
    }
    closedir(d);
    return NUX_SUCCESS;
}
static void
path_parent (char *buf, const char *path)
{
    assert(path);
    int len = strnlen(path, PATH_MAX_LEN);
    assert(len);
    // Remove trailing '/' for folder
    if (path[len - 1] == '/')
    {
        --len;
    }
    // Remove the basename
    for (int n = len; n; --n)
    {
        if (path[n - 1] == '/')
        {
            if (n - 1 == 0) // Special case for root dir
            {
                ++n;
            }
            strncpy(buf, path, n - 1);
            buf[n - 1] = 0;
            return;
        }
    }
    strncpy(buf, "", ARRAY_LEN(""));
}
static void
path_basename (char *buf, const char *path)
{
    int len = strnlen(path, PATH_MAX_LEN);
    for (int n = len; n; --n)
    {
        if (path[n - 1] == '/')
        {
            strncpy(buf, path + n, len - n);
            buf[len - n] = 0;
            return;
        }
    }
    memcpy(buf, path, PATH_MAX_LEN);
}
static bool
path_isdir (const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
    {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

static void
file_dialog_change_dir (file_dialog_t *fd, const char *dir)
{
    memcpy(fd->dir, dir, PATH_MAX_LEN);
    memcpy(fd->edit_dir, dir, PATH_MAX_LEN);
    fd->edit_dir_len      = strnlen(dir, PATH_MAX_LEN);
    fd->edit_filename_len = 0;
    fd->file_selected     = -1;
}

static void
file_dialog_open (file_dialog_t *fd)
{
    char        cwd_buf[PATH_BUF_LEN];
    const char *cwd = getcwd(cwd_buf, PATH_MAX_LEN);
    char        dir[PATH_BUF_LEN];
    memcpy(dir, cwd, PATH_MAX_LEN - 1);
    file_dialog_change_dir(fd, dir);
    fd->open = true;
}

static bool
file_dialog (file_dialog_t     *fd,
             struct nk_context *ctx,
             struct nk_rect     bounds,
             char               output[PATH_BUF_LEN])
{
    if (!fd->open)
    {
        return false;
    }

    bool exit = false;
    if (nk_begin(ctx,
                 "File Dialog",
                 bounds,
                 NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        const int row = 30;
        char      files[64][PATH_BUF_LEN];
        int       count;

        nux_status_t status
            = path_list_files(fd->dir, files, ARRAY_LEN(files), &count);
        assert(status);
        if (count > (int)ARRAY_LEN(files))
        {
            count = ARRAY_LEN(files);
        }

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 30);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        if (nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP))
        {
            char parent[PATH_BUF_LEN];
            path_parent(parent, fd->dir);
            int parent_len = strnlen(parent, PATH_MAX_LEN);
            if (parent_len)
            {
                char temp[PATH_BUF_LEN];
                strncpy(temp, parent, PATH_MAX_LEN);
                file_dialog_change_dir(fd, temp);
            }
        }
        nk_edit_string(ctx,
                       NK_EDIT_FIELD,
                       fd->edit_dir,
                       &fd->edit_dir_len,
                       PATH_MAX_LEN,
                       nk_filter_default);

        nk_layout_row_dynamic(ctx, bounds.h - row * 5, 1);
        if (nk_group_begin(ctx, "Files", NK_WINDOW_BORDER))
        {
            nk_layout_row_dynamic(ctx, row, 1);
            for (int i = 0; i < count; ++i)
            {
                nk_bool     selected = fd->file_selected == i;
                const char *file     = files[i];
                char        basename[PATH_MAX_LEN];
                path_basename(basename, file);

                // Must be checked before selectable label update
                if (selected && runtime.nk_glfw.is_double_click_down)
                {
                    if (path_isdir(file))
                    {
                        file_dialog_change_dir(fd, file);
                    }
                    else
                    {
                        memcpy(output, files[i], PATH_MAX_LEN);
                        fd->open = false;
                        exit     = true;
                    }
                }

                if (nk_selectable_label(ctx, basename, NK_TEXT_LEFT, &selected))
                {
                    if (selected)
                    {
                        fd->file_selected = i;
                        strncpy(fd->edit_filename, basename, PATH_MAX_LEN);
                        fd->edit_filename_len = strnlen(basename, PATH_MAX_LEN);
                    }
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
                       PATH_MAX_LEN,
                       nk_filter_default);

        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_static(ctx, 150);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_end(ctx);

        nk_spacer(ctx); // TODO: file type
        nk_spacer(ctx); // TODO: file type
        // nk_label(ctx, "File type:", NK_TEXT_ALIGN_LEFT |
        // NK_TEXT_ALIGN_MIDDLE); const nu_char_t *items[] = { "", ".jpeg" };
        // nk_combobox(ctx,
        //             items,
        //             NU_ARRAY_SIZE(items),
        //             &fd->filetype,
        //             row,
        //             nk_vec2(100, 100));
        nk_spacer(ctx);
        if (nk_button_label(ctx, "Ok") && fd->file_selected != -1)
        {
            strncpy(output, files[fd->file_selected], PATH_MAX_LEN);
            fd->open = false;
            exit     = true;
        }
        if (nk_button_label(ctx, "Cancel"))
        {
            fd->open = false;
            exit     = false;
        }
    }
    nk_end(ctx);
    return exit;
}

void
view_open (struct nk_context *ctx, struct nk_rect bounds)
{
    if (!filedialog.open)
    {
        file_dialog_open(&filedialog);
    }

    if (filedialog.open)
    {
        char file[PATH_BUF_LEN];
        if (file_dialog(&filedialog, ctx, bounds, file))
        {
            runtime_open(file);
        }
    }
}
