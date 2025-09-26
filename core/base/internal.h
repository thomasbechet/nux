#ifndef NUX_BASE_INTERNAL_H
#define NUX_BASE_INTERNAL_H

#include "module.h"

typedef struct
{
    void               *userdata;
    nux_config_t        config;
    nux_b32_t           running;
    nux_u64_t           frame;
    nux_f32_t           time_elapsed;
    nux_pcg_t           pcg;
    nux_resource_pool_t resources;
    nux_arena_t        *core_arena;
    nux_arena_t        *frame_arena;
    nux_resource_type_t resources_types[NUX_RESOURCE_MAX];
    nux_u64_t           stats[NUX_STAT_MAX];
    nux_c8_t            error_message[256];
    nux_status_t        error_status;
    nux_status_t        error_enable;
} nux_base_module_t;

nux_base_module_t *nux_base_module(void);
nux_status_t       nux_resource_init(void);

#endif
