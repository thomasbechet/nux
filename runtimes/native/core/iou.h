#ifndef IOU_H
#define IOU_H

#include "vm.h"

nu_status_t iou_init(vm_t *vm);
nu_status_t iou_load_full(vm_t *vm, const nu_char_t *name);
void iou_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void iou_vlog(vm_t            *vm,
              nu_log_level_t   level,
              const nu_char_t *fmt,
              va_list          args);

#endif
