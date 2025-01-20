#ifndef CPU_H
#define CPU_H

#include "vm.h"

nu_status_t cpu_init(vm_t *vm);
void        cpu_free(vm_t *vm);
nu_status_t cpu_load(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t cpu_update(vm_t *vm);

#endif
