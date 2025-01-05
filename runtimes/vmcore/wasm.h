#ifndef NU_WASM_H
#define NU_WASM_H

#include "types.h"

void nux_wasm_init(nux_vm_t *vm);
void nux_wasm_load(nux_vm_t *vm, const nux_chunk_header_t *header);
void nux_wasm_update(nux_vm_t *vm);

#endif
