#ifndef NUX_ECS_H
#define NUX_ECS_H

#include "base/base.h"

////////////////////////////
///        TYPES         ///
////////////////////////////

#define ECS_ENTITY_PER_MASK    32
#define ECS_COMPONENT_NAME_LEN 64

typedef nux_u32_t nux_ecs_mask_t;
NUX_VEC_DEFINE(nux_ecs_bitset, nux_ecs_mask_t);
NUX_VEC_DEFINE(nux_ecs_chunk_vec, void *);

typedef struct
{
    nux_c8_t name[ECS_COMPONENT_NAME_LEN];
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
    nux_ecs_mask_t mask;
    nux_u32_t      mask_index;
    nux_u32_t      mask_offset;
} nux_ecs_iter_t;

typedef struct
{
    nux_ecs_bitset_t        bitset;
    nux_ecs_container_vec_t containers;
} nux_ecs_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// ecs.c

nux_status_t nux_ecs_init(nux_ctx_t *ctx);
nux_u32_t    nux_ecs_register_component(const nux_c8_t *name, nux_u32_t size);

nux_id_t  nux_ecs_new(nux_u32_t capa);
nux_u32_t nux_ecs_add(nux_id_t ecs);
nux_u32_t nux_ecs_add_at(nux_id_t ecs, nux_u32_t index);
void      nux_ecs_remove(nux_id_t ecs, nux_u32_t e);
nux_b32_t nux_ecs_valid(nux_id_t ecs, nux_u32_t e);
nux_u32_t nux_ecs_count(nux_id_t ecs);
nux_u32_t nux_ecs_capacity(nux_id_t ecs);
nux_u32_t nux_ecs_clear(nux_id_t ecs);
void     *nux_ecs_set(nux_id_t ecs, nux_u32_t e, nux_u32_t c);
void      nux_ecs_unset(nux_id_t ecs, nux_u32_t e, nux_u32_t c);
nux_b32_t nux_ecs_has(nux_id_t ecs, nux_u32_t e, nux_u32_t c);
void     *nux_ecs_get(nux_id_t ecs, nux_u32_t e, nux_u32_t c);
nux_u32_t nux_ecs_iter(nux_u32_t include_count, nux_u32_t exclude_count);
void      nux_ecs_includes(nux_u32_t iter, nux_u32_t c);
void      nux_ecs_excludes(nux_u32_t iter, nux_u32_t c);
nux_u32_t nux_ecs_begin(nux_id_t ecs, nux_u32_t iter);
nux_u32_t nux_ecs_next(nux_id_t ecs, nux_u32_t iter);

#endif
