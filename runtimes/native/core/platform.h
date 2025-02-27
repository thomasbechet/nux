#ifndef PLATFORM_H
#define PLATFORM_H

#include "vm.h"

void *os_malloc(vm_t *vm, nu_size_t n);
void  os_vlog(vm_t            *vm,
              nu_log_level_t   level,
              const nu_char_t *fmt,
              va_list          args);

void os_gpu_init_texture(vm_t *vm, nu_u32_t id);
void os_gpu_free_texture(vm_t *vm, nu_u32_t id);
void os_gpu_update_texture(vm_t *vm, nu_u32_t id);
void os_gpu_init_mesh(vm_t *vm, nu_u32_t id);
void os_gpu_free_mesh(vm_t *vm, nu_u32_t id);
void os_gpu_update_mesh(vm_t *vm, nu_u32_t id);
void os_gpu_init_model(vm_t *vm, nu_u32_t id);
void os_gpu_free_model(vm_t *vm, nu_u32_t id);
void os_gpu_update_model(vm_t                   *vm,
                         nu_u32_t                id,
                         nu_u32_t                node_id,
                         const gfx_model_node_t *node);

void os_gpu_begin_frame(vm_t *vm);
void os_gpu_end_frame(vm_t *vm);
void os_gpu_set_render_state(vm_t *vm, sys_render_state_t state);
void os_gpu_clear(vm_t *vm, nu_u32_t color);
void os_gpu_draw_model(vm_t *vm, nu_u32_t id);
void os_gpu_draw_volume(vm_t *vm, const nu_f32_t *center, const nu_f32_t *size);
void os_gpu_draw_cube(vm_t *vm, const nu_f32_t *center, const nu_f32_t *size);
void os_gpu_draw_lines(vm_t           *vm,
                       const nu_f32_t *points,
                       nu_u32_t        count,
                       nu_bool_t       linestrip);
void os_gpu_draw_text(vm_t *vm, const void *text, nu_u32_t len);
void os_gpu_draw_blit(
    vm_t *vm, nu_u32_t id, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);

nu_status_t os_cart_mount(vm_t *vm, const nu_char_t *name);
nu_status_t os_cart_seek(vm_t *vm, nu_size_t n);
nu_size_t   os_cart_read(vm_t *vm, void *p, nu_size_t n);
void        os_gpad_update(vm_t *vm);

nu_status_t os_cpu_load_wasm(vm_t      *vm,
                             nu_byte_t *buffer,
                             nu_size_t  buffer_size);
nu_status_t os_cpu_call_event(vm_t *vm, wasm_event_t event);

#endif
