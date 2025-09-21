#ifndef NUX_ECS_INTERNAL_H
#define NUX_ECS_INTERNAL_H

#include <io/internal.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

#define ECS_ENTITY_PER_MASK 32

typedef nux_u32_t nux_ecs_mask_t;
NUX_VEC_DEFINE(nux_ecs_bitset, nux_ecs_mask_t);
NUX_VEC_DEFINE(nux_ecs_chunk_vec, void *);

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       size;
    nux_status_t (*read)(nux_serde_reader_t *s,
                         const nux_c8_t     *key,
                         void               *data);
    nux_status_t (*write)(nux_serde_writer_t *s,
                          const nux_c8_t     *key,
                          const void         *data);
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
    nux_rid_t               self;
    nux_arena_t            *arena;
    nux_ecs_bitset_t        bitset;
    nux_ecs_container_vec_t containers;
} nux_ecs_t;

typedef struct
{
    nux_u32_vec_t  includes;
    nux_u32_vec_t  excludes;
    nux_ecs_t     *ecs; // current ecs
    nux_ecs_mask_t mask;
    nux_u32_t      mask_index;
    nux_u32_t      mask_offset;
} nux_ecs_iter_t;

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
    nux_rid_t mesh;
    nux_rid_t texture;
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

typedef struct nux_ecs_module
{
    nux_ecs_t          *active;
    nux_ecs_t          *default_ecs;
    nux_ecs_component_t components[NUX_COMPONENT_MAX];
    nux_u32_t           components_max;
} nux_ecs_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// ecs.c

nux_status_t         nux_ecs_init(nux_ctx_t *ctx);
void                 nux_ecs_free(nux_ctx_t *ctx);
nux_ecs_component_t *nux_ecs_register_component(nux_ctx_t      *ctx,
                                                nux_u32_t       index,
                                                const nux_c8_t *name,
                                                nux_u32_t       size);

void *nux_ecs_add(nux_ctx_t *ctx, nux_eid_t e, nux_u32_t c);
void *nux_ecs_get(nux_ctx_t *ctx, nux_eid_t e, nux_u32_t c);

void nux_ecs_cleanup(nux_ctx_t *ctx, nux_rid_t rid);

// transform.c

nux_b32_t    nux_transform_update_matrix(nux_ctx_t *ctx, nux_eid_t e);
nux_status_t nux_transform_write(nux_serde_writer_t *s,
                                 const nux_c8_t     *key,
                                 const void         *data);

// serde.c

nux_status_t nux_ecs_write(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           nux_ecs_t          *ecs);
nux_status_t nux_ecs_read(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          nux_ecs_t          *ecs);

#endif
