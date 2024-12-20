#ifndef NU_VMCORE_H
#define NU_VMCORE_H

#include "common.h"

NU_API void nu_vm_init(const nu_byte_t *buffer, nu_size_t size);
NU_API void nu_vm_update(void);

#endif
