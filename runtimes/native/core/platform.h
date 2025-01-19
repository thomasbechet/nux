#ifndef VM_PLATFORM_H
#define VM_PLATFORM_H

#include "vm.h"

void *os_malloc(vm_t *vm, nu_size_t n);

void os_gpu_swap_buffer(vm_t *vm);
void os_gpu_write_texture(vm_t       *vm,
                          nu_u32_t    slot,
                          nu_u32_t    x,
                          nu_u32_t    y,
                          nu_u32_t    w,
                          nu_u32_t    h,
                          const void *p);
void os_gpu_write_vertex(vm_t       *vm,
                         nu_u32_t    first,
                         nu_u32_t    count,
                         const void *p);
void os_gpu_bind_texture(vm_t *vm, nu_u32_t slot);
void os_gpu_draw(vm_t *vm, nu_u32_t first, nu_u32_t count);

void      os_iop_mount(vm_t *vm, const nu_char_t *name);
void      os_iop_seek(vm_t *vm, nu_size_t n);
nu_size_t os_iop_read(vm_t *vm, void *p, nu_size_t n);
void      os_iop_vlog(vm_t            *vm,
                      nu_log_level_t   level,
                      const nu_char_t *fmt,
                      va_list          args);

nu_status_t os_cpu_load_wasm(vm_t      *vm,
                             nu_byte_t *buffer,
                             nu_size_t  buffer_size);
nu_status_t os_cpu_update_wasm(vm_t *vm);

#endif
