#ifndef NUX_SCENE_H
#define NUX_SCENE_H

#include "graphics/graphics.h"

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct
{
    nux_b2i_t viewport;
    nux_f32_t fov;
    nux_f32_t near;
    nux_f32_t far;
} nux_camera_t;

typedef struct
{
    nux_u32_t parent;
    nux_v3_t  local_translation;
    nux_q4_t  local_rotation;
    nux_v3_t  local_scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

typedef struct
{
    nux_res_t mesh;
    nux_res_t texture;
} nux_staticmesh_t;

typedef enum
{
    NUX_COMPONENT_TRANSFORM  = 0,
    NUX_COMPONENT_CAMERA     = 1,
    NUX_COMPONENT_STATICMESH = 2,

    NUX_COMPONENT_MAX = 16,
} nux_component_type_base_t;

typedef struct
{
    nux_u32_t type;
} nux_component_type_t;

typedef struct
{
    nux_res_t scene;
    nux_res_t parent;
    nux_res_t id;
    nux_u32_t components[NUX_COMPONENT_MAX];
} nux_node_t;

typedef union
{
    nux_transform_t  transform;
    nux_camera_t     camera;
    nux_staticmesh_t staticmesh;
} nux_component_t;

NUX_POOL_DEFINE(nux_node_pool, nux_node_t);
NUX_POOL_DEFINE(nux_component_pool, nux_component_t);

typedef struct
{
    nux_u32_t first_vertex;
    nux_u32_t first_transform;
    nux_u32_t has_texture;
} nux_gpu_scene_batch_t;

typedef struct
{
    nux_arena_t          *arena;
    nux_node_pool_t       nodes;
    nux_component_pool_t  components;
    nux_gpu_buffer_t      constants_buffer;
    nux_gpu_buffer_t      batches_buffer;
    nux_u32_t             batches_buffer_head;
    nux_gpu_buffer_t      transforms_buffer;
    nux_u32_t             transforms_buffer_head;
    nux_gpu_command_vec_t commands;
    nux_gpu_command_vec_t commands_lines;
} nux_scene_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// scene.c

void nux_component_register(nux_ctx_t *ctx, nux_u32_t type);

void  nux_scene_cleanup(nux_ctx_t *ctx, void *data);
void *nux_scene_add_component(nux_ctx_t *ctx, nux_res_t node, nux_u32_t type);
void nux_scene_remove_component(nux_ctx_t *ctx, nux_res_t node, nux_u32_t type);
void *nux_scene_get_component(nux_ctx_t *ctx, nux_res_t node, nux_u32_t type);

// transform.c

nux_b32_t nux_transform_update_matrix(nux_ctx_t *ctx, nux_res_t node);

#endif
