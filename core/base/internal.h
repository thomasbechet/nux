#ifndef NUX_BASE_INTERNAL_H
#define NUX_BASE_INTERNAL_H

#include <base/module.h>

NUX_VEC_DEFINE(nux_module_vec, nux_module_t);

typedef enum
{
    DEFAULT_MODULE_CAPACITY = 8,
} nux_base_defaults_t;

typedef struct
{
    void               *userdata;
    nux_config_t        config;
    nux_b32_t           running;
    nux_u64_t           frame;
    nux_f32_t           time_elapsed;
    nux_pcg_t           pcg;
    nux_module_vec_t    modules;
    nux_u64_t           stats[NUX_STAT_MAX];
    nux_resource_pool_t resources;
    nux_resource_type_t resources_types[NUX_RESOURCE_MAX];
    nux_arena_t        *core_arena;
    nux_arena_t        *frame_arena;
    nux_c8_t            error_message[256];
    nux_status_t        error_status;
    nux_status_t        error_enable;
} nux_base_module_t;

nux_status_t         nux_base_init(void *userdata);
nux_pcg_t           *nux_base_pcg(void);
nux_resource_pool_t *nux_base_resources(void);
nux_resource_type_t *nux_base_resource_types(void);

#endif
