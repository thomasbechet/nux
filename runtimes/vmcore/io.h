#ifndef VM_IO_H
#define VM_IO_H

#include "types.h"

#define VM_IO_MEM_SIZE NU_MEM_1M

nu_status_t vm_io_init(vm_t *vm);
nu_status_t vm_cart_load_full(vm_t *vm, const nu_char_t *name);

#endif
