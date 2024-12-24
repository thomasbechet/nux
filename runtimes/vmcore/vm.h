#ifndef NUX_VM_H
#define NUX_VM_H

#include <nulib.h>

typedef struct
{
    void     *heap;
    nu_size_t heap_size;
    void     *user;
} nux_vm_info_t;

typedef struct nux_vm *nux_vm_t;

NU_API nux_vm_t nux_vm_init(const nux_vm_info_t *info);
NU_API void     nux_vm_update(nux_vm_t vm);
NU_API void     nux_vm_load(nux_vm_t vm, const nu_byte_t *buf, nu_size_t size);
NU_API void     nux_vm_hotreload(nux_vm_t vm);

#endif
