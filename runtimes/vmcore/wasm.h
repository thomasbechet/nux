#ifndef VM_WASM_H
#define VM_WASM_H

#include "types.h"

void vm_wasm_init(vm_t *vm);
void vm_wasm_free(vm_t *vm);
void vm_wasm_load(vm_t *vm, const vm_chunk_header_t *header);
void vm_wasm_update(vm_t *vm);

#endif
