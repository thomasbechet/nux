#include "internal.h"

static nux_status_t
cart_write (nux_file_t *file, const void *p, nux_u32_t n)
{
    NUX_ENSURE(nux_file_write(file, p, n),
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
    nux_base_module_t *module = nux_base_module();

    if (module->cart_writer.started)
    {
        NUX_CHECK(nux_io_cart_end(), return NUX_FAILURE);
    }

    nux_file_t *file
        = nux_file_open(nux_arena_frame(), path, NUX_IO_READ_WRITE);
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
    nux_base_module_t *module = nux_base_module();

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
    nux_base_module_t *module = nux_base_module();

    NUX_ENSURE(module->cart_writer.started,
               return NUX_FAILURE,
               "cart writer not started");

    nux_file_t *file
        = nux_resource_check(NUX_RESOURCE_FILE, module->cart_writer.file);
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
    NUX_ENSURE(nux_file_seek(file, path_offset),
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
    NUX_ENSURE(nux_file_seek(file, entry_cursor),
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
    void     *data = nux_file_load(nux_arena_frame(), path, &size);
    NUX_CHECK(data, return NUX_FAILURE);
    return nux_io_write_cart_data(path, 0, NUX_FALSE, data, size);
}
