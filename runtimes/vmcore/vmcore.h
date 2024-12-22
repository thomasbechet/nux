#ifndef NU_VMCORE_H
#define NU_VMCORE_H

#include <nulib.h>

NU_API void nu_vmcore_init(const nu_byte_t *buffer, nu_size_t size);
NU_API void nu_vmcore_free(void);
NU_API void nu_vmcore_update(void);

#endif
