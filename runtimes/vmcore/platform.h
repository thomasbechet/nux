#ifndef NUX_PLATFORM_H
#define NUX_PLATFORM_H

#include <nulib.h>

void      os_mount(void *user, const nu_byte_t *name);
void      os_seek(void *user, nu_size_t n);
nu_size_t os_read(void *user, void *p, nu_size_t n);

void os_swap_buffer(void *user);
void os_draw(void *user);

#endif
