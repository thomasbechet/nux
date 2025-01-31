#ifndef PLATFORM_H
#define PLATFORM_H

#include "vm.h"

void *os_malloc(vm_t *vm, nu_size_t n);

void os_gpu_init(vm_t *vm);
void os_gpu_init_texture(vm_t *vm, nu_u32_t index);
void os_gpu_free_texture(vm_t *vm, nu_u32_t index);
void os_gpu_update_texture(vm_t       *vm,
                           nu_u32_t    index,
                           nu_u32_t    x,
                           nu_u32_t    y,
                           nu_u32_t    w,
                           nu_u32_t    h,
                           const void *p);
void os_gpu_init_mesh(vm_t *vm, nu_u32_t index);
void os_gpu_free_mesh(vm_t *vm, nu_u32_t index);
void os_gpu_update_mesh(vm_t                  *vm,
                        nu_u32_t               index,
                        gpu_vertex_attribute_t attributes,
                        nu_u32_t               first,
                        nu_u32_t               count,
                        const void            *p);
void os_gpu_init_model(vm_t *vm, nu_u32_t index);
void os_gpu_free_model(vm_t *vm, nu_u32_t index);
void os_gpu_update_model(vm_t                   *vm,
                         nu_u32_t                index,
                         nu_u32_t                node_index,
                         const gpu_model_node_t *node);
void os_gpu_begin(vm_t *vm);
void os_gpu_end(vm_t *vm);
void os_gpu_push_transform(vm_t *vm, gpu_transform_t transform);
void os_gpu_draw_model(vm_t *vm, nu_u32_t index);

nu_status_t os_iou_mount(vm_t *vm, const nu_char_t *name);
nu_status_t os_iou_seek(vm_t *vm, nu_size_t n);
nu_size_t   os_iou_read(vm_t *vm, void *p, nu_size_t n);
void        os_iou_vlog(vm_t            *vm,
                        nu_log_level_t   level,
                        const nu_char_t *fmt,
                        va_list          args);
void        os_iou_update_controllers(vm_t *vm);

nu_status_t os_cpu_load_wasm(vm_t      *vm,
                             nu_byte_t *buffer,
                             nu_size_t  buffer_size);
nu_status_t os_cpu_call_event(vm_t *vm, cpu_event_t event);

#endif
