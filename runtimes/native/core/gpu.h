#ifndef GPU_H
#define GPU_H

#include "vm.h"

nu_status_t gpu_init(vm_t *vm, const vm_config_t *config);
nu_status_t gpu_free(vm_t *vm);
void        gpu_begin(vm_t *vm);
void        gpu_end(vm_t *vm);

void gpu_alloc_texture(vm_t              *vm,
                       nu_u32_t           index,
                       gpu_texture_size_t size,
                       const void        *p);
void gpu_write_texture(vm_t       *vm,
                       nu_u32_t    index,
                       nu_u32_t    x,
                       nu_u32_t    y,
                       nu_u32_t    w,
                       nu_u32_t    h,
                       const void *p);

void gpu_alloc_mesh(vm_t                  *vm,
                    nu_u32_t               index,
                    nu_u32_t               count,
                    gpu_primitive_t        primitive,
                    gpu_vertex_attribute_t attributes,
                    const void            *p);
void gpu_write_mesh(vm_t                  *vm,
                    nu_u32_t               index,
                    gpu_vertex_attribute_t attributes,
                    nu_u32_t               first,
                    nu_u32_t               count,
                    const void            *p);

void gpu_set_transform(vm_t *vm, gpu_transform_t transform, const nu_f32_t *m);
void gpu_set_texture(vm_t *vm, nu_u32_t index);
void gpu_draw_mesh(vm_t *vm, nu_u32_t mesh);
void gpu_draw_submesh(vm_t *vm, nu_u32_t mesh, nu_u32_t first, nu_u32_t count);

#endif
