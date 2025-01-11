#include "vm.h"

#include "wasm.h"
#include "io.h"

#include <nulib.h>

vm_t *
vm_init (const vm_info_t *info)
{
    NU_ASSERT(info->heap_size > sizeof(vm_t));
    vm_t *vm = info->heap;
    nu_memset(vm, 0, sizeof(*vm));
    vm->heap_ptr  = (void *)((nu_size_t)info->heap + sizeof(*vm));
    vm->heap      = info->heap;
    vm->heap_size = info->heap_size;
    vm->config    = *info->specs;

    vm_io_init(vm);
    vm_wasm_init(vm);

    return vm;
}
void
vm_load (vm_t *vm, const nu_char_t *name)
{
    vm_cart_load_full(vm, name);
}
void
vm_update (vm_t *vm)
{
    vm_wasm_update(vm);
}

void *
vm_malloc (vm_t *vm, nu_size_t n)
{
    if ((nu_size_t)vm->heap_ptr > (nu_size_t)vm->heap + vm->heap_size)
    {
        fprintf(stderr, "out of memory\n");
        return NU_NULL;
    }
    void *ptr    = vm->heap_ptr;
    vm->heap_ptr = (void *)((nu_size_t)vm->heap_ptr + n);
    return ptr;
}
