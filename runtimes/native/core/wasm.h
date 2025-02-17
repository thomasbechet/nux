#ifndef CPU_H
#define CPU_H

#include "syscall.h"

typedef enum
{
    WASM_EVENT_START,
    WASM_EVENT_UPDATE,
} wasm_event_t;

typedef struct
{
    nu_bool_t loaded;
} wasm_t;

nu_status_t wasm_init(vm_t *vm);
void        wasm_free(vm_t *vm);
nu_status_t wasm_call_event(vm_t *vm, wasm_event_t event);

#endif
