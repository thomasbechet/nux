#ifndef GPU_H
#define GPU_H

#include "vm.h"

nu_status_t gpu_init(vm_t *vm, const vm_config_t *config);
nu_status_t gpu_free(vm_t *vm);
void        gpu_begin(vm_t *vm);
void        gpu_end(vm_t *vm);

nu_status_t gpu_load_texture(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t gpu_load_mesh(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t gpu_load_model(vm_t *vm, const cart_chunk_header_t *header);

nu_status_t gpu_alloc_pool(vm_t *vm, nu_u32_t index, nu_u32_t size);
nu_status_t gpu_bind_pool(vm_t *vm, nu_u32_t index);
nu_status_t gpu_clear_pool(vm_t *vm, nu_u32_t index);

nu_status_t gpu_alloc_texture(vm_t              *vm,
                              nu_u32_t           index,
                              gpu_texture_size_t size);
nu_status_t gpu_update_texture(vm_t       *vm,
                               nu_u32_t    index,
                               nu_u32_t    x,
                               nu_u32_t    y,
                               nu_u32_t    w,
                               nu_u32_t    h,
                               const void *p);

nu_status_t gpu_alloc_mesh(vm_t                  *vm,
                           nu_u32_t               index,
                           nu_u32_t               count,
                           gpu_primitive_t        primitive,
                           gpu_vertex_attribute_t attributes);
nu_status_t gpu_update_mesh(vm_t                  *vm,
                            nu_u32_t               index,
                            gpu_vertex_attribute_t attributes,
                            nu_u32_t               first,
                            nu_u32_t               count,
                            const void            *p);

nu_status_t gpu_set_model_mesh(vm_t *vm, nu_u32_t index, nu_u32_t mesh);
nu_status_t gpu_set_model_texture(vm_t *vm, nu_u32_t index, nu_u32_t texture);
nu_status_t gpu_set_model_transform(vm_t           *vm,
                                    nu_u32_t        index,
                                    const nu_f32_t *m);
nu_status_t gpu_set_model_parent(vm_t *vm, nu_u32_t index, nu_u32_t parent);

void gpu_push_transform(vm_t *vm, gpu_transform_t transform, const nu_f32_t *m);
void gpu_draw_model(vm_t *vm, nu_u32_t index);

nu_u32_t gpu_vertex_size(gpu_vertex_attribute_t attributes);
nu_u32_t gpu_vertex_offset(gpu_vertex_attribute_t attributes,
                           gpu_vertex_attribute_t attribute);
nu_u32_t gpu_texture_memsize(gpu_texture_size_t size);
nu_u32_t gpu_texture_width(gpu_texture_size_t size);

#endif
