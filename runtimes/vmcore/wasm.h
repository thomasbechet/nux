#ifndef VM_WASM_H
#define VM_WASM_H

#include "types.h"

nu_status_t vm_wasm_init(vm_t *vm);
void        vm_wasm_free(vm_t *vm);
nu_status_t vm_wasm_load(vm_t *vm, const vm_chunk_header_t *header);
nu_status_t vm_wasm_update(vm_t *vm);

#endif
