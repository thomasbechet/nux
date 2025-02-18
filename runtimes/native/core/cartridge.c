#include "cartridge.h"

#include "vm.h"
#include "platform.h"

nu_size_t
cart_read (vm_t *vm, void *p, nu_size_t n)
{
    return os_cart_read(vm, p, n);
}
nu_status_t
cart_read_u32 (vm_t *vm, nu_u32_t *v)
{
    NU_CHECK(os_cart_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_u32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
cart_read_f32 (vm_t *vm, nu_f32_t *v)
{
    NU_CHECK(os_cart_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_f32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
cart_read_m4 (vm_t *vm, nu_m4_t *v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(vm, &v->data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}

nu_status_t
cart_parse_header (const void *data, cart_header_t *header)
{
    const nu_byte_t *ptr = data;
    header->version      = nu_u32_le(*((nu_u32_t *)ptr));
    ptr += sizeof(nu_u32_t);
    header->chunk_count = nu_u32_le(*((nu_u32_t *)ptr));
    // TODO: validate
    return NU_SUCCESS;
}
nu_status_t
cart_parse_entries (const void         *data,
                    nu_u32_t            count,
                    cart_chunk_entry_t *entries)
{
    const nu_byte_t *ptr = data;
    for (nu_size_t i = 0; i < count; ++i)
    {
        // Type
        cart_chunk_entry_t *entry = entries + i;
        entry->type               = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Name
        nu_memcpy(entry->name, ptr, CART_CHUNK_NAME_MAX);
        if (entry->name[CART_CHUNK_NAME_MAX - 1] != '\0')
        {
            return NU_FAILURE;
        }
        ptr += CART_CHUNK_NAME_MAX;
        // Offset
        entry->offset = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Length
        entry->length = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
    }
    return NU_SUCCESS;
}
