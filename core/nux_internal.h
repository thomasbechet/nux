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

    nux_ecs_component_vec_t components;

    // graphics

    nux_gpu_pipeline_t uber_pipeline_opaque;
    nux_gpu_pipeline_t uber_pipeline_line;
    nux_gpu_pipeline_t blit_pipeline;
    nux_gpu_pipeline_t canvas_pipeline;
    nux_gpu_buffer_t   vertices_buffer;
    nux_u32_t          vertices_buffer_head;
    nux_font_t         default_font;
    nux_u8_t           pal[NUX_PALETTE_SIZE];
    nux_u32_t          colormap[NUX_COLORMAP_SIZE];
    nux_v2i_t          cursor;
    nux_u32_vec_t      free_texture_slots;
    nux_u32_vec_t      free_buffer_slots;
    nux_u32_vec_t      free_pipeline_slots;
    nux_u32_vec_t      free_framebuffer_slots;
    nux_renderer_t     renderer;

    // physics

    nux_point_mass_vec_t           point_masses;
    nux_collision_constraint_vec_t collision_constraints;
    nux_distance_constraint_vec_t  distance_constraints;
    nux_res_t                      rigidbody_transform_iter;
    nux_res_t                      collider_transform_iter;
};

#endif
