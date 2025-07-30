#include "internal.h"

nux_res_t
nux_file_open (nux_ctx_t      *ctx,
               nux_res_t       arena,
               const nux_c8_t *path,
               nux_io_mode_t   mode)
{
    nux_res_t   res;
    nux_file_t *file
        = nux_arena_alloc_res(ctx, arena, NUX_RES_FILE, sizeof(*file), &res);
    NUX_CHECK(file, return NUX_NULL);

    if (!nux_io_open(ctx, path, mode, file))
    {
        return NUX_NULL;
    }

    return res;
}
void
nux_file_close (nux_ctx_t *ctx, nux_res_t res)
{
    nux_file_t *file = nux_res_check(ctx, NUX_RES_FILE, res);
    NUX_CHECK(file, return);
    nux_io_close(ctx, file);
}
nux_u32_t
nux_file_read (nux_ctx_t *ctx, nux_res_t res, void *data, nux_u32_t n)
{
    nux_file_t *file = nux_res_check(ctx, NUX_RES_FILE, res);
    NUX_CHECK(file, return 0);
    return nux_io_read(ctx, file, data, n);
}
nux_u32_t
nux_file_write (nux_ctx_t *ctx, nux_res_t res, const void *data, nux_u32_t n)
{
    nux_file_t *file = nux_res_check(ctx, NUX_RES_FILE, res);
    NUX_CHECK(file, return 0);
    return nux_io_write(ctx, file, data, n);
}
nux_status_t
nux_file_seek (nux_ctx_t *ctx, nux_res_t res, nux_u32_t cursor)
{
    nux_file_t *file = nux_res_check(ctx, NUX_RES_FILE, res);
    NUX_CHECK(file, return 0);
    return nux_io_seek(ctx, file, cursor);
}

void
nux_file_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_file_t *file = data;
    nux_io_close(ctx, file);
}
