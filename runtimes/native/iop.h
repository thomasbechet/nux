#ifndef IOP_H
#define IOP_H

#include "vm.h"

nu_status_t iop_init(vm_t *vm);
nu_status_t iop_load_full(vm_t *vm, const nu_char_t *name);
void iop_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void iop_vlog(vm_t            *vm,
              nu_log_level_t   level,
              const nu_char_t *fmt,
              va_list          args);

#endif
