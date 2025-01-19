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
cpu_load (vm_t *vm, const vm_chunk_header_t *header)
{
    // Load module data
    NU_ASSERT(header->length);
    nu_byte_t *buffer = vm_malloc(vm, header->length);
    NU_ASSERT(buffer);
    NU_ASSERT(os_read(vm->user, buffer, header->length));
    NU_ASSERT(os_load_wasm(vm->user, buffer, header->length));
    return NU_SUCCESS;
}
nu_status_t
cpu_update (vm_t *vm)
{
    NU_ASSERT(os_update_wasm(vm->user));
    return NU_SUCCESS;
}
