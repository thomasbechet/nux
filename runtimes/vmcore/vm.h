#ifndef NUX_VM_H
#define NUX_VM_H

#include "config.h"

typedef struct nux_vm_t nux_vm_t;

typedef struct nux_vm_info_t
{
    void                  *heap;
    nu_size_t              heap_size;
    void                  *user;
    const nux_vm_config_t *specs;
} nux_vm_info_t;

NU_API nux_vm_t *nux_vm_init(const nux_vm_info_t *info);
NU_API void      nux_vm_load(nux_vm_t *vm, const nu_byte_t *name);
NU_API void      nux_vm_update(nux_vm_t *vm);

void *vm_malloc(nux_vm_t *vm, nu_size_t n);

#endif
