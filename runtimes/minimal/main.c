#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <nux.h>

static FILE *files[NUX_FILE_MAX];

void *
nux_os_alloc (void *p, nux_u32_t osize, nux_u32_t nsize)
{
    if (!p)
    {
        return malloc(nsize);
    }
    else if (!nsize)
    {
        free(p);
        return NULL;
    }
    else
    {
        return realloc(p, nsize);
    }
}
void
nux_os_log (nux_log_level_t level, const nux_c8_t *log, nux_u32_t n)
{
    printf("%.*s\n", n, log);
}
nux_status_t
nux_os_open_file (nux_u32_t       slot,
                  const nux_c8_t *path,
                  nux_u32_t       len,
                  nux_io_mode_t   mode)
{
    assert(files[slot] == NULL);
    nux_c8_t finalpath[NUX_PATH_BUF_SIZE + 2];
    snprintf(finalpath, sizeof(finalpath), "./%s", path);
    files[slot] = fopen(path, mode == NUX_IO_READ ? "rb" : "wb");
    if (!files[slot])
    {
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
void
nux_os_close_file (nux_u32_t slot)
{
    fclose(files[slot]);
    files[slot] = NULL;
}
nux_status_t
nux_os_file_stat (nux_u32_t slot, nux_os_file_stat_t *stat)
{
    long cursor = ftell(files[slot]);
    if (fseek(files[slot], 0, SEEK_END) < 0)
    {
        return NUX_FAILURE;
    }
    stat->size = ftell(files[slot]);
    fseek(files[slot], cursor, SEEK_SET);
    return NUX_SUCCESS;
}
nux_status_t
nux_os_seek_file (nux_u32_t slot, nux_u32_t cursor)
{
    return (fseek(files[slot], cursor, SEEK_SET) < 0) ? NUX_FAILURE
                                                      : NUX_SUCCESS;
}
nux_u32_t
nux_os_read_file (nux_u32_t slot, void *p, nux_u32_t n)
{
    return fread(p, 1, n, files[slot]);
}
nux_u32_t
nux_os_write_file (nux_u32_t slot, const void *p, nux_u32_t n)
{
    return fwrite(p, 1, n, files[slot]);
}
nux_status_t
nux_os_pipeline_create (nux_u32_t slot, const nux_gpu_pipeline_info_t *info)
{
    return NUX_SUCCESS;
}
void
nux_os_pipeline_delete (nux_u32_t slot)
{
}
nux_status_t
nux_os_framebuffer_create (nux_u32_t slot, nux_u32_t texture)
{
    return NUX_SUCCESS;
}
void
nux_os_framebuffer_delete (nux_u32_t slot)
{
}
nux_status_t
nux_os_texture_create (nux_u32_t slot, const nux_gpu_texture_info_t *info)
{
    return NUX_SUCCESS;
}
void
nux_os_texture_delete (nux_u32_t slot)
{
}
nux_status_t
nux_os_texture_update (nux_u32_t   slot,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_buffer_create (nux_u32_t             slot,
                      nux_gpu_buffer_type_t type,
                      nux_u32_t             size)
{
    return NUX_SUCCESS;
}
void
nux_os_buffer_delete (nux_u32_t slot)
{
}
nux_status_t
nux_os_buffer_update (nux_u32_t   slot,
                      nux_u32_t   offset,
                      nux_u32_t   size,
                      const void *data)
{
    return NUX_SUCCESS;
}
void
nux_os_gpu_submit (const nux_gpu_command_t *cmds, nux_u32_t count)
{
}
void
nux_os_update_stats (nux_u64_t *stats)
{
}
void
nux_os_add_hotreload (const nux_c8_t *path, nux_rid_t handle)
{
}
void
nux_os_remove_hotreload (nux_rid_t handle)
{
}
void
nux_os_pull_hotreload (nux_rid_t *handles, nux_u32_t *count)
{
    *count = 0;
}

static void
run (void)
{
    NUX_INFO("Hello World !");
    nux_module_requires("graphics");
}

int
main (int argc, char *argv[])
{
    nux_core_init();
    run();
    nux_core_free();
    return 0;
}
