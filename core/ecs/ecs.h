#ifndef NUX_ECS_H
#define NUX_ECS_H

#include "base/base.h"

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
    nux_arena_t            *arena;
    nux_ecs_bitset_t        bitset;
    nux_ecs_container_vec_t containers;
} nux_ecs_t;

NUX_VEC_DEFINE(nux_ecs_component_vec, nux_ecs_component_t);

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// ecs.c

nux_status_t nux_ecs_init(nux_ctx_t *ctx);
nux_u32_t    nux_ecs_register_component(nux_ctx_t      *ctx,
                                        const nux_c8_t *name,
                                        nux_u32_t       size);

void *nux_ecs_set(nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c);
void *nux_ecs_get(nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c);

void nux_ecs_cleanup(nux_ctx_t *ctx, void *data);

#endif
