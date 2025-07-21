#include "internal.h"
#include "nux.h"

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
cart_read_header (nux_ctx_t *ctx, nux_disk_ *cart)
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

static nux_status_t
concat_path (nux_c8_t *dst, const nux_c8_t *a, const nux_c8_t *b)
{
    nux_u32_t a_len = nux_strnlen(a, NUX_PATH_MAX);
    nux_u32_t b_len = nux_strnlen(b, NUX_PATH_MAX);
    if (a_len + b_len + 1 > NUX_PATH_MAX)
    {
        return NUX_FAILURE;
    }
    nux_memcpy(dst, a, a_len);
    dst[a_len] = '/';
    nux_memcpy(dst + a_len + 1, b, b_len);
    dst[a_len + b_len + 1] = '\0';
    return NUX_SUCCESS;
}
static nux_u32_t
open_os_file (nux_ctx_t      *ctx,
              const nux_c8_t *path,
              nux_u32_t       len,
              nux_io_mode_t   mode)
{
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_file_slots);
    NUX_CHECKM(slot, "Out of os file slots", return NUX_NULL);
    NUX_CHECK(nux_os_file_open(ctx->userdata, *slot, path, len, mode),
              goto cleanup);
    return *slot;
cleanup:
    nux_u32_vec_pushv(&ctx->free_file_slots, *slot);
    return NUX_NULL;
}
static void
close_os_file (nux_ctx_t *ctx, nux_u32_t slot)
{
    nux_os_file_close(ctx->userdata, slot);
    nux_u32_vec_pushv(&ctx->free_file_slots, slot);
}
static nux_status_t
open_file (nux_ctx_t      *ctx,
           const nux_c8_t *path,
           nux_io_mode_t   mode,
           nux_file_t     *file)
{
    for (nux_u32_t d = 0; d < ctx->disks_count; ++d)
    {
        nux_disk_t *disk = ctx->disks + d;
        if (disk->type == NUX_DISK_OS)
        {
            nux_c8_t os_path[NUX_PATH_BUF_SIZE];
            if (!concat_path(os_path, disk->path, path))
            {
                return NUX_FAILURE;
            }
            nux_u32_t slot = open_os_file(
                ctx, os_path, nux_strnlen(os_path, NUX_PATH_MAX), mode);
            if (slot)
            {
                file->type    = NUX_DISK_OS;
                file->mode    = mode;
                file->os.slot = slot;
                return NUX_SUCCESS;
            }
        }
        else if (disk->type == NUX_DISK_CART)
        {
            nux_u32_t hash = nux_hash(path, nux_strnlen(path, NUX_PATH_MAX));
            for (nux_u32_t i = 0; i < disk->cart.entries_count; ++i)
            {
                nux_cart_entry_t *entry = disk->cart.entries + i;
                if (entry->path_hash == hash)
                {
                    file->type        = NUX_DISK_CART;
                    file->mode        = mode;
                    file->cart.slot   = disk->cart.slot;
                    file->cart.offset = entry->data_offset;
                    file->cart.length = entry->data_length;
                    file->cart.cursor = 0;
                    return NUX_SUCCESS;
                }
            }
        }
    }
    return NUX_FAILURE;
}
static void
close_file (nux_ctx_t *ctx, nux_file_t *file)
{
    if (file->type == NUX_DISK_OS)
    {
        close_os_file(ctx, file->os.slot);
    }
}
static nux_u32_t
read_file (nux_ctx_t *ctx, nux_file_t *file, void *data, nux_u32_t n)
{
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_read(ctx->userdata, file->os.slot, data, n);
    }
    else if (file->type == NUX_DISK_CART)
    {
        nux_u32_t read = NUX_MIN(file->cart.length - file->cart.cursor, n);
        if (!read)
        {
            return 0;
        }
        nux_os_file_seek(ctx->userdata,
                         file->cart.slot,
                         file->cart.offset + file->cart.cursor);
        nux_u32_t got
            = nux_os_file_read(ctx->userdata, file->cart.slot, data, read);
        if (got != read)
        {
            NUX_ERROR("Failed to load cart data at %d (expected %d, got %d)",
                      file->cart.offset,
                      read,
                      got);
            return 0;
        }
        file->cart.cursor += read;
        return read;
    }
    return 0;
}
static nux_u32_t
write_file (nux_ctx_t *ctx, nux_file_t *file, const void *data, nux_u32_t n)
{
    if (file->mode == NUX_IO_READ)
    {
        NUX_ERROR("Failed to write read only file");
        return 0;
    }
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_write(ctx->userdata, file->os.slot, data, n);
    }
    else if (file->type == NUX_DISK_CART)
    {
        nux_u32_t write = NUX_MIN(file->cart.length - file->cart.cursor, n);
        nux_os_file_seek(ctx->userdata,
                         file->cart.slot,
                         file->cart.offset + file->cart.cursor);
        nux_u32_t got
            = nux_os_file_write(ctx->userdata, file->cart.slot, data, write);
        if (got != write)
        {
            NUX_ERROR("Failed to write cart data at %d (expected %d, got %d)",
                      file->cart.offset,
                      write,
                      got);
            return 0;
        }
        file->cart.cursor += write;
        return write;
    }
    return 0;
}
static nux_status_t
seek_file (nux_ctx_t *ctx, nux_file_t *file, nux_u32_t cursor)
{
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_seek(ctx->userdata, file->os.slot, cursor);
    }
    else if (file->type == NUX_DISK_CART)
    {
        if (cursor > file->cart.length)
        {
            return NUX_FAILURE;
        }
        file->cart.cursor = cursor;
        return NUX_SUCCESS;
    }
    return NUX_FAILURE;
}

nux_status_t
nux_io_mount (nux_ctx_t *ctx, nux_disk_type_t type, const nux_c8_t *path)
{
    NUX_CHECKM(ctx->disks_count < NUX_ARRAY_SIZE(ctx->disks),
               "Out of disks",
               return NUX_FAILURE);

    nux_u32_t   path_len = nux_strnlen(path, NUX_PATH_MAX);
    nux_disk_t *disk     = ctx->disks + ctx->disks_count;
    disk->type           = type;
    nux_strncpy(disk->path, path, path_len);
    switch (type)
    {
        case NUX_DISK_CART: {
            nux_u32_t file_slot
                = open_os_file(ctx->userdata, path, path_len, NUX_IO_READ);
            if (!file_slot)
            {
                NUX_ERROR("Failed to open cart %s", path);
                return NUX_FAILURE;
            }
            disk->cart.slot = file_slot;
        }
        break;
        default:
            break;
    }

    ++ctx->disks_count;

    return NUX_SUCCESS;
}
nux_u32_t
nux_io_open (nux_ctx_t *ctx, const nux_c8_t *path, nux_io_mode_t mode)
{
    nux_u32_t   id;
    nux_file_t *file
        = nux_arena_alloc_type(ctx, NUX_TYPE_FILE, sizeof(*file), &id);
    NUX_CHECK(file, return NUX_NULL);

    if (!open_file(ctx, path, mode, file))
    {
        return NUX_NULL;
    }

    return id;
}
void
nux_io_close (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_file_t *file = nux_id_get(ctx, NUX_TYPE_FILE, id);
    NUX_CHECK(file, return);
    close_file(ctx, file);
}
nux_u32_t
nux_io_read (nux_ctx_t *ctx, nux_u32_t id, void *data, nux_u32_t n)
{
    nux_file_t *file = nux_id_get(ctx, NUX_TYPE_FILE, id);
    NUX_CHECK(file, return 0);
    return read_file(ctx, file, data, n);
}
nux_u32_t
nux_io_write (nux_ctx_t *ctx, nux_u32_t id, const void *data, nux_u32_t n)
{
    nux_file_t *file = nux_id_get(ctx, NUX_TYPE_FILE, id);
    NUX_CHECK(file, return 0);
    return write_file(ctx, file, data, n);
}
nux_status_t
nux_io_seek (nux_ctx_t *ctx, nux_u32_t id, nux_u32_t cursor)
{
    nux_file_t *file = nux_id_get(ctx, NUX_TYPE_FILE, id);
    NUX_CHECK(file, return 0);
    return seek_file(ctx, file, cursor);
}

void *
nux_io_load (nux_ctx_t *ctx, const nux_c8_t *path, nux_u32_t *size)
{
    nux_u32_t path_length = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t path_hash   = nux_hash(path, path_length);

    // Find file in cartridges
    {
        for (nux_u32_t c = 0; c < ctx->disks_count; ++c)
        {
            nux_disk_ *cart = ctx->disks + c;
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

void
nux_file_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_file_t *file = data;
    close_file(ctx, file);
}
