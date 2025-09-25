#include "internal.h"

nux_file_t *
nux_file_open (nux_arena_t *arena, const nux_c8_t *path, nux_io_mode_t mode)
{
    nux_file_t *file = nux_resource_new(arena, NUX_RESOURCE_FILE);
    NUX_CHECK(file, return NUX_NULL);

    if (!nux_io_open(path, mode, file))
    {
        return NUX_NULL;
    }

    return file;
}
void
nux_file_close (nux_file_t *file)
{
    nux_io_close(file);
}
nux_u32_t
nux_file_read (nux_file_t *file, void *data, nux_u32_t n)
{
    return nux_io_read(file, data, n);
}
nux_u32_t
nux_file_write (nux_file_t *file, const void *data, nux_u32_t n)
{
    return nux_io_write(file, data, n);
}
nux_status_t
nux_file_seek (nux_file_t *file, nux_u32_t cursor)
{
    return nux_io_seek(file, cursor);
}

void
nux_file_cleanup (void *data)
{
    nux_io_close(data);
}
