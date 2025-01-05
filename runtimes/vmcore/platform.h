#ifndef NUX_PLATFORM_H
#define NUX_PLATFORM_H

#include <nulib.h>

void      os_mount(void *user, const nu_byte_t *name);
void      os_seek(void *user, nu_size_t n);
nu_size_t os_read(void *user, void *p, nu_size_t n);
void      os_trace(void *user, const void *str, nu_size_t n);

void os_swap_buffer(void *user);
void os_write_texture(void       *user,
                      nu_u32_t    slot,
                      nu_u32_t    x,
                      nu_u32_t    y,
                      nu_u32_t    w,
                      nu_u32_t    h,
                      const void *p);
void os_write_vertex(void *user, nu_u32_t first, nu_u32_t count, const void *p);
void os_bind_texture(void *user, nu_u32_t slot);
void os_draw(void *user);

#endif
