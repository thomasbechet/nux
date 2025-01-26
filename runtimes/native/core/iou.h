#ifndef IOU_H
#define IOU_H

#include "vm.h"

nu_status_t iou_init(vm_t *vm);
nu_status_t iou_load_cart(vm_t *vm, const nu_char_t *name);
nu_size_t   iou_read(vm_t *vm, void *p, nu_size_t n);
nu_status_t iou_read_u32(vm_t *vm, nu_u32_t *v);
nu_status_t iou_read_f32(vm_t *vm, nu_f32_t *v);
nu_status_t iou_read_m4(vm_t *vm, nu_m4_t *v);

void iou_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void iou_vlog(vm_t            *vm,
              nu_log_level_t   level,
              const nu_char_t *fmt,
              va_list          args);

#endif
