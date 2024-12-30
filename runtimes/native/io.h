#ifndef NUX_NATIVE_IO_H
#define NUX_NATIVE_IO_H

#include "platform.h"

void      os_mount(void *user, const nu_byte_t *name);
void      os_seek(void *user, nu_size_t n);
nu_size_t nos_read(void *user, void *p, nu_size_t n);

#endif
