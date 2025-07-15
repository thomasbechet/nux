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
                  nux_u32_t       n)
{
    if (files[slot])
    {
        fclose(files[slot]);
    }
    files[slot] = fopen(path, "r");
    if (!files[slot])
    {
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
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
nux_status_t
nux_os_file_read (void *userdata, nux_u32_t slot, void *p, nux_u32_t n)
{
    return fread(p, n, 1, files[slot]) == 1 ? NUX_SUCCESS : NUX_FAILURE;
}
