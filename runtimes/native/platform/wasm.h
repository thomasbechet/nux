#ifndef VM_WASM_H
#define VM_WASM_H

#include "../vm.h"

nu_status_t wasm_init(void);
void        wasm_free(void);
nu_status_t wasm_load(const vm_chunk_header_t *header);

#endif
