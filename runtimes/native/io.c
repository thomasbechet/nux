#include "internal.h"

void
io_init (void)
{
    memset(runtime.files, 0, sizeof(runtime.files));
}
void
io_free (void)
{
    for (int i = 0; i < (int)ARRAY_LEN(runtime.files); ++i)
    {
        if (runtime.files[i])
        {
            fclose(runtime.files[i]);
        }
    }
}

nux_status_t
nux_os_file_open (void           *userdata,
                  nux_u32_t       slot,
                  const nux_c8_t *path,
                  nux_u32_t       n,
                  nux_io_mode_t   mode)
{
    assert(runtime.files[slot] == NULL);
    nux_c8_t finalpath[NUX_PATH_BUF_SIZE + 2];
    snprintf(finalpath, sizeof(finalpath), "./%s", path);
    runtime.files[slot] = fopen(path, mode == NUX_IO_READ ? "rb" : "wb");
    if (!runtime.files[slot])
    {
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
void
nux_os_file_close (void *userdata, nux_u32_t slot)
{
    fclose(runtime.files[slot]);
    runtime.files[slot] = NULL;
}
nux_status_t
nux_os_file_stat (void *userdata, nux_u32_t slot, nux_file_stat_t *stat)
{
    long cursor = ftell(runtime.files[slot]);
    if (fseek(runtime.files[slot], 0, SEEK_END) < 0)
    {
        return NUX_FAILURE;
    }
    stat->size = ftell(runtime.files[slot]);
    fseek(runtime.files[slot], cursor, SEEK_SET);
    return NUX_SUCCESS;
}
nux_status_t
nux_os_file_seek (void *userdata, nux_u32_t slot, nux_u32_t n)
{
    return (fseek(runtime.files[slot], n, SEEK_SET) < 0) ? NUX_FAILURE
                                                         : NUX_SUCCESS;
}
nux_u32_t
nux_os_file_read (void *userdata, nux_u32_t slot, void *p, nux_u32_t n)
{
    return fread(p, 1, n, runtime.files[slot]);
}
nux_u32_t
nux_os_file_write (void *userdata, nux_u32_t slot, const void *p, nux_u32_t n)
{
    return fwrite(p, 1, n, runtime.files[slot]);
}
