#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "base/internal.h"
#include "ecs/internal.h"
#include "graphics/internal.h"
#include "physics/internal.h"

struct nux_context
{
    // thread data

    nux_c8_t     error_message[256];
    nux_status_t error_status;
    nux_status_t error_enable;
    nux_ecs_t   *active_ecs;

    // base

    void               *userdata;
    nux_config_t        config;
    nux_b32_t           running;
    nux_u64_t           frame;
    nux_f32_t           time_elapsed;
    nux_pcg_t           pcg;
    nux_resource_pool_t resources;
    nux_arena_t         core_arena;
    nux_res_t           core_arena_res;
    nux_res_t           frame_arena;
    nux_resource_type_t resources_types[NUX_RES_MAX];
    nux_u32_t           resources_types_count;
    nux_controller_t    controllers[NUX_CONTROLLER_MAX];
    nux_u64_t           stats[NUX_STAT_MAX];
    nux_disk_t          disks[NUX_DISK_MAX];
    nux_u32_t           disks_count;
    nux_u32_vec_t       free_file_slots;
    nux_cart_writer_t   cart_writer;
    lua_State          *L;

    // ecs

    nux_ecs_module_t *ecs;

    // graphics

    nux_graphics_module_t *graphics;

    // physics

    nux_physics_module_t *physics;
};

#endif
