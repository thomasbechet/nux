#ifndef CPU_H
#define CPU_H

#include "shared.h"
#include "iou.h"

/////////////////
///    CPU    ///
/////////////////

typedef enum
{
    CPU_EVENT_START,
    CPU_EVENT_UPDATE,
} cpu_event_t;

typedef struct
{
    nu_u32_t mem_heap_size;
    nu_u32_t mem_stack_size;
} cpu_config_t;

typedef struct
{
    cpu_config_t config;
    void        *buffer;
    nu_size_t    buffer_size;
} cpu_t;

nu_status_t cpu_init(vm_t *vm, const cpu_config_t *config);
void        cpu_free(vm_t *vm);
nu_status_t cpu_load_wasm(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t cpu_call_event(vm_t *vm, cpu_event_t event);

#endif
