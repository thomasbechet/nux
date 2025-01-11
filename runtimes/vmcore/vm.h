#ifndef VM_H
#define VM_H

#include "config.h"

typedef struct vm vm_t;

typedef struct vm_info
{
    void              *heap;
    nu_size_t          heap_size;
    void              *user;
    const vm_config_t *specs;
} vm_info_t;

NU_API vm_t *vm_init(const vm_info_t *info);
NU_API void  vm_load(vm_t *vm, const nu_char_t *name);
NU_API void  vm_update(vm_t *vm);

void *vm_malloc(vm_t *vm, nu_size_t n);

#endif
