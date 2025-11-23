#ifndef NUX_SCENE_INTERNAL_H
#define NUX_SCENE_INTERNAL_H

#include <scene/scene.h>

#define nux_nid_index(id)   (id - 1)
#define nux_nid_make(index) (index + 1)

typedef enum
{
    NUX_NODE_PER_MASK = 32,
} nux_scene_constants_t;

typedef nux_u32_t nux_scene_mask_t;
typedef nux_vec(nux_scene_mask_t) nux_scene_bitset_t;

typedef enum
{
    NUX_NODE_TRANSFORM  = 0,
    NUX_NODE_STATICMESH = 1,
    NUX_NODE_CAMERA     = 2,
    NUX_NODE_POINTLIGHT = 3,
} nux_node_type_t;

typedef struct nux_node_t
{
    nux_node_type_t type;
    nux_id_t        parent;
    nux_id_t        next;
    nux_id_t        prev;
    nux_id_t        child;
} nux_node_t;

typedef struct
{
    nux_u32_t capa;
    nux_vec(void *) chunks;
    nux_scene_bitset_t bitset;
    nux_u32_t          component_size;
} nux_scene_container_t;

struct nux_scene
{
    nux_arena_t       *arena;
    nux_scene_bitset_t bitset;
    nux_vec(nux_node_t) nodes;
    nux_vec(nux_scene_container_t) containers;
    nux_id_t root;
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
    nux_v3_t  translation;
    nux_q4_t  rotation;
    nux_v3_t  scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

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

typedef struct
{
    nux_component_info_t info;
} nux_component_t;

typedef struct
{
    nux_scene_t    *active;
    nux_scene_t    *default_scene;
    nux_component_t components[NUX_COMPONENT_MAX];
    nux_u32_t       components_max;
    nux_query_t    *iter_any;
} nux_scene_module_t;

void nux_scene_cleanup(void *data);

void         nux_transform_add(nux_id_t id, void *data);
nux_status_t nux_transform_write(nux_serde_writer_t *s, const void *data);
nux_status_t nux_transform_read(nux_serde_reader_t *s, void *data);

void                nux_scene_module_register(void);
nux_scene_module_t *nux_scene_module(void);

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
