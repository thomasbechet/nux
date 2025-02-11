#ifndef CPU_H
#define CPU_H

#include "syscall.h"

typedef enum
{
    CPU_EVENT_START,
    CPU_EVENT_UPDATE,
} cpu_event_t;

typedef struct
{
    nu_u32_t ram_capacity;
} cpu_config_t;

typedef struct
{
    nu_byte_t *ram;
    nu_u32_t   ram_capa;
    nu_u32_t   ram_size;
    nu_bool_t  loaded;
} cpu_t;

nu_status_t cpu_init(vm_t *vm, const cpu_config_t *config);
void        cpu_free(vm_t *vm);
nu_status_t cpu_call_event(vm_t *vm, cpu_event_t event);
void       *cpu_malloc(vm_t *vm, nu_u32_t n);

#endif
