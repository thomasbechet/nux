#ifndef GPU_H
#define GPU_H

#include "vm.h"

nu_status_t gpu_init(vm_t *vm);
nu_status_t gpu_free(vm_t *vm);

void gpu_write_texture(vm_t       *vm,
                       nu_u32_t    slot,
                       nu_u32_t    x,
                       nu_u32_t    y,
                       nu_u32_t    w,
                       nu_u32_t    h,
                       const void *p);
void gpu_write_vertex(vm_t *vm, nu_u32_t first, nu_u32_t count, const void *p);
void gpu_bind_texture(vm_t *vm, nu_u32_t slot);
void gpu_draw(vm_t *vm, nu_u32_t first, nu_u32_t count);
void gpu_render(vm_t *vm);

#endif
