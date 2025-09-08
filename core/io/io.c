#include "internal.h"

static nux_status_t
open_os_file (nux_ctx_t      *ctx,
              const nux_c8_t *path,
              nux_io_mode_t   mode,
              nux_u32_t      *ret_slot)
{
    nux_io_module_t *module = ctx->io;

    nux_c8_t  normpath[NUX_PATH_BUF_SIZE];
    nux_u32_t len = nux_path_normalize(normpath, path);
    NUX_ENSURE(!nux_path_isdir(normpath),
               return NUX_NULL,
               "trying to open a directory '%s' as file",
               normpath);
    nux_u32_t *slot = nux_u32_vec_pop(&module->free_file_slots);
    NUX_ENSURE(slot, return NUX_NULL, "out of os file slots");
    NUX_ENSURE(nux_os_file_open(ctx->userdata, *slot, normpath, len, mode),
               goto cleanup,
               "failed to open os file '%s'",
               normpath);
    *ret_slot = *slot;
    return NUX_SUCCESS;
cleanup:
    nux_u32_vec_pushv(&module->free_file_slots, *slot);
    return NUX_FAILURE;
}
static void
close_os_file (nux_ctx_t *ctx, nux_u32_t slot)
{
    nux_os_file_close(ctx->userdata, slot);
    nux_u32_vec_pushv(&ctx->io->free_file_slots, slot);
}

nux_status_t
nux_io_init (nux_ctx_t *ctx)
{
    ctx->io = nux_arena_alloc(ctx, ctx->core_arena_rid, sizeof(*ctx->io));
    NUX_CHECK(ctx->io, return NUX_FAILURE);

    nux_io_module_t *module = ctx->io;

    NUX_CHECK(nux_u32_vec_alloc(
                  &ctx->core_arena, NUX_IO_FILE_MAX, &module->free_file_slots),
              goto error);

    // Register types
    nux_resource_type_t *type;
    type          = nux_resource_register(ctx, NUX_RESOURCE_FILE, "file");
    type->cleanup = nux_file_cleanup;

    // Initialize values
    nux_u32_vec_fill_reversed(&module->free_file_slots);
    module->disks_count = 0;

    // Add OS disk
    nux_disk_t *disk = module->disks + module->disks_count;
    disk->type       = NUX_DISK_OS;
    ++module->disks_count;

    // Initialize controllers
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        nux_controller_t *controller      = module->controllers + i;
        controller->mode                  = NUX_CONTROLLER_MODE_MOTION;
        controller->cursor_motion_axis[0] = NUX_AXIS_LEFTX;
        controller->cursor_motion_axis[1] = NUX_AXIS_LEFTY;
        controller->cursor_motion_speed   = 100;
    }

    return NUX_SUCCESS;
error:
    return NUX_FAILURE;
}
nux_status_t
nux_io_free (nux_ctx_t *ctx)
{
    nux_io_module_t *module = ctx->io;

    // Unmount disks
    for (nux_u32_t i = 0; i < module->disks_count; ++i)
    {
        nux_disk_t *disk = module->disks + i;
        if (disk->type == NUX_DISK_CART)
        {
            close_os_file(ctx, disk->cart.slot);
        }
    }

    NUX_ASSERT(module->free_file_slots.size == NUX_IO_FILE_MAX);

    return NUX_SUCCESS;
}

static nux_status_t
cart_read (nux_ctx_t *ctx, nux_u32_t slot, void *p, nux_u32_t n)
{
    NUX_ENSURE(nux_os_file_read(ctx, slot, p, n) == n,
               return NUX_FAILURE,
               "failed to read cart file");

    return NUX_SUCCESS;
}
static nux_status_t
cart_read_u32 (nux_ctx_t *ctx, nux_u32_t slot, nux_u32_t *v)
{
    NUX_CHECK(cart_read(ctx, slot, v, sizeof(*v)), return NUX_FAILURE);
    *v = nux_u32_le(*v);

    return NUX_SUCCESS;
}
static nux_status_t
cart_read_entries (nux_ctx_t *ctx, nux_cart_t *cart)
{
    nux_u32_t    id, version, entry_count;
    nux_status_t status = NUX_SUCCESS;
    status &= cart_read_u32(ctx, cart->slot, &id);
    status &= cart_read_u32(ctx, cart->slot, &version);
    status &= cart_read_u32(ctx, cart->slot, &cart->entries_count);
    NUX_CHECK(status, return NUX_FAILURE);
    cart->entries = nux_arena_alloc_raw(
        &ctx->core_arena, sizeof(*cart->entries) * cart->entries_count);
    NUX_CHECK(cart->entries, return NUX_FAILURE);
    for (nux_u32_t i = 0; i < cart->entries_count; ++i)
    {
        nux_cart_entry_t *entry = cart->entries + i;
        status                  = NUX_SUCCESS;
        status &= cart_read_u32(ctx, cart->slot, &entry->data_type);
        status &= cart_read_u32(ctx, cart->slot, &entry->data_offset);
        status &= cart_read_u32(ctx, cart->slot, &entry->data_length);
        status &= cart_read_u32(ctx, cart->slot, &entry->path_hash);
        status &= cart_read_u32(ctx, cart->slot, &entry->path_offset);
        status &= cart_read_u32(ctx, cart->slot, &entry->path_length);
        NUX_CHECK(status, return NUX_FAILURE);
    }

    {
        for (nux_u32_t i = 0; i < cart->entries_count; ++i)
        {
            nux_cart_entry_t *entry = cart->entries + i;
            nux_os_file_seek(ctx->userdata, cart->slot, entry->path_offset);
            nux_c8_t path[NUX_PATH_BUF_SIZE];
            nux_os_file_read(
                ctx->userdata, cart->slot, path, entry->path_length + 1);
            NUX_DEBUG("[%d] 0x%08X %s", i, entry->path_hash, path);
        }
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_io_mount (nux_ctx_t *ctx, const nux_c8_t *path)
{
    nux_io_module_t *module = ctx->io;

    NUX_DEBUG("mounting '%s", path);
    NUX_ENSURE(module->disks_count < NUX_ARRAY_SIZE(module->disks),
               return NUX_FAILURE,
               "out of disks");

    // Prepare disk path
    nux_disk_t *disk   = module->disks + module->disks_count;
    disk->type         = NUX_DISK_CART;
    nux_u32_t path_len = nux_strnlen(path, NUX_PATH_MAX);
    nux_strncpy(disk->cart.path, path, path_len);

    // Open file
    nux_u32_t file_slot;
    NUX_CHECK(open_os_file(ctx, path, NUX_IO_READ, &file_slot),
              return NUX_FAILURE);
    disk->cart.slot = file_slot;

    // Read entries
    NUX_CHECK(cart_read_entries(ctx, &disk->cart), return NUX_FAILURE);
    ++module->disks_count;

    return NUX_SUCCESS;
}

nux_b32_t
nux_io_exists (nux_ctx_t *ctx, const nux_c8_t *path)
{
    nux_file_t   file;
    nux_status_t status = nux_io_open(ctx, path, NUX_IO_READ, &file);
    if (status)
    {
        nux_io_close(ctx, &file);
    }
    nux_error_reset(ctx); // ignore error if any
    return status ? NUX_TRUE : NUX_FALSE;
}
nux_status_t
nux_io_open (nux_ctx_t      *ctx,
             const nux_c8_t *path,
             nux_io_mode_t   mode,
             nux_file_t     *file)
{
    nux_io_module_t *module = ctx->io;

    for (nux_u32_t d = 0; d < module->disks_count; ++d)
    {
        nux_disk_t *disk = module->disks + d;
        if (disk->type == NUX_DISK_OS)
        {
            nux_u32_t slot;
            nux_error_disable(ctx);
            nux_status_t status = open_os_file(ctx, path, mode, &slot);
            nux_error_enable(ctx);
            if (status)
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
    NUX_ENSURE(NUX_FALSE, ;, "file not found '%s'", path);
    return NUX_FAILURE;
}
void
nux_io_close (nux_ctx_t *ctx, nux_file_t *file)
{
    if (file->type == NUX_DISK_OS)
    {
        close_os_file(ctx, file->os.slot);
    }
}
nux_u32_t
nux_io_read (nux_ctx_t *ctx, nux_file_t *file, void *data, nux_u32_t n)
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
        NUX_ENSURE(
            got == read,
            return 0,
            "failed to load cart data at offset %d (expected %d, got %d)",
            file->cart.offset,
            read,
            got);
        file->cart.cursor += read;
        return read;
    }
    return 0;
}
nux_u32_t
nux_io_write (nux_ctx_t *ctx, nux_file_t *file, const void *data, nux_u32_t n)
{
    NUX_ENSURE(
        file->mode != NUX_IO_READ, return 0, "failed to write read only file");
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
        NUX_ENSURE(got == write,
                   return 0,
                   "failed to write cart data at %d (expected %d, got %d)",
                   file->cart.offset,
                   write,
                   got);
        file->cart.cursor += write;
        return write;
    }
    return 0;
}
nux_status_t
nux_io_seek (nux_ctx_t *ctx, nux_file_t *file, nux_u32_t cursor)
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
nux_io_stat (nux_ctx_t *ctx, nux_file_t *file, nux_file_stat_t *stat)
{
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_stat(ctx->userdata, file->os.slot, stat);
    }
    else if (file->type == NUX_DISK_CART)
    {
        stat->size = file->cart.length;
        return NUX_SUCCESS;
    }
    return NUX_FAILURE;
}
void *
nux_io_load (nux_ctx_t      *ctx,
             nux_rid_t       arena,
             const nux_c8_t *path,
             nux_u32_t      *size)
{
    void      *data = NUX_NULL;
    nux_file_t file;
    NUX_CHECK(nux_io_open(ctx, path, NUX_IO_READ, &file), return NUX_NULL);

    // Get the file size
    nux_file_stat_t stat;
    NUX_CHECK(nux_io_stat(ctx, &file, &stat), goto cleanup0);

    // Seek to beginning
    NUX_CHECK(nux_io_seek(ctx, &file, 0), goto cleanup0);

    // Return buffer size to user
    if (size)
    {
        *size = stat.size;
    }
    if (!stat.size) // empty file
    {
        goto cleanup0;
    }

    // Allocate memory
    data = nux_arena_alloc(ctx, arena, stat.size);
    NUX_CHECK(data, goto cleanup0);

    // Read file
    nux_u32_t read = nux_io_read(ctx, &file, data, stat.size);
    NUX_ENSURE(
        read == stat.size,
        goto cleanup0,
        "failed to read file content '%s' (expect %d bytes, got %d bytes)",
        path,
        stat.size,
        read);

    nux_io_close(ctx, &file);
    return data;

cleanup0:
    nux_io_close(ctx, &file);
    return NUX_NULL;
}

static nux_status_t
cart_write (nux_ctx_t *ctx, nux_u32_t slot, const void *p, nux_u32_t n)
{
    NUX_ENSURE(nux_os_file_write(ctx, slot, p, n) == n,
               return NUX_FAILURE,
               "failed to write cart file");
    return NUX_SUCCESS;
}
static nux_status_t
cart_write_u32 (nux_ctx_t *ctx, nux_u32_t slot, nux_u32_t v)
{
    v = nux_u32_le(v);
    NUX_CHECK(cart_write(ctx, slot, &v, sizeof(v)), return NUX_FAILURE);
    return NUX_SUCCESS;
}

nux_status_t
nux_io_cart_begin (nux_ctx_t *ctx, const nux_c8_t *path, nux_u32_t entry_count)
{
    nux_io_module_t *module = ctx->io;

    if (module->cart_writer.started)
    {
        NUX_CHECK(nux_io_cart_end(ctx), return NUX_FAILURE);
    }

    nux_u32_t slot;
    NUX_CHECK(open_os_file(ctx, path, NUX_IO_READ_WRITE, &slot),
              return NUX_FAILURE);

    module->cart_writer.slot        = slot;
    module->cart_writer.entry_count = entry_count;
    module->cart_writer.entry_index = 0;
    module->cart_writer.cursor
        = NUX_CART_HEADER_SIZE + NUX_CART_ENTRY_SIZE * entry_count;

    nux_status_t status = NUX_SUCCESS;
    status &= cart_write_u32(ctx, module->cart_writer.slot, 0xCA);
    status &= cart_write_u32(ctx, module->cart_writer.slot, 0xFE);
    status &= cart_write_u32(ctx, module->cart_writer.slot, entry_count);
    NUX_CHECK(status, goto cleanup);

    module->cart_writer.started = NUX_TRUE;

    return NUX_SUCCESS;

cleanup:
    close_os_file(ctx, slot);
    return NUX_FAILURE;
}
nux_status_t
nux_io_cart_end (nux_ctx_t *ctx)
{
    nux_io_module_t *module = ctx->io;

    if (module->cart_writer.started)
    {
        NUX_ENSURE(module->cart_writer.entry_index
                       == module->cart_writer.entry_index,
                   return NUX_FAILURE,
                   "missing cart entries (got %d, expect %d)",
                   module->cart_writer.entry_index,
                   module->cart_writer.entry_count);
        close_os_file(ctx, module->cart_writer.slot);
        module->cart_writer.started = NUX_FALSE;
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_io_write_cart_data (nux_ctx_t      *ctx,
                        const nux_c8_t *path,
                        nux_u32_t       type,
                        nux_b32_t       compress,
                        const void     *data,
                        nux_u32_t       size)
{
    nux_io_module_t *module = ctx->io;

    NUX_ENSURE(module->cart_writer.started,
               return NUX_FAILURE,
               "cart writer not started");

    nux_status_t status = NUX_SUCCESS;

    nux_u32_t data_cursor = module->cart_writer.cursor;
    nux_u32_t entry_cursor
        = NUX_CART_HEADER_SIZE
          + NUX_CART_ENTRY_SIZE * module->cart_writer.entry_index;

    nux_u32_t path_length = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t path_hash   = nux_hash(path, path_length);
    nux_u32_t path_offset = module->cart_writer.cursor;
    nux_u32_t data_type   = type;
    nux_u32_t data_offset = path_offset + path_length + 1; // null terminated
    nux_u32_t data_length = size;

    ++module->cart_writer.entry_index;
    module->cart_writer.cursor += path_length + 1 + data_length;

    // Write path + data
    NUX_ENSURE(nux_os_file_seek(ctx, module->cart_writer.slot, path_offset),
               return NUX_FAILURE,
               "failed to seek to data section");
    status &= cart_write(ctx, module->cart_writer.slot, path, path_length);
    status &= cart_write(
        ctx, module->cart_writer.slot, "\0", 1); // null terminated
    status &= cart_write(ctx, module->cart_writer.slot, data, data_length);
    NUX_ENSURE(status,
               return NUX_FAILURE,
               "failed to write cart data entry %d",
               module->cart_writer.entry_index);

    // Write entry
    NUX_ENSURE(nux_os_file_seek(ctx, module->cart_writer.slot, entry_cursor),
               return NUX_FAILURE,
               "failed to seek entry");
    status &= cart_write_u32(ctx, module->cart_writer.slot, data_type);
    status &= cart_write_u32(ctx, module->cart_writer.slot, data_offset);
    status &= cart_write_u32(ctx, module->cart_writer.slot, data_length);
    status &= cart_write_u32(ctx, module->cart_writer.slot, path_hash);
    status &= cart_write_u32(ctx, module->cart_writer.slot, path_offset);
    status &= cart_write_u32(ctx, module->cart_writer.slot, path_length);
    NUX_CHECK(status, return NUX_FAILURE);

    return NUX_SUCCESS;
}
nux_status_t
nux_io_write_cart_file (nux_ctx_t *ctx, const nux_c8_t *path)
{
    nux_u32_t size;
    void     *data = nux_io_load(ctx, nux_arena_frame(ctx), path, &size);
    NUX_CHECK(data, return NUX_FAILURE);
    return nux_io_write_cart_data(ctx, path, 0, NUX_FALSE, data, size);
}
