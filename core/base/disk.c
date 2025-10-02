#include "internal.h"

static nux_status_t
cart_read (nux_file_t *file, void *p, nux_u32_t n)
{
    NUX_ENSURE(nux_file_read(file, p, n) == n,
               return NUX_FAILURE,
               "failed to read cart file");
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_u32 (nux_file_t *file, nux_u32_t *v)
{
    NUX_CHECK(cart_read(file, v, sizeof(*v)), return NUX_FAILURE);
    *v = nux_u32_le(*v);
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_entries (nux_cart_t *cart)
{
    nux_u32_t    id, version, entry_count;
    nux_status_t status = NUX_SUCCESS;
    status &= cart_read_u32(cart->file, &id);
    status &= cart_read_u32(cart->file, &version);
    status &= cart_read_u32(cart->file, &cart->entries_count);
    NUX_CHECK(status, return NUX_FAILURE);
    cart->entries = nux_arena_malloc(
        nux_arena_core(), sizeof(*cart->entries) * cart->entries_count);
    NUX_CHECK(cart->entries, return NUX_FAILURE);
    for (nux_u32_t i = 0; i < cart->entries_count; ++i)
    {
        nux_cart_entry_t *entry = cart->entries + i;
        status                  = NUX_SUCCESS;
        status &= cart_read_u32(cart->file, &entry->data_type);
        status &= cart_read_u32(cart->file, &entry->data_offset);
        status &= cart_read_u32(cart->file, &entry->data_length);
        status &= cart_read_u32(cart->file, &entry->path_hash);
        status &= cart_read_u32(cart->file, &entry->path_offset);
        status &= cart_read_u32(cart->file, &entry->path_length);
        NUX_CHECK(status, return NUX_FAILURE);
    }

    {
        for (nux_u32_t i = 0; i < cart->entries_count; ++i)
        {
            nux_cart_entry_t *entry = cart->entries + i;
            nux_file_seek(cart->file, entry->path_offset);
            nux_c8_t path[NUX_PATH_BUF_SIZE];
            nux_file_read(cart->file, path, entry->path_length + 1);
            NUX_DEBUG("[%d] 0x%08X %s", i, entry->path_hash, path);
        }
    }

    return NUX_SUCCESS;
}

void
nux_disk_cleanup (void *data)
{
    nux_disk_t *disk = data;
}

nux_status_t
nux_disk_mount (const nux_c8_t *path)
{
    nux_base_module_t *module = nux_base_module();

    NUX_DEBUG("mounting '%s", path);
    nux_disk_t *disk = nux_resource_new(nux_arena_core(), NUX_RESOURCE_DISK);
    NUX_CHECK(disk, return NUX_FAILURE);

    disk->type      = NUX_DISK_CART;
    disk->cart.path = nux_arena_alloc_string(nux_arena_core(), path);

    // Open file
    nux_u32_t file_slot;
    disk->cart.file = nux_file_open(nux_arena_core(), path, NUX_IO_READ);
    NUX_CHECK(disk->cart.file, return NUX_FAILURE);

    // Read entries
    NUX_CHECK(cart_read_entries(&disk->cart), return NUX_FAILURE);

    if (module->first_disk)
    {
        module->first_disk->next = disk;
        disk->prev               = module->first_disk;
    }
    else
    {
        module->first_disk = disk;
    }

    return NUX_SUCCESS;
}
