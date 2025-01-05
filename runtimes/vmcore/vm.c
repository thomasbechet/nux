#include "vm.h"

#include "wasm.h"
#include "io.h"

#include <nulib.h>

nux_vm_t *
nux_vm_init (const nux_vm_info_t *info)
{
    NU_ASSERT(info->heap_size > sizeof(nux_vm_t));
    nux_vm_t *vm = info->heap;
    nu_memset(vm, 0, sizeof(*vm));
    vm->heap_ptr  = (void *)((nu_size_t)info->heap + sizeof(*vm));
    vm->heap      = info->heap;
    vm->heap_size = info->heap_size;
    vm->config    = *info->specs;

    nux_io_init(vm);
    nux_wasm_init(vm);

    return vm;
}
void
nux_vm_load (nux_vm_t *vm, const nu_byte_t *name)
{
    nux_cart_load_full(vm, name);
}
void
nux_vm_update (nux_vm_t *vm)
{
    nux_wasm_update(vm);
}

void *
vm_malloc (nux_vm_t *vm, nu_size_t n)
{
    printf("allocate %lu\n", n);
    if ((nu_size_t)vm->heap_ptr > (nu_size_t)vm->heap + vm->heap_size)
    {
        fprintf(stderr, "out of memory\n");
        return NU_NULL;
    }
    void *ptr    = vm->heap_ptr;
    vm->heap_ptr = (void *)((nu_size_t)vm->heap_ptr + n);
    return ptr;
}
