#ifndef NUX_H
#define NUX_H

#include "nux_config.h"
#include "nux_api.h"

//////////////////////////////////////////////////////////////////////////
//////                               Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct nux_instance nux_instance_t;
typedef void (*nux_callback_t)(nux_env_t *);

typedef struct
{
    nux_u32_t max_object_count;
    nux_u32_t memory_size;

    void          *userdata;
    nux_callback_t init;
    nux_callback_t update;
} nux_instance_config_t;

typedef enum
{
    NUX_GPU_PROGRAM_MAX = 128,
    NUX_GPU_TEXTURE_MAX = 128,
    NUX_GPU_BUFFER_MAX  = 8,
} nux_gpu_constants_t;

typedef enum
{
    NUX_DEBUG_I32,
    NUX_DEBUG_F32,
} nux_debug_type_t;

typedef enum
{
    NUX_MEMORY_USAGE_CORE,
    NUX_MEMORY_USAGE_STATE,
    NUX_MEMORY_USAGE_GPU_BUFFER,
    NUX_MEMORY_USAGE_GPU_COMMAND,
} nux_memory_usage_t;

typedef struct
{
    nux_u32_t version;
    nux_u32_t entry_count;
} nux_cart_header_t;

typedef enum
{
    NUX_GPU_TEXTURE_FORMAT_RGBA,
    NUX_GPU_TEXTURE_FORMAT_INDEX,
} nux_gpu_texture_format_t;

typedef enum
{
    NUX_GPU_TEXTURE_FILTER_LINEAR,
    NUX_GPU_TEXTURE_FILTER_NEAREST
} nux_gpu_texture_filter_t;

typedef struct
{
    nux_gpu_texture_format_t format;
    nux_gpu_texture_filter_t filter;
    nux_u32_t                width;
    nux_u32_t                height;
} nux_gpu_texture_info_t;

typedef enum
{
    NUX_GPU_BEGIN_RENDERPASS,
    NUX_GPU_DRAW_MAIN,
    NUX_GPU_DRAW_CANVAS,
} nux_gpu_command_type_t;

typedef enum
{
    NUX_GPU_BUFFER_UNIFORM = 0,
    NUX_GPU_BUFFER_STORAGE = 1,
} nux_gpu_buffer_type_t;

typedef enum
{
    NUX_GPU_PIPELINE_MAIN,
    NUX_GPU_PIPELINE_CANVAS,
} nux_gpu_pipeline_type_t;

typedef struct
{
    nux_u32_t storage;
    nux_u32_t uniform;
    nux_u32_t vertex_count;
} nux_gpu_draw_main_t;

typedef struct
{
    nux_u32_t colormap;
    nux_u32_t canvas;
    nux_u32_t uniform;
} nux_gpu_draw_canvas_t;

typedef struct
{
    nux_u32_t framebuffer;
    nux_u32_t pipeline;
    nux_u32_t viewport[4];
    nux_u32_t scissor[4];
} nux_gpu_begin_renderpass_t;

typedef struct
{
    nux_gpu_command_type_t type;
    union
    {
        nux_gpu_begin_renderpass_t begin_renderpass;
        nux_gpu_draw_main_t        draw_main;
        nux_gpu_draw_canvas_t      draw_canvas;
    };
} nux_gpu_command_t;

typedef struct
{
    nux_gpu_command_type_t type : 2;
} nux_gpu_command2_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void        *nux_os_malloc(void              *userdata,
                                   nux_memory_usage_t usage,
                                   nux_u32_t          n);
NUX_API void         nux_os_free(void *userdata, void *p);
NUX_API void        *nux_os_realloc(void *userdata, void *p, nux_u32_t n);
NUX_API void         nux_os_debug(void            *userdata,
                                  const nux_c8_t  *name,
                                  nux_u32_t        n,
                                  nux_debug_type_t type,
                                  void            *p);
NUX_API nux_status_t nux_os_mount(void           *userdata,
                                  const nux_c8_t *name,
                                  nux_u32_t       n);
NUX_API nux_status_t nux_os_seek(void *userdata, nux_u32_t n);
NUX_API nux_u32_t    nux_os_read(void *userdata, void *p, nux_u32_t n);
NUX_API void nux_os_log(void *userdata, const nux_c8_t *log, nux_u32_t n);
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
NUX_API void         nux_os_submit_commands(void                    *userdata,
                                            const nux_gpu_command_t *commands,
                                            nux_u32_t                count);
NUX_API void         nux_os_update_inputs(void      *user,
                                          nux_u32_t *buttons,
                                          nux_f32_t *axis);
NUX_API void         nux_os_update_stats(void *userdata, nux_u32_t *stats);

//////////////////////////////////////////////////////////////////////////
//////                          Instance API                        //////
//////////////////////////////////////////////////////////////////////////

NUX_API
nux_instance_t      *nux_instance_init(const nux_instance_config_t *config);
NUX_API void         nux_instance_free(nux_instance_t *inst);
NUX_API void         nux_instance_tick(nux_instance_t *inst);
NUX_API nux_status_t nux_instance_load(nux_instance_t *inst,
                                       const nux_c8_t *cart,
                                       nux_u32_t       n);

#endif
