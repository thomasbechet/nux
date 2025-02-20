#ifndef GFX_H
#define GFX_H

#include "syscall.h"
#include "cartridge.h"

#define GFX_MIN_TEXTURE_SIZE 32
#define GFX_MAX_TEXTURE_SIZE 256

typedef struct
{
    nu_v4u_t   scissor;
    nu_v4u_t   viewport;
    nu_m4_t    model;
    nu_m4_t    view;
    nu_m4_t    projection;
    nu_v2u_t   cursor;
    nu_color_t fog_color;
    nu_f32_t   fog_density;
    nu_f32_t   fog_near;
    nu_f32_t   fog_far;
    nu_color_t color;
} gfx_state_t;

typedef struct
{
    nu_u32_t texture;
    nu_u32_t mesh;
    nu_m4_t  local_to_parent;
    nu_u32_t parent;
} gfx_model_node_t;

typedef struct
{
    gfx_state_t state;
} gfx_t;

nu_status_t gfx_init(vm_t *vm);
nu_status_t gfx_free(vm_t *vm);
void        gfx_begin_frame(vm_t *vm);
void        gfx_end_frame(vm_t *vm);

nu_u32_t gfx_texture_memsize(nu_u32_t size);
nu_u32_t gfx_vertex_memsize(sys_vertex_attribute_t attributes, nu_u32_t count);
nu_u32_t gfx_vertex_offset(sys_vertex_attribute_t attributes,
                           sys_vertex_attribute_t attribute,
                           nu_u32_t               count);

#endif
