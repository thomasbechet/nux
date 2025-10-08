#ifndef NUX_GRAPHICS_INTERNAL_H
#define NUX_GRAPHICS_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_u32_t top;
    nux_u32_t bottom;
    nux_u32_t capacity;
} nux_dsa_t; // double stack allocator

typedef struct
{
    nux_gpu_pipeline_t uber_pipeline_opaque;
    nux_gpu_pipeline_t uber_pipeline_line;
    nux_gpu_pipeline_t blit_pipeline;
    nux_gpu_pipeline_t canvas_pipeline;
    nux_gpu_buffer_t   vertices_buffer;
    nux_dsa_t          vertices_dsa;
    nux_gpu_buffer_t   transforms_buffer;
    nux_dsa_t          transforms_dsa;
    nux_font_t         default_font;
    nux_u32_vec_t      free_texture_slots;
    nux_u32_vec_t      free_buffer_slots;
    nux_u32_vec_t      free_pipeline_slots;
    nux_u32_vec_t      free_framebuffer_slots;
    nux_u32_t          identity_transform_index;

    nux_gpu_encoder_t encoder;
    nux_gpu_encoder_t immediate_encoder;
    nux_gpu_buffer_t  constants_buffer;
    nux_gpu_buffer_t  batches_buffer;
    nux_dsa_t         batches_dsa;
    nux_query_t      *transform_iter;
    nux_query_t      *transform_staticmesh_iter;
    nux_query_t      *transform_camera_iter;

    const nux_texture_t *active_texture;
} nux_graphics_module_t;

nux_graphics_module_t *nux_graphics_module(void);

// graphics.c

nux_status_t nux_graphics_init(void);
void         nux_graphics_free(void);
nux_status_t nux_graphics_pre_update(void);
nux_status_t nux_graphics_update(void);

// buffer.c

void         nux_dsa_init(nux_dsa_t *a, nux_u32_t capacity);
nux_status_t nux_dsa_push_bottom(nux_dsa_t *a,
                                 nux_u32_t  count,
                                 nux_u32_t *index);
nux_status_t nux_dsa_push_top(nux_dsa_t *a, nux_u32_t count, nux_u32_t *index);
void         nux_dsa_reset_bottom(nux_dsa_t *a);
void         nux_dsa_reset_top(nux_dsa_t *a);

#endif
