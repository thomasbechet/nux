#ifndef NUX_ECS_INTERNAL_H
#define NUX_ECS_INTERNAL_H

#include "graphics/internal.h"

////////////////////////////
///        TYPES         ///
////////////////////////////

#define ECS_ENTITY_PER_MASK    32
#define ECS_COMPONENT_NAME_LEN 64
#define ECS_COMPONENT_MAX      64

typedef nux_u32_t nux_ecs_mask_t;
NUX_VEC_DEFINE(nux_ecs_bitset, nux_ecs_mask_t);
NUX_VEC_DEFINE(nux_ecs_chunk_vec, void *);

typedef struct
{
    nux_c8_t  name[ECS_COMPONENT_NAME_LEN + 1];
    nux_u32_t size;
} nux_ecs_component_t;

typedef struct
{
    nux_u32_t           capa;
    nux_ecs_chunk_vec_t chunks;
    nux_ecs_bitset_t    bitset;
    nux_u32_t           component_size;
} nux_ecs_container_t;

NUX_VEC_DEFINE(nux_ecs_container_vec, nux_ecs_container_t);

typedef struct
{
    nux_u32_vec_t  includes;
    nux_u32_vec_t  excludes;
    nux_res_t      ecs; // current ecs
    nux_ecs_mask_t mask;
    nux_u32_t      mask_index;
    nux_u32_t      mask_offset;
} nux_ecs_iter_t;

typedef struct
{
    nux_res_t               self;
    nux_arena_t            *arena;
    nux_ecs_bitset_t        bitset;
    nux_ecs_container_vec_t containers;
} nux_ecs_t;

NUX_VEC_DEFINE(nux_ecs_component_vec, nux_ecs_component_t);

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
    nux_u32_t transform;
} nux_staticmesh_t;

typedef enum
{
    NUX_COMPONENT_TRANSFORM  = 1,
    NUX_COMPONENT_CAMERA     = 2,
    NUX_COMPONENT_STATICMESH = 3,
    NUX_COMPONENT_RIGIDBODY  = 4,
    NUX_COMPONENT_COLLIDER   = 5,

    NUX_COMPONENT_MAX = 16,
} nux_component_type_base_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// ecs.c

nux_status_t nux_ecs_init(nux_ctx_t *ctx);
void         nux_ecs_free(nux_ctx_t *ctx);
nux_u32_t    nux_ecs_register_component(nux_ctx_t      *ctx,
                                        const nux_c8_t *name,
                                        nux_u32_t       size);

void *nux_ecs_set(nux_ctx_t *ctx, nux_ent_t e, nux_u32_t c);
void *nux_ecs_get(nux_ctx_t *ctx, nux_ent_t e, nux_u32_t c);

void nux_ecs_cleanup(nux_ctx_t *ctx, nux_res_t res);

// lua_bindings.c

nux_status_t nux_lua_open_ecs(nux_ctx_t *ctx);

// transform.c

nux_b32_t nux_transform_update_matrix(nux_ctx_t *ctx, nux_ent_t e);

#endif
