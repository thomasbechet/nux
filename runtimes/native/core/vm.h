#ifndef VM_H
#define VM_H

#include "gpu.h"
#include "cpu.h"

/////////////////
///    VM     ///
/////////////////

#ifdef NUX_BUILD_SDK
#define VM_RUNTIME_VERSION "0.0.1 (with sdk tools)"
#else
#define VM_RUNTIME_VERSION "0.0.1 (no sdk tools)"
#endif

#define VM_CONFIG_DEFAULT                                                 \
    (vm_config_t)                                                         \
    {                                                                     \
        .cpu.mem_heap_size = NU_MEM_1M, .cpu.mem_stack_size = NU_MEM_64K, \
        .gpu.vram_capacity = NU_MEM_8M,                                   \
    }

typedef struct
{
    cpu_config_t cpu;
    gpu_config_t gpu;
} vm_config_t;

struct vm
{
    cpu_t     cpu;
    gpu_t     gpu;
    iou_t     iou;
    nu_bool_t running;
};

NU_API nu_status_t vm_init(vm_t *vm, const vm_config_t *config);
NU_API void        vm_free(vm_t *vm);
NU_API nu_status_t vm_load(vm_t *vm, const nu_char_t *name);
NU_API nu_status_t vm_tick(vm_t *vm);

#endif
