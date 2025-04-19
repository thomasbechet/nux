#ifndef NUX_H
#define NUX_H

#include "nux_config.h"
#include "nux_api.h"

//////////////////////////////////////////////////////////////////////////
//////                               Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct nux_instance *nux_instance_t;
typedef void (*nux_callback_t)(nux_env_t);

typedef struct
{
    void          *userdata;
    nux_callback_t init;
    nux_callback_t update;
} nux_instance_config_t;

typedef enum
{
    NUX_INSPECT_I32,
    NUX_INSPECT_F32,
} nux_inspect_type_t;

typedef enum
{
    NUX_MEMORY_USAGE_STATE,
    NUX_MEMORY_USAGE_CORE,
} nux_memory_usage_t;

typedef struct
{
    nux_u32_t version;
    nux_u32_t entry_count;
} nux_cart_header_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void        *nux_platform_malloc(void              *userdata,
                                         nux_memory_usage_t usage,
                                         nux_u32_t          n);
NUX_API void         nux_platform_free(void *userdata, void *p);
NUX_API void        *nux_platform_realloc(void *userdata, void *p, nux_u32_t n);
NUX_API nux_status_t nux_platform_mount(nux_instance_t  inst,
                                        const nux_c8_t *name,
                                        nux_u32_t       n);
NUX_API nux_status_t nux_platform_seek(nux_instance_t inst, nux_u32_t n);
NUX_API nux_u32_t nux_platform_read(nux_instance_t inst, void *p, nux_u32_t n);
NUX_API void      nux_platform_log(nux_instance_t  inst,
                                   const nux_c8_t *log,
                                   nux_u32_t       n);
NUX_API void      nux_platform_inspect(nux_instance_t     inst,
                                       const nux_c8_t    *name,
                                       nux_u32_t          n,
                                       nux_inspect_type_t type,
                                       void              *p);

//////////////////////////////////////////////////////////////////////////
//////                          Instance API                        //////
//////////////////////////////////////////////////////////////////////////

NUX_API nux_instance_t nux_instance_init(const nux_instance_config_t *config);
NUX_API void           nux_instance_free(nux_instance_t inst);
NUX_API void           nux_instance_tick(nux_instance_t inst);
NUX_API nux_status_t   nux_instance_load(nux_instance_t  inst,
                                         const nux_c8_t *cart,
                                         nux_u32_t       n);
NUX_API void  nux_instance_save_state(nux_instance_t inst, nux_u8_t *state);
NUX_API void  nux_instance_load_state(nux_instance_t  inst,
                                      const nux_u8_t *state);
NUX_API void *nux_instance_get_userdata(nux_instance_t inst);
NUX_API void  nux_instance_set_buttons(nux_instance_t inst,
                                       nux_u32_t      player,
                                       nux_u32_t      state);
NUX_API void  nux_instance_set_axis(nux_instance_t inst,
                                    nux_u32_t      player,
                                    nux_axis_t     axis,
                                    nux_f32_t      value);
NUX_API const nux_c8_t *nux_instance_get_error(nux_instance_t inst);
NUX_API const nux_u8_t *nux_instance_get_framebuffer(nux_instance_t inst);
NUX_API const nux_u8_t *nux_instance_get_palette(nux_instance_t inst);

//////////////////////////////////////////////////////////////////////////
//////                           Helper API                         //////
//////////////////////////////////////////////////////////////////////////

NUX_API nux_u32_t       nux_texture_memsize(nux_u32_t size);
NUX_API nux_u32_t       nux_vertex_memsize(nux_vertex_attribute_t attributes,
                                           nux_u32_t              count);
NUX_API nux_u32_t       nux_vertex_offset(nux_vertex_attribute_t attributes,
                                          nux_vertex_attribute_t attribute,
                                          nux_u32_t              count);
NUX_API const nux_c8_t *nux_error_message(nux_error_t error);
NUX_API nux_status_t    nux_cart_parse_header(const void        *data,
                                              nux_cart_header_t *header);

#endif
