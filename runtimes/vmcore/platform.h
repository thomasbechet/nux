#ifndef NUX_PLATFORM_H
#define NUX_PLATFORM_H

#include <nulib.h>

typedef enum
{
    NUX_TEX64  = 0,
    NUX_TEX128 = 1,
    NUX_TEX256 = 2
} nux_gpu_texture_t;

void *os_malloc(void *user, nu_size_t n);

void      os_mount(void *user, const nu_byte_t *name);
void      os_seek(void *user, nu_size_t n);
nu_size_t os_read(void *user, void *p, nu_size_t n);
void      os_trace(void *user, const void *str, nu_size_t n);

void os_swap_buffer(void *user);
void os_write_texture(void             *user,
                      nux_gpu_texture_t type,
                      nu_u32_t          slot,
                      const void       *p);
void os_draw(void *user);

#endif
