#ifndef VM_H
#define VM_H

#include "gpu.h"
#include "cpu.h"
#include "gamepad.h"

#define VM_VERSION_MAJOR(num)          ((num >> 16) & 0xFF)
#define VM_VERSION_MINOR(num)          ((num >> 8) & 0xFF)
#define VM_VERSION_PATCH(num)          (num & 0xFF)
#define VM_VERSION_MAKE(maj, min, pat) (maj << 16 | min << 8 | pat)
#define VM_VERSION                     VM_VERSION_MAKE(0, 0, 1)

typedef struct
{
    cpu_config_t cpu;
    gpu_config_t gpu;
} vm_config_t;

struct vm
{
    cpu_t     cpu;
    gpu_t     gpu;
    gamepad_t gamepad;
    nu_bool_t running;
};

NU_API nu_status_t vm_init(vm_t *vm, const vm_config_t *config);
NU_API void        vm_free(vm_t *vm);
NU_API nu_status_t vm_load(vm_t *vm, const nu_char_t *name);
NU_API nu_status_t vm_tick(vm_t *vm);
NU_API void        vm_save_state(const vm_t *vm, void *state);
NU_API nu_status_t vm_load_state(vm_t *vm, const void *state);

NU_API void      vm_config_default(vm_config_t *config);
NU_API nu_size_t vm_config_state_memsize(const vm_config_t *config);

void vm_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void vm_vlog(vm_t            *vm,
             nu_log_level_t   level,
             const nu_char_t *fmt,
             va_list          args);

#endif
