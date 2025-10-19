#ifndef NUX_SCENE_MODULE_H
#define NUX_SCENE_MODULE_H

#include <base/module.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_NODE_PER_MASK = 32,
} nux_scene_constants_t;

typedef nux_u32_t nux_scene_mask_t;
NUX_VEC_DEFINE(nux_scene_bitset, nux_scene_mask_t);
NUX_VEC_DEFINE(nux_scene_chunk_vec, void *);

typedef struct
{
    nux_nid_t parent;
    nux_nid_t next;
    nux_nid_t prev;
    nux_nid_t child;
} nux_node_t;

NUX_VEC_DEFINE(nux_node_vec, nux_node_t);

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       size;
    nux_status_t (*read)(nux_serde_reader_t *s, void *data);
    nux_status_t (*write)(nux_serde_writer_t *s, const void *data);
} nux_component_t;

typedef struct
{
    nux_u32_t             capa;
    nux_scene_chunk_vec_t chunks;
    nux_scene_bitset_t    bitset;
    nux_u32_t             component_size;
} nux_scene_container_t;

NUX_VEC_DEFINE(nux_scene_container_vec, nux_scene_container_t);

struct nux_scene
{
    nux_arena_t              *arena;
    nux_scene_bitset_t        bitset;
    nux_node_vec_t            nodes;
    nux_scene_container_vec_t containers;
    nux_nid_t                 root;
};

struct nux_query
{
    nux_u32_vec_t    includes;
    nux_u32_vec_t    excludes;
    nux_scene_t     *scene; // current scene
    nux_scene_mask_t mask;
    nux_u32_t        mask_index;
    nux_u32_t        mask_offset;
};

typedef struct
{
    nux_f32_t near;
    nux_f32_t far;
    nux_f32_t fov;
    nux_f32_t aspect; // 0 for no aspect ratio
    nux_b32_t ortho;
    nux_v2_t  ortho_size;
    nux_u32_t render_mask;
} nux_camera_t;

typedef struct
{
    nux_v3_t  translation;
    nux_q4_t  rotation;
    nux_v3_t  scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

typedef struct
{
    nux_rid_t mesh;
    nux_rid_t texture;
    nux_u32_t transform;
    nux_u32_t render_layer;
    nux_b32_t draw_bounds;
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

typedef struct
{
    nux_scene_t        *scene;
    nux_serde_writer_t *output;
    nux_serde_writer_t  writer;
    nux_u32_t          *node_map;
    nux_u32_t           node_count;
} nux_scene_writer_t;

typedef struct
{
    nux_scene_t        *scene;
    nux_serde_reader_t *input;
    nux_serde_reader_t  reader;
    nux_u32_t          *node_map;
    nux_u32_t           node_count;
} nux_scene_reader_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// scene.c

nux_component_t *nux_component_register(nux_u32_t       index,
                                        const nux_c8_t *name,
                                        nux_u32_t       size);

void *nux_component_add(nux_nid_t e, nux_u32_t c);
void *nux_component_get(nux_nid_t e, nux_u32_t c);

void nux_scene_cleanup(void *data);

// transform.c

nux_status_t nux_transform_write(nux_serde_writer_t *s, const void *data);
nux_status_t nux_transform_read(nux_serde_reader_t *s, void *data);

// serde.c

nux_status_t nux_scene_writer_init(nux_scene_writer_t *s,
                                   nux_serde_writer_t *output,
                                   nux_scene_t        *scene);
nux_status_t nux_scene_reader_init(nux_scene_reader_t *s,
                                   nux_serde_reader_t *input,
                                   nux_scene_t        *scene);

nux_status_t nux_scene_write(nux_serde_writer_t *s,
                             const nux_c8_t     *key,
                             nux_scene_t        *scene);
nux_status_t nux_scene_read(nux_serde_reader_t *s,
                            const nux_c8_t     *key,
                            nux_scene_t        *scene);

#endif
