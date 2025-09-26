#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <nux.h>

static FILE *files[NUX_IO_FILE_MAX];

void *
nux_os_alloc (void *userdata, void *p, nux_u32_t osize, nux_u32_t nsize)
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
nux_os_log (void           *userdata,
            nux_log_level_t level,
            const nux_c8_t *log,
            nux_u32_t       n)
{
    printf("%.*s\n", n, log);
}
nux_status_t
nux_os_file_open (void           *userdata,
                  nux_u32_t       slot,
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
nux_os_file_close (void *userdata, nux_u32_t slot)
{
    fclose(files[slot]);
    files[slot] = NULL;
}
nux_status_t
nux_os_file_stat (void *userdata, nux_u32_t slot, nux_file_stat_t *stat)
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
nux_os_file_seek (void *userdata, nux_u32_t slot, nux_u32_t cursor)
{
    return (fseek(files[slot], cursor, SEEK_SET) < 0) ? NUX_FAILURE
                                                      : NUX_SUCCESS;
}
nux_u32_t
nux_os_file_read (void *userdata, nux_u32_t slot, void *p, nux_u32_t n)
{
    return fread(p, 1, n, files[slot]);
}
nux_u32_t
nux_os_file_write (void *userdata, nux_u32_t slot, const void *p, nux_u32_t n)
{
    return fwrite(p, 1, n, files[slot]);
}
nux_status_t
nux_os_pipeline_create (void                          *userdata,
                        nux_u32_t                      slot,
                        const nux_gpu_pipeline_info_t *info)
{
    return NUX_SUCCESS;
}
void
nux_os_pipeline_delete (void *userdata, nux_u32_t slot)
{
}
nux_status_t
nux_os_framebuffer_create (void *userdata, nux_u32_t slot, nux_u32_t texture)
{
    return NUX_SUCCESS;
}
void
nux_os_framebuffer_delete (void *userdata, nux_u32_t slot)
{
}
nux_status_t
nux_os_texture_create (void                         *userdata,
                       nux_u32_t                     slot,
                       const nux_gpu_texture_info_t *info)
{
    return NUX_SUCCESS;
}
void
nux_os_texture_delete (void *userdata, nux_u32_t slot)
{
}
nux_status_t
nux_os_texture_update (void       *userdata,
                       nux_u32_t   slot,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_buffer_create (void                 *userdata,
                      nux_u32_t             slot,
                      nux_gpu_buffer_type_t type,
                      nux_u32_t             size)
{
    return NUX_SUCCESS;
}
void
nux_os_buffer_delete (void *userdata, nux_u32_t slot)
{
}
nux_status_t
nux_os_buffer_update (void       *userdata,
                      nux_u32_t   slot,
                      nux_u32_t   offset,
                      nux_u32_t   size,
                      const void *data)
{
    return NUX_SUCCESS;
}
void
nux_os_gpu_submit (void                    *userdata,
                   const nux_gpu_command_t *cmds,
                   nux_u32_t                count)
{
}
void
nux_os_input_update (void      *user,
                     nux_u32_t  controller,
                     nux_u32_t *buttons,
                     nux_f32_t *axis,
                     nux_f32_t *cursor)
{
}
void
nux_os_stats_update (void *userdata, nux_u64_t *stats)
{
}
void
nux_os_hotreload_add (void *userdata, const nux_c8_t *path, nux_rid_t handle)
{
}
void
nux_os_hotreload_remove (void *userdata, nux_rid_t handle)
{
}
void
nux_os_hotreload_pull (void *userdata, nux_rid_t *handles, nux_u32_t *count)
{
    *count = 0;
}

int
main (int argc, char *argv[])
{
    nux_instance_t *instance
        = nux_instance_init(NULL, argc > 1 ? argv[1] : "init.lua");
    nux_instance_update(instance);
    nux_instance_free(instance);
    return 0;
}
