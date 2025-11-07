#ifndef NUX_GRAPHICS_PLATFORM_H
#define NUX_GRAPHICS_PLATFORM_H

#include <graphics/graphics.h>

typedef enum
{
    NUX_GPU_PIPELINE_MAX    = 128,
    NUX_GPU_FRAMEBUFFER_MAX = 32,
    NUX_GPU_TEXTURE_MAX     = 128,
    NUX_GPU_BUFFER_MAX      = 32,
} nux_os_constants_t;

typedef enum
{
    NUX_GPU_TEXTURE_FILTER_LINEAR,
    NUX_GPU_TEXTURE_FILTER_NEAREST
} nux_gpu_texture_filter_t;

typedef struct
{
    nux_texture_type_t       type;
    nux_gpu_texture_filter_t filter;
    nux_u32_t                width;
    nux_u32_t                height;
} nux_gpu_texture_info_t;

typedef enum
{
    NUX_GPU_PIPELINE_UBER   = 0,
    NUX_GPU_PIPELINE_BLIT   = 1,
    NUX_GPU_PIPELINE_CANVAS = 2,
} nux_gpu_pipeline_type_t;

typedef enum
{
    NUX_GPU_DESC_UBER_CONSTANTS   = 0,
    NUX_GPU_DESC_UBER_BATCHES     = 1,
    NUX_GPU_DESC_UBER_TRANSFORMS  = 2,
    NUX_GPU_DESC_UBER_VERTICES    = 3,
    NUX_GPU_DESC_UBER_TEXTURE0    = 4,
    NUX_GPU_DESC_UBER_BATCH_INDEX = 5,

    NUX_GPU_DESC_CANVAS_CONSTANTS   = 0,
    NUX_GPU_DESC_CANVAS_QUADS       = 1,
    NUX_GPU_DESC_CANVAS_BATCHES     = 2,
    NUX_GPU_DESC_CANVAS_TEXTURE     = 3,
    NUX_GPU_DESC_CANVAS_BATCH_INDEX = 4,

    NUX_GPU_DESC_BLIT_TEXTURE        = 0,
    NUX_GPU_DESC_BLIT_TEXTURE_WIDTH  = 1,
    NUX_GPU_DESC_BLIT_TEXTURE_HEIGHT = 2,

    NUX_GPU_DESC_MAX = 7,
} nux_gpu_desc_t;

typedef struct
{
    nux_gpu_pipeline_type_t type;
    nux_vertex_primitive_t  primitive;
    nux_b32_t               enable_blend;
    nux_b32_t               enable_depth_test;
} nux_gpu_pipeline_info_t;

typedef enum
{
    NUX_GPU_BUFFER_UNIFORM = 0,
    NUX_GPU_BUFFER_STORAGE = 1,
} nux_gpu_buffer_type_t;

typedef enum
{
    NUX_GPU_COMMAND_BIND_FRAMEBUFFER = 0,
    NUX_GPU_COMMAND_BIND_PIPELINE    = 1,
    NUX_GPU_COMMAND_BIND_BUFFER      = 2,
    NUX_GPU_COMMAND_BIND_TEXTURE     = 3,
    NUX_GPU_COMMAND_PUSH_U32         = 4,
    NUX_GPU_COMMAND_PUSH_F32         = 5,
    NUX_GPU_COMMAND_DRAW             = 6,
    NUX_GPU_COMMAND_CLEAR_COLOR      = 7,
    NUX_GPU_COMMAND_CLEAR_DEPTH      = 8,
    NUX_GPU_COMMAND_VIEWPORT         = 9,
} nux_gpu_command_type_t;

typedef struct
{
    nux_u32_t type;
    struct
    {
        nux_u32_t slot;
    } bind_framebuffer;
    struct
    {
        nux_u32_t slot;
    } bind_pipeline;
    struct
    {
        nux_u32_t desc;
        nux_u32_t slot;
    } bind_buffer;
    struct
    {
        nux_u32_t desc;
        nux_u32_t slot;
    } bind_texture;
    struct
    {
        nux_u32_t desc;
        nux_u32_t value;
    } push_u32;
    struct
    {
        nux_u32_t desc;
        nux_f32_t value;
    } push_f32;
    struct
    {
        nux_u32_t count;
    } draw;
    struct
    {
        nux_u32_t color;
    } clear_color;
    struct
    {
        nux_v4_t extent;
    } viewport;
} nux_gpu_command_t;

NUX_API nux_status_t nux_os_pipeline_create(
    void *userdata, nux_u32_t slot, const nux_gpu_pipeline_info_t *info);
NUX_API void         nux_os_pipeline_delete(void *userdata, nux_u32_t slot);
NUX_API nux_status_t nux_os_framebuffer_create(void     *userdata,
                                               nux_u32_t slot,
                                               nux_u32_t texture);
NUX_API void         nux_os_framebuffer_delete(void *userdata, nux_u32_t slot);
NUX_API nux_status_t nux_os_texture_create(void     *userdata,
                                           nux_u32_t slot,
                                           const nux_gpu_texture_info_t *info);
NUX_API void         nux_os_texture_delete(void *userdata, nux_u32_t slot);
NUX_API nux_status_t nux_os_texture_update(void       *userdata,
                                           nux_u32_t   slot,
                                           nux_u32_t   x,
                                           nux_u32_t   y,
                                           nux_u32_t   w,
                                           nux_u32_t   h,
                                           const void *data);
NUX_API nux_status_t nux_os_buffer_create(void                 *userdata,
                                          nux_u32_t             slot,
                                          nux_gpu_buffer_type_t type,
                                          nux_u32_t             size);
NUX_API void         nux_os_buffer_delete(void *userdata, nux_u32_t slot);
NUX_API nux_status_t nux_os_buffer_update(void       *userdata,
                                          nux_u32_t   slot,
                                          nux_u32_t   offset,
                                          nux_u32_t   size,
                                          const void *data);
NUX_API void         nux_os_gpu_submit(void                    *userdata,
                                       const nux_gpu_command_t *cmds,
                                       nux_u32_t                count);

#endif
