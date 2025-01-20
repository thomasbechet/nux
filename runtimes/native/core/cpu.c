#include "vm.h"

#include "platform.h"

nu_status_t
cpu_init (vm_t *vm)
{
    return NU_SUCCESS;
}
void
cpu_free (vm_t *vm)
{
}
nu_status_t
cpu_load (vm_t *vm, const cart_chunk_header_t *header)
{
    // Load module data
    NU_ASSERT(header->length);
    nu_byte_t *buffer = os_malloc(vm, header->length);
    NU_ASSERT(buffer);
    NU_ASSERT(os_iop_read(vm, buffer, header->length));
    NU_ASSERT(os_cpu_load_wasm(vm, buffer, header->length));
    return NU_SUCCESS;
}
nu_status_t
cpu_update (vm_t *vm)
{
    NU_ASSERT(os_cpu_update_wasm(vm));
    return NU_SUCCESS;
}
