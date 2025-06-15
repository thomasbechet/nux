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
    NUX_GPU_PIPELINE_MAX    = 128,
    NUX_GPU_FRAMEBUFFER_MAX = 32,
    NUX_GPU_TEXTURE_MAX     = 128,
    NUX_GPU_BUFFER_MAX      = 32,
} nux_gpu_constants_t;

typedef enum
{
    NUX_DEBUG_I32,
    NUX_DEBUG_F32,
} nux_debug_type_t;

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
    NUX_GPU_BUFFER_UNIFORM = 0,
    NUX_GPU_BUFFER_STORAGE = 1,
} nux_gpu_buffer_type_t;

typedef enum
{
    NUX_GPU_PASS_MAIN,
    NUX_GPU_PASS_CANVAS,
} nux_gpu_pass_type_t;

typedef struct
{
    nux_gpu_pass_type_t type;
    nux_u32_t           pipeline;
    nux_u32_t           count;
    nux_u32_t           framebuffer;
    union
    {
        struct
        {
            nux_u32_t uniform_buffer;
        } main;
        struct
        {
            nux_u32_t uniform_buffer;
        } canvas;
    };
} nux_gpu_pass_t;

typedef union
{
    struct
    {
        nux_u32_t texture;
        nux_u32_t colormap;
        nux_u32_t storage;
        nux_u32_t vertex_base;
        nux_u32_t vertex_count;
    } main;
    struct
    {
        nux_u32_t texture;
        nux_u32_t colormap;
        nux_u32_t vertex_count;
    } canvas;
} nux_gpu_command_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void        *nux_os_malloc(void *userdata, nux_u32_t n);
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
NUX_API nux_status_t nux_os_create_pipeline(void               *userdata,
                                            nux_u32_t           slot,
                                            nux_gpu_pass_type_t type);
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
NUX_API void         nux_os_gpu_submit_pass(void                    *userdata,
                                            const nux_gpu_pass_t    *pass,
                                            const nux_gpu_command_t *cmds);
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
