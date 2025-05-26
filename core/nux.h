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
    nux_u16_t                slot;
    nux_gpu_texture_format_t format;
    nux_u16_t                texture_width;
    nux_u16_t                texture_height;
    nux_u16_t                write_x;
    nux_u16_t                write_y;
    nux_u16_t                write_width;
    nux_u16_t                write_height;
    const void              *data;
} nux_gpu_update_texture_t;

typedef struct
{
    nux_b32_t       is_uniform_buffer;
    nux_u32_t       offset;
    nux_u32_t       size;
    const nux_u8_t *data;
} nux_gpu_update_buffer_t;

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
        nux_gpu_update_texture_t update_texture;
        nux_gpu_update_buffer_t  update_buffer;
    } data;
} nux_gpu_command_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void        *nux_platform_malloc(void              *userdata,
                                         nux_memory_usage_t usage,
                                         nux_u32_t          n);
NUX_API void         nux_platform_free(void *userdata, void *p);
NUX_API void        *nux_platform_realloc(void *userdata, void *p, nux_u32_t n);
NUX_API nux_status_t nux_platform_mount(void           *userdata,
                                        const nux_c8_t *name,
                                        nux_u32_t       n);
NUX_API nux_status_t nux_platform_seek(void *userdata, nux_u32_t n);
NUX_API nux_u32_t    nux_platform_read(void *userdata, void *p, nux_u32_t n);
NUX_API void nux_platform_log(void *userdata, const nux_c8_t *log, nux_u32_t n);
NUX_API void nux_platform_debug(void            *userdata,
                                const nux_c8_t  *name,
                                nux_u32_t        n,
                                nux_debug_type_t type,
                                void            *p);

//////////////////////////////////////////////////////////////////////////
//////                          Instance API                        //////
//////////////////////////////////////////////////////////////////////////

NUX_API nux_instance_t  *nux_instance_init(const nux_instance_config_t *config);
NUX_API void             nux_instance_free(nux_instance_t *inst);
NUX_API void             nux_instance_tick(nux_instance_t *inst);
NUX_API nux_status_t     nux_instance_load(nux_instance_t *inst,
                                           const nux_c8_t *cart,
                                           nux_u32_t       n);
NUX_API void            *nux_instance_get_userdata(nux_instance_t *inst);
NUX_API void             nux_instance_set_buttons(nux_instance_t *inst,
                                                  nux_u32_t       player,
                                                  nux_u32_t       state);
NUX_API void             nux_instance_set_axis(nux_instance_t *inst,
                                               nux_u32_t       player,
                                               nux_axis_t      axis,
                                               nux_f32_t       value);
NUX_API void             nux_instance_set_stat(nux_instance_t *inst,
                                               nux_stat_t      stat,
                                               nux_u32_t       value);
NUX_API const nux_c8_t  *nux_instance_get_error(nux_instance_t *inst);
NUX_API const nux_u8_t  *nux_instance_get_canvas(nux_instance_t *inst);
NUX_API const nux_u8_t  *nux_instance_get_texture(nux_instance_t *inst);
NUX_API const nux_u8_t  *nux_instance_get_colormap(nux_instance_t *inst);
NUX_API const nux_u32_t *nux_instance_get_buffer(nux_instance_t *inst);

//////////////////////////////////////////////////////////////////////////
//////                           Helper API                         //////
//////////////////////////////////////////////////////////////////////////

NUX_API const nux_c8_t *nux_error_message(nux_error_t error);

#endif
