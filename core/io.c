#include "internal.h"

static nux_status_t
cart_read (nux_ctx_t *ctx, nux_u32_t file, void *p, nux_u32_t n)
{
    if (nux_io_read(ctx, file, p, n) != n)
    {
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static nux_status_t
cart_write (nux_ctx_t *ctx, nux_u32_t file, const void *p, nux_u32_t n)
{
    if (nux_io_write(ctx, file, p, n) != n)
    {
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_u32 (nux_ctx_t *ctx, nux_u32_t f, nux_u32_t *v)
{
    NUX_CHECK(cart_read(ctx, f, v, sizeof(*v)), return NUX_FAILURE);
    *v = nux_u32_le(*v);
    return NUX_SUCCESS;
}
static nux_status_t
cart_write_u32 (nux_ctx_t *ctx, nux_u32_t f, nux_u32_t v)
{
    v = nux_u32_le(v);
    NUX_CHECK(cart_write(ctx, f, &v, sizeof(v)), return NUX_FAILURE);
    return NUX_SUCCESS;
}

static nux_status_t
cart_read_entry (nux_ctx_t *ctx, nux_cart_entry_t *entry)
{
}
static nux_status_t
cart_read_header (nux_ctx_t *ctx, nux_cart_t *cart)
{
    nux_u32_t    id, version, entry_count;
    nux_status_t status = NUX_SUCCESS;
    status &= cart_read_u32(ctx, cart->file_slot, &id);
    status &= cart_read_u32(ctx, cart->file_slot, &version);
    status &= cart_read_u32(ctx, cart->file_slot, &cart->entries_count);
    if (!status)
    {
        NUX_ERROR("Failed to read cart header");
        return NUX_FAILURE;
    }
    cart->entries
        = nux_arena_alloc(ctx, sizeof(*cart->entries) * cart->entries_count);
    if (!cart->entries)
    {
        NUX_ERROR("Failed to allocate %d entries", cart->entries_count);
        return NUX_FAILURE;
    }
    for (nux_u32_t i = 0; i < cart->entries_count; ++i)
    {
        nux_cart_entry_t *entry = cart->entries + i;
        status                  = NUX_SUCCESS;
        status &= cart_read_u32(ctx, cart->file_slot, &entry->data_type);
        status &= cart_read_u32(ctx, cart->file_slot, &entry->data_offset);
        status &= cart_read_u32(ctx, cart->file_slot, &entry->data_length);
        status &= cart_read_u32(ctx, cart->file_slot, &entry->path_hash);
        status &= cart_read_u32(ctx, cart->file_slot, &entry->path_offset);
        status &= cart_read_u32(ctx, cart->file_slot, &entry->path_length);
        if (!status)
        {
            NUX_ERROR("Failed to load cart entry %d", i);
            return NUX_FAILURE;
        }
    }
    return NUX_SUCCESS;
}

typedef struct
{
    nux_u32_t file_slot;
    nux_u32_t entry_count;
    nux_u32_t entry_index;
    nux_u32_t cursor;
} nux_cart_writer_t;

enum
{
    NUX_CART_HEADER_SIZE = 4 * 3,
    NUX_CART_ENTRY_SIZE  = 4 * 8,
};

nux_status_t
nux_cart_writer_begin (nux_ctx_t         *ctx,
                       nux_cart_writer_t *writer,
                       const nux_c8_t    *path,
                       nux_u32_t          entry_count)
{
    nux_u32_t slot = nux_io_open(ctx, path, NUX_IO_READ_WRITE);
    NUX_CHECK(slot, return NUX_FAILURE);

    writer->entry_count = entry_count;
    writer->entry_index = 0;
    writer->file_slot   = slot;
    writer->cursor = NUX_CART_HEADER_SIZE + NUX_CART_ENTRY_SIZE * entry_count;

    nux_status_t status = NUX_SUCCESS;
    status &= cart_write_u32(ctx, slot, 0xCA);
    status &= cart_write_u32(ctx, slot, 0xFE);
    status &= cart_write_u32(ctx, slot, entry_count);
    if (!status)
    {
        NUX_ERROR("Failed to write cart header");
        goto cleanup;
    }

cleanup:
    nux_io_close(ctx, slot);
    return NUX_FAILURE;
}
void
nux_cart_writer_end (nux_ctx_t *ctx, nux_cart_writer_t *writer)
{
    NUX_ASSERT(writer->entry_index != writer->entry_count - 1);
    nux_io_close(ctx, writer->file_slot);
}
nux_status_t
nux_cart_writer_entry (nux_ctx_t         *ctx,
                       nux_cart_writer_t *writer,
                       const nux_c8_t    *path,
                       nux_u32_t          type,
                       nux_b32_t          compress,
                       const void        *data,
                       nux_u32_t          size)
{
    nux_status_t status = NUX_SUCCESS;

    nux_u32_t data_cursor = writer->cursor;
    nux_u32_t entry_cursor
        = NUX_CART_HEADER_SIZE + NUX_CART_ENTRY_SIZE * writer->entry_index;

    nux_u32_t path_length = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t path_hash   = 0;
    nux_u32_t path_offset = writer->cursor;
    nux_u32_t data_type   = type;
    nux_u32_t data_offset = path_offset + path_length + 1; // null terminated
    nux_u32_t data_length = size;

    // Write path + data
    if (!nux_io_seek(ctx, writer->file_slot, writer->cursor))
    {
        NUX_ERROR("Failed to seek to data section");
        return NUX_FAILURE;
    }
    status &= nux_io_write(ctx, writer->file_slot, path, path_length);
    status &= nux_io_write(ctx, writer->file_slot, "\0", 1); // null terminated
    status &= nux_io_write(ctx, writer->file_slot, data, data_length);
    if (!status)
    {
        NUX_ERROR("Failed to write cart data entry %d", writer->entry_index);
        return NUX_FAILURE;
    }

    // Write entry
    if (!nux_io_seek(ctx, writer->file_slot, entry_cursor))
    {
        NUX_ERROR("Failed to seek entry");
        return NUX_FAILURE;
    }
    status &= cart_write_u32(ctx, writer->file_slot, data_type);
    status &= cart_write_u32(ctx, writer->file_slot, data_offset);
    status &= cart_write_u32(ctx, writer->file_slot, data_length);
    status &= cart_write_u32(ctx, writer->file_slot, path_hash);
    status &= cart_write_u32(ctx, writer->file_slot, path_offset);
    status &= cart_write_u32(ctx, writer->file_slot, path_length);
    if (!status)
    {
        NUX_ERROR("Failed to write cart entry %d", writer->entry_index);
        return NUX_FAILURE;
    }

    ++writer->entry_index;
    writer->cursor += path_length + data_length;

    return NUX_SUCCESS;
}

nux_status_t
nux_io_init (nux_ctx_t *ctx)
{
    NUX_CHECKM(nux_u32_vec_alloc(ctx, NUX_IO_FILE_MAX, &ctx->free_file_slots),
               "Failed to allocate file slots",
               goto error);

    nux_u32_vec_fill_reversed(&ctx->free_file_slots);
    nux_u32_vec_pop(&ctx->free_file_slots); // reserve 0 for null

    return NUX_SUCCESS;
error:
    return NUX_FAILURE;
}
nux_status_t
nux_io_free (nux_ctx_t *ctx)
{
    return NUX_SUCCESS;
}

nux_status_t
nux_io_mount (nux_ctx_t *ctx, const nux_c8_t *path)
{
    NUX_CHECKM(ctx->carts_count < NUX_ARRAY_SIZE(ctx->carts),
               "Out of carts",
               return NUX_FAILURE);
    nux_u32_t file_slot = nux_io_open(ctx, path, NUX_IO_READ);
    if (!file_slot)
    {
        NUX_ERROR("Failed to open cart %s", path);
        return NUX_FAILURE;
    }
    nux_cart_t *cart = ctx->carts + ctx->carts_count;
    ++ctx->carts_count;
    cart->file_slot = file_slot;

    return NUX_SUCCESS;
}
nux_status_t
nux_io_unmount (nux_ctx_t *ctx, const nux_c8_t *path)
{
    return NUX_SUCCESS;
}
nux_u32_t
nux_io_open (nux_ctx_t *ctx, const nux_c8_t *path, nux_io_mode_t mode)
{

    // Open workspace file
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_file_slots);
    NUX_CHECKM(slot, "Out of file slots", return NUX_NULL);
    NUX_CHECK(
        nux_os_file_open(
            ctx->userdata, *slot, path, nux_strnlen(path, NUX_PATH_MAX), mode),
        goto cleanup);
    return *slot;
cleanup:
    nux_u32_vec_pushv(&ctx->free_file_slots, *slot);
    return NUX_NULL;
}
void
nux_io_close (nux_ctx_t *ctx, nux_u32_t slot)
{
    nux_os_file_close(ctx->userdata, slot);
    nux_u32_vec_pushv(&ctx->free_file_slots, slot);
}
nux_u32_t
nux_io_read (nux_ctx_t *ctx, nux_u32_t slot, void *data, nux_u32_t n)
{
    return nux_os_file_read(ctx->userdata, slot, data, n);
}
nux_u32_t
nux_io_write (nux_ctx_t *ctx, nux_u32_t slot, const void *data, nux_u32_t n)
{
    return nux_os_file_write(ctx->userdata, slot, data, n);
}
nux_status_t
nux_io_seek (nux_ctx_t *ctx, nux_u32_t slot, nux_u32_t cursor)
{
    return nux_os_file_seek(ctx->userdata, slot, cursor);
}

void *
nux_io_load (nux_ctx_t *ctx, const nux_c8_t *path, nux_u32_t *size)
{
    nux_u32_t path_length = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t path_hash   = nux_hash(path, path_length);

    // Find file in cartridges
    {
        for (nux_u32_t c = 0; c < ctx->carts_count; ++c)
        {
            nux_cart_t *cart = ctx->carts + c;
            for (nux_u32_t i = 0; i < cart->entries_count; ++i)
            {
                nux_cart_entry_t *entry = cart->entries + i;
                if (entry->path_hash == path_hash)
                {
                    void *data = nux_arena_alloc(ctx, entry->data_length);
                    if (!data)
                    {
                        NUX_ERROR("Failed to allocate file memory");
                        return NUX_NULL;
                    }
                    nux_status_t status = NUX_SUCCESS;
                    status &= nux_io_seek(
                        ctx, cart->file_slot, entry->data_offset);
                    status &= nux_io_read(
                        ctx, cart->file_slot, data, entry->data_length);
                    if (!status)
                    {
                        NUX_ERROR("Failed to read file from cart");
                        return NUX_NULL;
                    }
                }
            }
        }
    }

    {
        // File not found, try to load the file directly
        nux_file_stat_t stat;

        // Open file at beginning
        nux_u32_t slot = nux_io_open(ctx, path, NUX_IO_READ);
        NUX_CHECK(slot, return NUX_NULL);
        NUX_CHECKM(nux_os_file_stat(ctx->userdata, slot, &stat),
                   "Failed to retrieve file stat",
                   goto cleanup);
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
        void *data = nux_arena_alloc(ctx, stat.size);
        if (!data)
        {
            NUX_ERROR("Failed to allocate file memory");
            return NUX_NULL;
        }

        // Read file
        NUX_CHECKM(nux_io_read(ctx, slot, data, stat.size),
                   "Failed to read file",
                   goto cleanup);

        return data;
    cleanup:
        nux_io_close(ctx, slot);
        return NUX_NULL;
    }
}
