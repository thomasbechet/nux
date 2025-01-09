#ifndef NUX_IO_H
#define NUX_IO_H

#include "types.h"

#define NUX_IO_MEM_SIZE NU_MEM_64K

void nux_io_init(nux_vm_t *vm);
void nux_cart_load_full(nux_vm_t *vm, const nu_char_t *name);

#endif
