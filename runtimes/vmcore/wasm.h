#ifndef NU_WASM_H
#define NU_WASM_H

#include "types.h"

void nux_wasm_init(nux_wasm_t *wasm, const nux_wasm_info_t *info);
void nux_wasm_load(nux_vm_t *vm, const nux_chunk_header_t *header);

#endif
