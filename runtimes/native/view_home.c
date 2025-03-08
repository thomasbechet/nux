#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>

#define UNUSED(a) (void)a
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define LEN(a)    (sizeof(a) / sizeof(a)[0])

enum file_groups
{
    FILE_GROUP_DEFAULT,
    FILE_GROUP_TEXT,
    FILE_GROUP_MUSIC,
    FILE_GROUP_FONT,
    FILE_GROUP_IMAGE,
    FILE_GROUP_MOVIE,
    FILE_GROUP_MAX
};

struct file_group
{
    enum file_groups group;
    const char      *name;
    struct nk_image *icon;
};

#define MAX_PATH_LEN 512
struct file_browser
{
    /* path */
    char file[MAX_PATH_LEN];
    char home[MAX_PATH_LEN];
    char desktop[MAX_PATH_LEN];
    char directory[MAX_PATH_LEN];

    /* directory content */
    char **files;
    char **directories;
    size_t file_count;
    size_t dir_count;
};

#ifdef __unix__
#include <dirent.h>
#include <unistd.h>
#endif

#ifndef _WIN32
#include <pwd.h>
#endif

static void
die (const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

#if 0
static char*
file_load(const char* path, size_t* siz)
{
    char *buf;
    FILE *fd = fopen(path, "rb");
    if (!fd) die("Failed to open file: %s\n", path);
    fseek(fd, 0, SEEK_END);
    *siz = (size_t)ftell(fd);
    fseek(fd, 0, SEEK_SET);
    buf = (char*)calloc(*siz, 1);
    fread(buf, *siz, 1, fd);
    fclose(fd);
    return buf;
}
#endif

static char *
str_duplicate (const char *src)
{
    char  *ret;
    size_t len = strlen(src);
    if (!len)
    {
        return 0;
    }
    ret = (char *)malloc(len + 1);
    if (!ret)
    {
        return 0;
    }
    memcpy(ret, src, len);
    ret[len] = '\0';
    return ret;
}

static void
dir_free_list (char **list, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i)
    {
        free(list[i]);
    }
    free(list);
}

static char **
dir_list (const char *dir, int return_subdirs, size_t *count)
{
    size_t     n = 0;
    char       buffer[MAX_PATH_LEN];
    char     **results  = NULL;
    const DIR *none     = NULL;
    size_t     capacity = 32;
    size_t     size;
    DIR       *z;

    assert(dir);
    assert(count);
    strncpy(buffer, dir, MAX_PATH_LEN);
    buffer[MAX_PATH_LEN - 1] = 0;
    n                        = strlen(buffer);

    if (n > 0 && (buffer[n - 1] != '/'))
    {
        buffer[n++] = '/';
    }

    size = 0;

    z = opendir(dir);
    if (z != none)
    {
        int            nonempty = 1;
        struct dirent *data     = readdir(z);
        nonempty                = (data != NULL);
        if (!nonempty)
        {
            return NULL;
        }

        do
        {
            DIR  *y;
            char *p;
            int   is_subdir;
            if (data->d_name[0] == '.')
            {
                continue;
            }

            strncpy(buffer + n, data->d_name, MAX_PATH_LEN - n);
            y         = opendir(buffer);
            is_subdir = (y != NULL);
            if (y != NULL)
            {
                closedir(y);
            }

            if ((return_subdirs && is_subdir)
                || (!is_subdir && !return_subdirs))
            {
                if (!size)
                {
                    results = (char **)calloc(capacity, sizeof(char *));
                }
                else if (size >= capacity)
                {
                    // void *old = results;
                    capacity = capacity * 2;
                    results
                        = (char **)realloc(results, capacity * sizeof(char *));
                    assert(results);
                    if (!results)
                    {
                        // free(old);
                    }
                }
                p               = str_duplicate(data->d_name);
                results[size++] = p;
            }
        } while ((data = readdir(z)) != NULL);
    }

    if (z)
    {
        closedir(z);
    }
    *count = size;
    return results;
}

static void
file_browser_reload_directory_content (struct file_browser *browser,
                                       const char          *path)
{
    const size_t path_len = nk_strlen(path) + 1;
    nu_memcpy(browser->directory, path, MIN(path_len, MAX_PATH_LEN));
    browser->directory[MAX_PATH_LEN - 1] = 0;
    dir_free_list(browser->files, browser->file_count);
    dir_free_list(browser->directories, browser->dir_count);
    browser->files       = dir_list(path, 0, &browser->file_count);
    browser->directories = dir_list(path, 1, &browser->dir_count);
}

static void
file_browser_init (struct file_browser *browser)
{
    memset(browser, 0, sizeof(*browser));
    {
        /* load files and sub-directory list */
        const char *home = getenv("HOME");
#ifdef _WIN32
        if (!home)
        {
            home = getenv("USERPROFILE");
        }
#else
        if (!home)
        {
            home = getpwuid(getuid())->pw_dir;
        }
        {
            size_t l;
            strncpy(browser->home, home, MAX_PATH_LEN);
            browser->home[MAX_PATH_LEN - 1] = 0;
            l                               = strlen(browser->home);
            strcpy(browser->home + l, "/");
            strcpy(browser->directory, browser->home);
        }
#endif
        {
            size_t l;
            strcpy(browser->desktop, browser->home);
            l = strlen(browser->desktop);
            strcpy(browser->desktop + l, "desktop/");
        }
        browser->files = dir_list(browser->directory, 0, &browser->file_count);
        browser->directories
            = dir_list(browser->directory, 1, &browser->dir_count);
    }
}

static void
file_browser_free (struct file_browser *browser)
{
    if (browser->files)
    {
        dir_free_list(browser->files, browser->file_count);
    }
    if (browser->directories)
    {
        dir_free_list(browser->directories, browser->dir_count);
    }
    browser->files       = NULL;
    browser->directories = NULL;
    memset(browser, 0, sizeof(*browser));
}

static int
file_browser_run (struct file_browser *browser, struct nk_context *ctx)
{
    int            ret = 0;
    struct nk_rect total_space;

    if (nk_begin(ctx,
                 "File Browser",
                 nk_rect(50, 50, 800, 600),
                 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE))
    {
        static float ratio[]   = { 0.25f, NK_UNDEFINED };
        float        spacing_x = ctx->style.window.spacing.x;

        /* output path directory selector in the menubar */
        ctx->style.window.spacing.x = 0;
        nk_menubar_begin(ctx);
        {
            char *d     = browser->directory;
            char *begin = d + 1;
            nk_layout_row_dynamic(ctx, 25, 6);
            while (*d++)
            {
                if (*d == '/')
                {
                    *d = '\0';
                    if (nk_button_label(ctx, begin))
                    {
                        *d++ = '/';
                        *d   = '\0';
                        file_browser_reload_directory_content(
                            browser, browser->directory);
                        break;
                    }
                    *d    = '/';
                    begin = d + 1;
                }
            }
        }
        nk_menubar_end(ctx);
        ctx->style.window.spacing.x = spacing_x;

        /* window layout */
        total_space = nk_window_get_content_region(ctx);
        nk_layout_row(ctx, NK_DYNAMIC, total_space.h, 2, ratio);
        nk_group_begin(ctx, "Special", NK_WINDOW_NO_SCROLLBAR);
        {

            nk_layout_row_dynamic(ctx, 40, 1);
            if (nk_button_label(ctx, "home"))
            {
                file_browser_reload_directory_content(browser, browser->home);
            }
            if (nk_button_label(ctx, "desktop"))
            {
                file_browser_reload_directory_content(browser,
                                                      browser->desktop);
            }
            if (nk_button_label(ctx, "computer"))
            {
                file_browser_reload_directory_content(browser, "/");
            }
            nk_group_end(ctx);
        }

        /* output directory content window */
        nk_group_begin(ctx, "Content", 0);
        {
            int    index = -1;
            size_t i = 0, j = 0, k = 0;
            size_t rows = 0, cols = 0;
            size_t count = browser->dir_count + browser->file_count;

            cols = 4;
            rows = count / cols;
            for (i = 0; i <= rows; i += 1)
            {
                {
                    size_t n = j + cols;
                    nk_layout_row_dynamic(ctx, 135, (int)cols);
                    for (; j < count && j < n; ++j)
                    {
                        /* draw one row of icons */
                        if (j < browser->dir_count)
                        {
                            /* draw and execute directory buttons */
                            if (nk_button_label(ctx, "DIR"))
                            {
                                index = (int)j;
                            }
                        }
                        else
                        {
                            /* draw and execute files buttons */
                            size_t fileIndex = ((size_t)j - browser->dir_count);
                            if (nk_button_label(ctx, "file"))
                            {
                                strncpy(browser->file,
                                        browser->directory,
                                        MAX_PATH_LEN);
                                n = strlen(browser->file);
                                strncpy(browser->file + n,
                                        browser->files[fileIndex],
                                        MAX_PATH_LEN - n);
                                ret = 1;
                            }
                        }
                    }
                }
                {
                    size_t n = k + cols;
                    nk_layout_row_dynamic(ctx, 20, (int)cols);
                    for (; k < count && k < n; k++)
                    {
                        /* draw one row of labels */
                        if (k < browser->dir_count)
                        {
                            nk_label(
                                ctx, browser->directories[k], NK_TEXT_CENTERED);
                        }
                        else
                        {
                            size_t t = k - browser->dir_count;
                            nk_label(ctx, browser->files[t], NK_TEXT_CENTERED);
                        }
                    }
                }
            }

            if (index != -1)
            {
                size_t n = strlen(browser->directory);
                strncpy(browser->directory + n,
                        browser->directories[index],
                        MAX_PATH_LEN - n);
                n = strlen(browser->directory);
                if (n < MAX_PATH_LEN - 1)
                {
                    browser->directory[n]     = '/';
                    browser->directory[n + 1] = '\0';
                }
                file_browser_reload_directory_content(browser,
                                                      browser->directory);
            }
            nk_group_end(ctx);
        }
    }
    nk_end(ctx);
    return ret;
}

void
view_home (struct nk_context *ctx, struct nk_rect bounds)
{
    // Inspector
    runtime_instance_t *instance = runtime_instance();
    const nu_size_t     row      = 30;
    struct nk_rect window_bounds = nk_rect(bounds.x, bounds.y, 300, bounds.h);
    struct nk_rect instance_bounds
        = nk_rect(window_bounds.w,
                  bounds.y,
                  NU_MAX(0, bounds.w - window_bounds.w),
                  bounds.h);
    if (nk_begin(ctx, "Games", window_bounds, 0))
    {
        // Cartridge
        static nu_size_t instance_count = 0;
        for (nu_size_t i = 0; i < instance_count; ++i)
        {
            nk_layout_row_dynamic(ctx, 110, 1);
            nu_char_t buf[64];
            nu_sv_fmt(buf, sizeof(buf), "Instance #%d", i);
            if (nk_group_begin(ctx, buf, NK_WINDOW_TITLE | NK_WINDOW_BORDER))
            {
                nk_layout_row_dynamic(ctx, row, 1);
                nk_button_label(ctx, "Load");
                nk_button_label(ctx, "Save");
                nk_group_end(ctx);
            }
        }
        // Instances
        nk_layout_row_dynamic(ctx, row, 1);
        {
            if (nk_button_symbol_label(
                    ctx, NK_SYMBOL_PLUS, "New instance", NK_TEXT_CENTERED))
            {
                ++instance_count;
            }
        }
        nk_checkbox_label(ctx, "Pause", &instance->pause);

        nk_layout_row_dynamic(ctx, 400, 1);
        if (nk_group_begin(ctx, "files", NK_WINDOW_TITLE))
        {
            nk_layout_row_dynamic(ctx, row, 1);
            nu_char_t        files[32][NU_PATH_MAX];
            static nu_bool_t selected[32] = { 0 };
            nu_size_t        count = nu_list_files(NU_SV("."), files, 32);
            for (nu_size_t i = 0; i < count; ++i)
            {
                nk_selectable_label(ctx, files[i], NK_TEXT_LEFT, selected + i);
            }
            nk_group_end(ctx);
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
        nk_end(ctx);
    }

    instance->viewport = instance_bounds;
}
