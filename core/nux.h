#ifndef NUX_H
#define NUX_H

#include "nux_config.h"
#include "nux_api.h"

//////////////////////////////////////////////////////////////////////////
//////                               Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef void (*nux_callback_t)(nux_ctx_t *);

typedef struct
{
    nux_u32_t max_object_count;
    nux_u32_t memory_size;

    nux_u32_t width;
    nux_u32_t height;
    nux_u32_t tick_frequency;

    void          *userdata;
    nux_callback_t init;
    nux_callback_t update;
} nux_config_t;

typedef enum
{
    NUX_GPU_PIPELINE_MAX    = 128,
    NUX_GPU_FRAMEBUFFER_MAX = 32,
    NUX_GPU_TEXTURE_MAX     = 128,
    NUX_GPU_BUFFER_MAX      = 32,
    NUX_IO_FILE_MAX         = 64,
} nux_os_constants_t;

typedef struct
{
    nux_u32_t version;
    nux_u32_t entry_count;
} nux_cart_header_t;

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
    NUX_GPU_PIPELINE_MAIN   = 0,
    NUX_GPU_PIPELINE_BLIT   = 1,
    NUX_GPU_PIPELINE_CANVAS = 2,
} nux_gpu_pipeline_type_t;

typedef enum
{
    NUX_GPU_INDEX_MAIN_CONSTANTS       = 0,
    NUX_GPU_INDEX_MAIN_TRANSFORMS      = 1,
    NUX_GPU_INDEX_MAIN_VERTICES        = 2,
    NUX_GPU_INDEX_MAIN_TEXTURE0        = 3,
    NUX_GPU_INDEX_MAIN_TRANSFORM_INDEX = 4,
    NUX_GPU_INDEX_MAIN_FIRST_VERTEX    = 5,
    NUX_GPU_INDEX_MAIN_HAS_TEXTURE     = 6,

    NUX_GPU_INDEX_CANVAS_CONSTANTS    = 0,
    NUX_GPU_INDEX_CANVAS_QUADS        = 1,
    NUX_GPU_INDEX_CANVAS_MODE         = 2,
    NUX_GPU_INDEX_CANVAS_FIRST_QUAD   = 3,
    NUX_GPU_INDEX_CANVAS_TEXTURE      = 4,
    NUX_GPU_INDEX_CANVAS_ATLAS_WIDTH  = 5,
    NUX_GPU_INDEX_CANVAS_ATLAS_HEIGHT = 6,

    NUX_GPU_INDEX_BLIT_TEXTURE        = 0,
    NUX_GPU_INDEX_BLIT_TEXTURE_WIDTH  = 1,
    NUX_GPU_INDEX_BLIT_TEXTURE_HEIGHT = 2,

    NUX_GPU_INDEX_MAX = 7,
} nux_gpu_index_t;

typedef struct
{
    nux_gpu_pipeline_type_t type;
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
    NUX_GPU_COMMAND_CLEAR            = 7,
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
        nux_u32_t index;
        nux_u32_t slot;
    } bind_buffer;
    struct
    {
        nux_u32_t index;
        nux_u32_t slot;
    } bind_texture;
    struct
    {
        nux_u32_t index;
        nux_u32_t value;
    } push_u32;
    struct
    {
        nux_u32_t index;
        nux_f32_t value;
    } push_f32;
    struct
    {
        nux_u32_t index;
    } push_v2;
    struct
    {
        nux_u32_t count;
    } draw;
    struct
    {
        nux_u32_t color;
    } clear;
} nux_gpu_command_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void        *nux_os_malloc(void *userdata, nux_u32_t n);
NUX_API void         nux_os_free(void *userdata, void *p);
NUX_API void        *nux_os_realloc(void *userdata, void *p, nux_u32_t n);
NUX_API nux_status_t nux_os_open(void           *userdata,
                                 nux_u32_t       slot,
                                 const nux_c8_t *url,
                                 nux_u32_t       n);
NUX_API nux_status_t nux_os_seek(void *userdata, nux_u32_t slot, nux_u32_t n);
NUX_API nux_u32_t    nux_os_read(void     *userdata,
                                 nux_u32_t slot,
                                 void     *p,
                                 nux_u32_t n);
NUX_API void         nux_os_console(void           *userdata,
                                    nux_log_level_t level,
                                    const nux_c8_t *log,
                                    nux_u32_t       n);
NUX_API nux_status_t nux_os_create_pipeline(
    void *userdata, nux_u32_t slot, const nux_gpu_pipeline_info_t *info);
NUX_API nux_status_t nux_os_create_framebuffer(void     *userdata,
                                               nux_u32_t slot,
                                               nux_u32_t texture);
NUX_API nux_status_t nux_os_create_texture(void     *userdata,
                                           nux_u32_t slot,
                                           const nux_gpu_texture_info_t *info);
NUX_API nux_status_t nux_os_update_texture(void       *userdata,
                                           nux_u32_t   slot,
                                           nux_u32_t   x,
                                           nux_u32_t   y,
                                           nux_u32_t   w,
                                           nux_u32_t   h,
                                           const void *data);
NUX_API nux_status_t nux_os_create_buffer(void                 *userdata,
                                          nux_u32_t             slot,
                                          nux_gpu_buffer_type_t type,
                                          nux_u32_t             size);
NUX_API nux_status_t nux_os_update_buffer(void       *userdata,
                                          nux_u32_t   slot,
                                          nux_u32_t   offset,
                                          nux_u32_t   size,
                                          const void *data);
NUX_API void         nux_os_gpu_submit(void                    *userdata,
                                       const nux_gpu_command_t *cmds,
                                       nux_u32_t                count);
NUX_API void         nux_os_update_inputs(void      *user,
                                          nux_u32_t *buttons,
                                          nux_f32_t *axis);
NUX_API void         nux_os_update_stats(void *userdata, nux_u32_t *stats);

//////////////////////////////////////////////////////////////////////////
//////                          Instance API                        //////
//////////////////////////////////////////////////////////////////////////

NUX_API
nux_ctx_t           *nux_instance_init(const nux_config_t *config);
NUX_API void         nux_instance_free(nux_ctx_t *ctx);
NUX_API void         nux_instance_tick(nux_ctx_t *ctx);
NUX_API nux_status_t nux_instance_load(nux_ctx_t      *ctx,
                                       const nux_c8_t *cart,
                                       nux_u32_t       n);

#endif
