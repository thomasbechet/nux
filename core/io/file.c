#include "internal.h"

nux_rid_t
nux_file_open (nux_ctx_t      *ctx,
               nux_rid_t       arena,
               const nux_c8_t *path,
               nux_io_mode_t   mode)
{
    nux_rid_t   rid;
    nux_file_t *file
        = nux_resource_new(ctx, arena, NUX_RESOURCE_FILE, sizeof(*file), &rid);
    NUX_CHECK(file, return NUX_NULL);

    if (!nux_io_open(ctx, path, mode, file))
    {
        return NUX_NULL;
    }

    return rid;
}
void
nux_file_close (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_file_t *file = nux_resource_check(ctx, NUX_RESOURCE_FILE, rid);
    NUX_CHECK(file, return);
    nux_io_close(ctx, file);
}
nux_u32_t
nux_file_read (nux_ctx_t *ctx, nux_rid_t rid, void *data, nux_u32_t n)
{
    nux_file_t *file = nux_resource_check(ctx, NUX_RESOURCE_FILE, rid);
    NUX_CHECK(file, return 0);
    return nux_io_read(ctx, file, data, n);
}
nux_u32_t
nux_file_write (nux_ctx_t *ctx, nux_rid_t rid, const void *data, nux_u32_t n)
{
    nux_file_t *file = nux_resource_check(ctx, NUX_RESOURCE_FILE, rid);
    NUX_CHECK(file, return 0);
    return nux_io_write(ctx, file, data, n);
}
nux_status_t
nux_file_seek (nux_ctx_t *ctx, nux_rid_t rid, nux_u32_t cursor)
{
    nux_file_t *file = nux_resource_check(ctx, NUX_RESOURCE_FILE, rid);
    NUX_CHECK(file, return 0);
    return nux_io_seek(ctx, file, cursor);
}

void
nux_file_cleanup (nux_ctx_t *ctx, nux_rid_t rid)
{
    nux_file_t *file = nux_resource_check(ctx, NUX_RESOURCE_FILE, rid);
    nux_io_close(ctx, file);
}
