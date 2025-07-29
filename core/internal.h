#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "base/base.h"
#include "io/io.h"
#include "graphics/graphics.h"
#include "lua/lua.h"
#include "scene/scene.h"
#include "ecs/ecs.h"

struct nux_context
{
    // Thread data

    nux_c8_t     error_message[256];
    nux_status_t error_status;
    nux_arena_t *active_arena;
    nux_res_t    active_arena_id;

    // System

    void          *userdata;
    nux_b32_t      running;
    nux_u64_t      frame;
    nux_f32_t      time;
    nux_callback_t init;
    nux_callback_t update;
    nux_pcg_t      pcg;

    // input

    nux_u32_t buttons[NUX_PLAYER_MAX];
    nux_u32_t buttons_prev[NUX_PLAYER_MAX];
    nux_f32_t axis[NUX_PLAYER_MAX * NUX_AXIS_MAX];
    nux_f32_t axis_prev[NUX_PLAYER_MAX * NUX_AXIS_MAX];

    nux_u32_t stats[NUX_STAT_MAX];

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

    // arena

    nux_arena_pool_t    arenas;
    nux_resource_pool_t resources;
    nux_arena_t        *core_arena;
    nux_res_t           frame_arena;

    // type

    nux_resource_type_t       resources_types[NUX_RES_MAX];
    nux_u32_t            resources_types_count;
    nux_component_type_t component_types[NUX_COMPONENT_MAX];
    nux_u32_t            component_types_count;

    // io

    nux_disk_t        disks[NUX_DISK_MAX];
    nux_u32_t         disks_count;
    nux_u32_vec_t     free_file_slots;
    nux_cart_writer_t cart_writer;

    // lua

    lua_State *L;

    // ecs

    nux_ecs_component_vec_t components;
    nux_ecs_iter_t          iters;
};

#endif
