#include "internal.h"

nux_status_t
nux_io_init (void)
{
    nux_core_module_t *module = nux_core();
    nux_register_resource(NUX_RESOURCE_FILE,
                          (nux_resource_info_t) {
                              .name    = "file",
                              .size    = sizeof(nux_file_t),
                              .cleanup = nux_file_cleanup,
                          });
    nux_register_resource(NUX_RESOURCE_DISK,
                          (nux_resource_info_t) {
                              .name    = "disk",
                              .size    = sizeof(nux_disk_t),
                              .cleanup = nux_disk_cleanup,
                          });

    // Initialize files
    NUX_CHECK(nux_u32_vec_init_capa(
                  nux_core_arena(), NUX_FILE_MAX, &module->free_file_slots),
              return NUX_FAILURE);

    // Initialize values
    nux_u32_vec_fill_reversed(&module->free_file_slots);

    // Register base OS disk
    nux_disk_t *disk = nux_new_resource(nux_core_arena(), NUX_RESOURCE_DISK);
    disk->type       = NUX_DISK_OS;

    return NUX_SUCCESS;
}
void
nux_io_free (void)
{
    nux_core_module_t *module = nux_core();
    NUX_ASSERT(module->free_file_slots.size = NUX_FILE_MAX);
}

nux_status_t
nux_io_open_os_file (const nux_c8_t *path,
                     nux_io_mode_t   mode,
                     nux_u32_t      *ret_slot)
{
    nux_core_module_t *module = nux_core();
    nux_c8_t           normpath[NUX_PATH_BUF_SIZE];
    nux_u32_t          len = nux_path_normalize(normpath, path);
    NUX_ENSURE(!nux_path_isdir(normpath),
               return NUX_NULL,
               "trying to open a directory '%s' as file",
               normpath);
    nux_u32_t *slot = nux_u32_vec_pop(&module->free_file_slots);
    NUX_ENSURE(slot, return NUX_NULL, "out of os file slots");
    NUX_ENSURE(nux_os_open_file(*slot, normpath, len, mode),
               goto cleanup,
               "failed to open os file '%s'",
               normpath);
    *ret_slot = *slot;
    return NUX_SUCCESS;
cleanup:
    nux_u32_vec_pushv(&module->free_file_slots, *slot);
    return NUX_FAILURE;
}
void
nux_io_close_os_file (nux_u32_t slot)
{
    nux_core_module_t *module = nux_core();
    nux_os_close_file(slot);
    nux_u32_vec_pushv(&module->free_file_slots, slot);
}

static nux_status_t
cart_write (nux_file_t *file, const void *p, nux_u32_t n)
{
    NUX_ENSURE(nux_write_file(file, p, n),
               return NUX_FAILURE,
               "failed to write cart file");
    return NUX_SUCCESS;
}
static nux_status_t
cart_write_u32 (nux_file_t *file, nux_u32_t v)
{
    v = nux_u32_le(v);
    NUX_CHECK(cart_write(file, &v, sizeof(v)), return NUX_FAILURE);
    return NUX_SUCCESS;
}

nux_status_t
nux_io_cart_begin (const nux_c8_t *path, nux_u32_t entry_count)
{
    nux_core_module_t *module = nux_core();
    if (module->cart_writer.started)
    {
        NUX_CHECK(nux_io_cart_end(), return NUX_FAILURE);
    }

    nux_file_t *file
        = nux_open_file(nux_frame_arena(), path, NUX_IO_READ_WRITE);
    NUX_CHECK(file, return NUX_FAILURE);

    module->cart_writer.file        = nux_resource_rid(file);
    module->cart_writer.entry_count = entry_count;
    module->cart_writer.entry_index = 0;
    module->cart_writer.cursor
        = NUX_CART_HEADER_SIZE + NUX_CART_ENTRY_SIZE * entry_count;

    nux_status_t status = NUX_SUCCESS;
    status &= cart_write_u32(file, 0xCA);
    status &= cart_write_u32(file, 0xFE);
    status &= cart_write_u32(file, entry_count);
    NUX_CHECK(status, goto cleanup);

    module->cart_writer.started = NUX_TRUE;

    return NUX_SUCCESS;
cleanup:
    return NUX_FAILURE;
}
nux_status_t
nux_io_cart_end (void)
{
    nux_core_module_t *module = nux_core();
    if (module->cart_writer.started)
    {
        NUX_ENSURE(module->cart_writer.entry_index
                       == module->cart_writer.entry_index,
                   return NUX_FAILURE,
                   "missing cart entries (got %d, expect %d)",
                   module->cart_writer.entry_index,
                   module->cart_writer.entry_count);
        module->cart_writer.started = NUX_FALSE;
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_io_write_cart_data (const nux_c8_t *path,
                        nux_u32_t       type,
                        nux_b32_t       compress,
                        const void     *data,
                        nux_u32_t       size)
{
    nux_core_module_t *module = nux_core();
    NUX_ENSURE(module->cart_writer.started,
               return NUX_FAILURE,
               "cart writer not started");

    nux_file_t *file
        = nux_check_resource(NUX_RESOURCE_FILE, module->cart_writer.file);
    NUX_CHECK(file, return NUX_FAILURE);

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
    NUX_ENSURE(nux_seek_file(file, path_offset),
               return NUX_FAILURE,
               "failed to seek to data section");
    status &= cart_write(file, path, path_length);
    status &= cart_write(file, "\0", 1); // null terminated
    status &= cart_write(file, data, data_length);
    NUX_ENSURE(status,
               return NUX_FAILURE,
               "failed to write cart data entry %d",
               module->cart_writer.entry_index);

    // Write entry
    NUX_ENSURE(nux_seek_file(file, entry_cursor),
               return NUX_FAILURE,
               "failed to seek entry");
    status &= cart_write_u32(file, data_type);
    status &= cart_write_u32(file, data_offset);
    status &= cart_write_u32(file, data_length);
    status &= cart_write_u32(file, path_hash);
    status &= cart_write_u32(file, path_offset);
    status &= cart_write_u32(file, path_length);
    NUX_CHECK(status, return NUX_FAILURE);

    return NUX_SUCCESS;
}
nux_status_t
nux_io_write_cart_file (const nux_c8_t *path)
{
    nux_u32_t size;
    void     *data = nux_load_file(nux_frame_arena(), path, &size);
    NUX_CHECK(data, return NUX_FAILURE);
    return nux_io_write_cart_data(path, 0, NUX_FALSE, data, size);
}
