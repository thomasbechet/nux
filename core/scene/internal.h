#ifndef NUX_SCENE_INTERNAL_H
#define NUX_SCENE_INTERNAL_H

#include <scene/scene.h>

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

void nux_scene_cleanup(void *data);

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
