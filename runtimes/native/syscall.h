#ifndef NU_SYSCALL_H
#define NU_SYSCALL_H

#include <runtimes/native/core.h>

void config(nu_u32_t max_tex128, nu_u32_t max_tex256, nu_u32_t max);

nu_u32_t alloc_texture(nu_u32_t type);
nu_u32_t alloc_vertices(nu_u32_t flags, nu_u32_t count);
void     release_texture(nu_u32_t idx);
void     release_vertex_buffer(nu_u32_t idx);

void upload_texture(nu_u32_t         idx,
                    nu_u32_t         x,
                    nu_u32_t         y,
                    nu_u32_t         w,
                    nu_u32_t         h,
                    const nu_byte_t *data);
void upload_vertex_buffer(nu_u32_t         idx,
                          nu_u32_t         first,
                          nu_u32_t         count,
                          const nu_byte_t *data);

void draw(nu_u32_t primitive, nu_u32_t first, nu_u32_t count);
void bind_texture(nu_u32_t idx);
void bind_vertex_buffer(nu_u32_t idx);

void load(nu_byte_t *dst, nu_u32_t section);

#endif
