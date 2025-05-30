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
    NUX_GPU_TEXTURE_MAX         = 128,
    NUX_GPU_UNIFORM_BUFFER_SIZE = 4096,
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
    NUX_GPU_RGBA,
    NUX_GPU_R,
} nux_gpu_texture_format_t;

typedef enum
{
    NUX_GPU_UPDATE_TEXTURE,
    NUX_GPU_UPDATE_STORAGE_BUFFER,
    NUX_GPU_UPDATE_UNIFORM_BUFFER,
    NUX_GPU_DRAW,
} nux_gpu_command_type_t;

typedef struct
{
    nux_u16_t texture0;
    nux_u32_t primitive;
    nux_u32_t count;
} nux_gpu_draw_t;

typedef struct
{
    nux_gpu_command_type_t type;
    union
    {
        nux_gpu_draw_t draw;
    } data;
} nux_gpu_command_t;

typedef enum
{
    NUX_GPU_SHADER_GLSL,
    NUX_GPU_SHADER_SPIRV,
} nux_gpu_shader_source_t;

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
NUX_API nux_status_t nux_os_create_pipeline(void                   *userdata,
                                            nux_u32_t               slot,
                                            nux_gpu_shader_source_t type,
                                            const nux_c8_t         *vertex,
                                            nux_u32_t               vertex_len,
                                            const nux_c8_t         *fragment,
                                            nux_u32_t fragment_len);
NUX_API nux_status_t nux_os_update_texture(void                    *userdata,
                                           nux_u32_t                slot,
                                           nux_gpu_texture_format_t format,
                                           nux_u32_t                texw,
                                           nux_u32_t                texh,
                                           nux_u32_t                x,
                                           nux_u32_t                y,
                                           nux_u32_t                w,
                                           nux_u32_t                h,
                                           const void              *data);
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
