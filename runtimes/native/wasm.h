#ifndef WASM_H
#define WASM_H

#include "core/vm.h"

nu_status_t wasm_init(void);
void        wasm_free(void);
nu_status_t wasm_load(const cart_chunk_header_t *header);

#endif
