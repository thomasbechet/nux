#ifndef NUX_GRAPHICS_INTERNAL_H
#define NUX_GRAPHICS_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_gpu_pipeline_t uber_pipeline_opaque;
    nux_gpu_pipeline_t uber_pipeline_line;
    nux_gpu_pipeline_t blit_pipeline;
    nux_gpu_pipeline_t canvas_pipeline;
    nux_gpu_buffer_t   vertices_buffer;
    nux_u32_t          vertices_buffer_head;
    nux_u32_t          vertices_buffer_head_frame;
    nux_gpu_buffer_t   transforms_buffer;
    nux_u32_t          transforms_buffer_head;
    nux_u32_t          transforms_buffer_head_frame;
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
    nux_u32_t         batches_buffer_head;
    nux_ecs_iter_t   *transform_iter;
    nux_ecs_iter_t   *transform_staticmesh_iter;
    nux_ecs_iter_t   *transform_camera_iter;

    const nux_texture_t *active_texture;
} nux_graphics_module_t;

nux_graphics_module_t *nux_graphics_module(void);

nux_status_t nux_graphics_init(void);
void         nux_graphics_free(void);
nux_status_t nux_graphics_pre_update(void);
nux_status_t nux_graphics_update(void);

#endif
