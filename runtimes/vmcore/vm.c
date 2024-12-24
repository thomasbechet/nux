#include "vm.h"

#include "gpu.h"
#include "wasm.h"
#include "platform.h"

#include <nulib.h>

typedef struct
{
    nux_gpu_t  gpu;
    nux_wasm_t wasm;
    void      *heap_ptr;
    nu_size_t  heap_size;
    void      *heap;
    void      *user;
    nu_u32_t   cart_id;
} nux_vm;

static void *
vm_malloc (nux_vm *vm, nu_size_t n)
{
    if ((nu_size_t)vm->heap_ptr > (nu_size_t)vm->heap + vm->heap_size)
    {
        return NU_NULL;
    }
    void *ptr    = vm->heap_ptr;
    vm->heap_ptr = (void *)((nu_size_t)vm->heap_ptr + n);
    return ptr;
}

static void
vm_load_cart (nux_vm *vm, const nu_byte_t *cart)
{
    vm->cart_id = os_cart_mount(vm->user, cart);
    os_cart_seek(vm->user, 0);
}

nux_vm_t
nux_vm_init (const nux_vm_info_t *info)
{
    NU_ASSERT(info->heap_size > sizeof(nux_vm));
    nux_vm *vm = info->heap;
    nu_memset(vm, 0, sizeof(*vm));
    vm->heap_ptr  = (void *)((nu_size_t)info->heap + sizeof(*vm));
    vm->heap      = info->heap;
    vm->heap_size = info->heap_size;

    nux_wasm_info_t wasm_info;
    nu_memset(&wasm_info, 0, sizeof(wasm_info));
    wasm_info.heap_size = NU_MEM_32K;
    wasm_info.heap      = vm_malloc(vm, wasm_info.heap_size);
    NU_ASSERT(wasm_info.heap);
    wasm_info.main_module      = NU_NULL;
    wasm_info.main_module_size = 0;
    // TODO: wasm module info
    nux_wasm_init(&vm->wasm, &wasm_info);

    nux_gpu_init(&vm->gpu);
    return (nux_vm_t)vm;
}
void
nux_vm_update (nux_vm_t vm)
{
}
