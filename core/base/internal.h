#ifndef NUX_BASE_INTERNAL_H
#define NUX_BASE_INTERNAL_H

#include <base/module.h>

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       size;
    void           *data;
    nux_u32_t       flags;
    nux_status_t (*on_init)(void);
    void (*on_free)(void);
    nux_status_t (*on_event)(nux_os_event_t *event);
    nux_u32_t dependencies_count;
    nux_u32_t dependencies_first;
} nux_module_info_t;

typedef struct
{
    nux_module_info_t   info;
    nux_module_status_t status;
} nux_module_t;

NUX_VEC_DEFINE(nux_module_vec, nux_module_t);

typedef enum
{
    DEFAULT_MODULE_CAPACITY              = 8,
    DEFAULT_MODULE_DEPENDENCIES_CAPACITY = 64,
    ARENA_ALLOCATOR_TYPE                 = 12345,
} nux_base_defaults_t;

typedef struct
{
    void               *userdata;
    nux_config_t        config;
    nux_b32_t           running;
    nux_u64_t           frame;
    nux_f32_t           time_elapsed;
    nux_pcg_t           pcg;
    nux_u64_t           stats[NUX_STAT_MAX];
    nux_resource_pool_t resources;
    nux_resource_type_t resources_types[NUX_RESOURCE_MAX];
    nux_arena_t        *frame_arena;
    nux_c8_t            error_message[256];
    nux_status_t        error_status;
    nux_status_t        error_enable;
    nux_allocator_t     os_allocator;
    nux_block_arena_t   core_block_arena;
    nux_arena_t        *core_arena;

    nux_module_vec_t modules;
    nux_ptr_vec_t    modules_dependencies;
    nux_module_t    *active_module;
} nux_base_module_t;

nux_status_t nux_base_init(void *userdata);
void         nux_base_free(void);

nux_pcg_t           *nux_base_pcg(void);
nux_resource_pool_t *nux_base_resources(void);
nux_resource_type_t *nux_base_resource_types(void);
nux_allocator_t     *nux_os_allocator(void);

nux_resource_entry_t *nux_resource_add(nux_resource_pool_t *resources,
                                       nux_u32_t            type);

nux_u32_t nux_resource_header_size(nux_u32_t size);
void  nux_resource_header_init(nux_resource_header_t *header, nux_rid_t rid);
void *nux_resource_header_to_data(nux_resource_header_t *header);
nux_resource_header_t *nux_resource_header_from_data(void *data);

#endif
