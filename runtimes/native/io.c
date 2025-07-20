#include "internal.h"

static FILE *files[NUX_IO_FILE_MAX];

void
io_init (void)
{
    memset(files, 0, sizeof(files));
}
void
io_free (void)
{
    for (int i = 0; i < (int)ARRAY_LEN(files); ++i)
    {
        if (files[i])
        {
            fclose(files[i]);
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
    assert(files[slot] == NULL);
    files[slot] = fopen(path, mode == NUX_IO_READ ? "r" : "w");
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
    // reset file cursor
    fseek(files[slot], cursor, SEEK_SET);
    return NUX_SUCCESS;
}
nux_status_t
nux_os_file_seek (void *userdata, nux_u32_t slot, nux_u32_t n)
{
    return (fseek(files[slot], n, SEEK_SET) < 0) ? NUX_FAILURE : NUX_SUCCESS;
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
