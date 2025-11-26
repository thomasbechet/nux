#ifndef NUX_SCENE_INTERNAL_H
#define NUX_SCENE_INTERNAL_H

#include <scene/scene.h>

typedef enum
{
    NUX_COMPONENT_MAX = 32
} nux_scene_constants_t;

typedef struct nux_transform_t
{
    nux_v3_t  translation;
    nux_q4_t  rotation;
    nux_v3_t  scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

typedef struct nux_node_t
{
    nux_scene_t *scene;
    nux_id_t     parent;
    nux_id_t     next;
    nux_id_t     prev;
    nux_id_t     child;
    nux_id_t     components[NUX_COMPONENT_MAX];
} nux_node_t;

typedef struct nux_scene_t
{
    nux_node_t *root;
} nux_scene_t;

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
    nux_b32_t            used;
    nux_component_info_t info;
    nux_u32_t            index;
} nux_component_type_t;

typedef struct
{
    nux_scene_t         *active_scene;
    nux_scene_t         *default_scene;
    nux_u32_t            component_count;
    nux_component_type_t components[NUX_OBJECT_MAX];
} nux_scene_module_t;

void nux_scene_module_register(void);

void nux_scene_cleanup(void *data);

nux_component_type_t *nux_component_type(nux_u32_t type);

void         nux_transform_init(nux_node_t *node);
nux_status_t nux_transform_write(nux_serde_writer_t *s, const void *data);
nux_status_t nux_transform_read(nux_serde_reader_t *s, void *data);

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
