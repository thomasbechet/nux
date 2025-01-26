#ifndef CPU_H
#define CPU_H

#include "vm.h"

nu_status_t cpu_init(vm_t *vm, const vm_config_t *config);
void        cpu_free(vm_t *vm);
nu_status_t cpu_load_wasm(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t cpu_call_event(vm_t *vm, cpu_event_t event);

#endif
