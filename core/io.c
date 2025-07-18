#include "internal.h"

nux_status_t
nux_io_init (nux_ctx_t *ctx)
{
    NUX_CHECKM(nux_u32_vec_alloc(ctx, NUX_IO_FILE_MAX, &ctx->free_file_slots),
               "Failed to allocate file slots",
               goto error);

    nux_u32_vec_fill_reversed(&ctx->free_file_slots);
    // Reserve slot 0 to NULL file
    nux_u32_vec_pop(&ctx->free_file_slots);

    return NUX_SUCCESS;
error:
    return NUX_FAILURE;
}
nux_status_t
nux_io_free (nux_ctx_t *ctx)
{
    return NUX_SUCCESS;
}

nux_u32_t
nux_io_open (nux_ctx_t *ctx, const nux_c8_t *path)
{
    // Reserve slot
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_file_slots);
    NUX_CHECKM(slot, "Out of file slots", return NUX_NULL);
    NUX_CHECK(nux_os_file_open(
                  ctx->userdata, *slot, path, nux_strnlen(path, NUX_PATH_MAX)),
              goto cleanup);

    return *slot;

cleanup:
    nux_u32_vec_pushv(&ctx->free_file_slots, *slot);
    return NUX_NULL;
}
void
nux_io_close (nux_ctx_t *ctx, nux_u32_t slot)
{
    // TODO: os close function ?
    nux_u32_vec_pushv(&ctx->free_file_slots, slot);
}
nux_u32_t
nux_io_read (nux_ctx_t *ctx, nux_u32_t slot, void *data, nux_u32_t n)
{
    return nux_os_file_read(ctx->userdata, slot, data, n);
}
void *
nux_io_load (nux_ctx_t *ctx, const nux_c8_t *path, nux_u32_t *size)
{
    nux_file_stat_t stat;
    void           *data = NUX_NULL;

    // Open file at beginning
    nux_u32_t slot = nux_io_open(ctx, path);
    NUX_CHECK(slot, return NUX_NULL);
    NUX_CHECKM(nux_os_file_stat(ctx->userdata, slot, &stat),
               "Failed to retrieve file stat",
               goto cleanup);
    NUX_INFO("file %d", stat.size);
    NUX_CHECKM(nux_os_file_seek(ctx->userdata, slot, 0),
               "Failed to seek file",
               goto cleanup);

    if (size)
    {
        *size = stat.size;
    }
    if (!stat.size) // empty file
    {
        goto cleanup;
    }

    // Allocate memory
    data = nux_arena_alloc(ctx, stat.size);
    NUX_CHECK(data, goto cleanup);

    // Read file
    NUX_CHECKM(nux_io_read(ctx, slot, data, stat.size),
               "Failed to read file",
               goto cleanup);

    return data;
cleanup:
    nux_io_close(ctx, slot);
    return NUX_NULL;
}
