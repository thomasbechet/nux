#include "internal.h"

static nux_status_t
open_file (nux_file_t *file, const nux_c8_t *path, nux_io_mode_t mode)
{
    nux_disk_t *disk = NUX_NULL;
    while ((disk = nux_resource_next(NUX_RESOURCE_DISK, disk)))
    {
        if (disk->type == NUX_DISK_OS)
        {
            nux_u32_t slot;
            nux_error_disable();
            nux_status_t status = nux_io_open_os_file(path, mode, &slot);
            nux_error_enable();
            if (status)
            {
                file->type    = NUX_DISK_OS;
                file->is_open = NUX_TRUE;
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
                    file->is_open     = NUX_TRUE;
                    file->mode        = mode;
                    file->cart.slot   = disk->cart.file->cart.slot;
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
static nux_status_t
close_file (nux_file_t *file)
{
    NUX_CHECK(file->is_open, return NUX_SUCCESS);
    if (file->type == NUX_DISK_OS)
    {
        nux_io_close_os_file(file->os.slot);
    }
    file->is_open = NUX_FALSE;
    return NUX_SUCCESS;
}

nux_file_t *
nux_file_open (nux_arena_t *arena, const nux_c8_t *path, nux_io_mode_t mode)
{
    nux_file_t *file = nux_resource_new(arena, NUX_RESOURCE_FILE);
    NUX_CHECK(file, return NUX_NULL);
    NUX_ENSURE(open_file(file, path, mode),
               return NUX_NULL,
               "file not found '%s'",
               path);
    return file;
}
void
nux_file_close (nux_file_t *file)
{
    close_file(file);
}
nux_u32_t
nux_file_read (nux_file_t *file, void *data, nux_u32_t n)
{
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_read(nux_userdata(), file->os.slot, data, n);
    }
    else if (file->type == NUX_DISK_CART)
    {
        nux_u32_t read = NUX_MIN(file->cart.length - file->cart.cursor, n);
        if (!read)
        {
            return 0;
        }
        nux_os_file_seek(nux_userdata(),
                         file->cart.slot,
                         file->cart.offset + file->cart.cursor);
        nux_u32_t got
            = nux_os_file_read(nux_userdata(), file->cart.slot, data, read);
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
nux_file_write (nux_file_t *file, const void *data, nux_u32_t n)
{
    void *userdata = nux_userdata();
    NUX_ENSURE(
        file->mode != NUX_IO_READ, return 0, "failed to write read only file");
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_write(userdata, file->os.slot, data, n);
    }
    else if (file->type == NUX_DISK_CART)
    {
        nux_u32_t write = NUX_MIN(file->cart.length - file->cart.cursor, n);
        nux_os_file_seek(
            userdata, file->cart.slot, file->cart.offset + file->cart.cursor);
        nux_u32_t got
            = nux_os_file_write(userdata, file->cart.slot, data, write);
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
nux_file_seek (nux_file_t *file, nux_u32_t cursor)
{
    void *userdata = nux_userdata();
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_seek(userdata, file->os.slot, cursor);
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
nux_file_stat (nux_file_t *file, nux_file_stat_t *stat)
{
    void *userdata = nux_userdata();
    if (file->type == NUX_DISK_OS)
    {
        return nux_os_file_stat(userdata, file->os.slot, stat);
    }
    else if (file->type == NUX_DISK_CART)
    {
        stat->size = file->cart.length;
        return NUX_SUCCESS;
    }
    return NUX_FAILURE;
}
void *
nux_file_load (nux_allocator_t *a, const nux_c8_t *path, nux_u32_t *size)
{
    void       *data = NUX_NULL;
    nux_file_t *file = nux_file_open(nux_arena_frame(), path, NUX_IO_READ);
    NUX_CHECK(file, return NUX_NULL);

    // Get the file size
    nux_file_stat_t stat;
    NUX_CHECK(nux_file_stat(file, &stat), goto cleanup0);

    // Seek to beginning
    NUX_CHECK(nux_file_seek(file, 0), goto cleanup0);

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
    data = nux_malloc(a, stat.size);
    NUX_CHECK(data, goto cleanup0);

    // Read file
    nux_u32_t read = nux_file_read(file, data, stat.size);
    NUX_ENSURE(
        read == stat.size,
        goto cleanup0,
        "failed to read file content '%s' (expect %d bytes, got %d bytes)",
        path,
        stat.size,
        read);

cleanup0:
    return data;
}
nux_b32_t
nux_file_exists (const nux_c8_t *path)
{
    nux_file_t   file;
    nux_status_t status = open_file(&file, path, NUX_IO_READ);
    if (status)
    {
        close_file(&file);
    }
    nux_error_reset(); // ignore error if any
    return status ? NUX_TRUE : NUX_FALSE;
}

void
nux_file_cleanup (void *data)
{
    nux_file_t *file = data;
    close_file(file);
}
