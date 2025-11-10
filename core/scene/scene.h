#ifndef NUX_SCENE_H
#define NUX_SCENE_H

#include <serde/serde.h>

typedef enum
{
    NUX_COMPONENT_TRANSFORM  = 1,
    NUX_COMPONENT_CAMERA     = 2,
    NUX_COMPONENT_STATICMESH = 3,
    NUX_COMPONENT_RIGIDBODY  = 4,
    NUX_COMPONENT_COLLIDER   = 5,

    NUX_COMPONENT_MAX = 16,
} nux_component_type_;

typedef struct nux_scene nux_scene_t;
typedef struct nux_query nux_query_t;

typedef void (*nux_component_add_callback_t)(nux_nid_t nid, void *data);
typedef void (*nux_component_remove_callback_t)(nux_nid_t nid, void *data);
typedef nux_status_t (*nux_component_read_callback_t)(nux_serde_reader_t *s,
                                                      void               *data);
typedef nux_status_t (*nux_component_write_callback_t)(nux_serde_writer_t *s,
                                                       const void *data);

typedef struct
{
    const nux_c8_t                 *name;
    nux_u32_t                       size;
    nux_component_add_callback_t    add;
    nux_component_remove_callback_t remove;
    nux_component_read_callback_t   read;
    nux_component_write_callback_t  write;
} nux_component_info_t;

void  nux_component_register(nux_u32_t index, nux_component_info_t info);
void *nux_component_get(nux_nid_t e, nux_u32_t c);

nux_m4_t nux_transform_matrix(nux_nid_t e);
nux_v3_t nux_transform_local_translation(nux_nid_t e);
nux_q4_t nux_transform_local_rotation(nux_nid_t e);
nux_v3_t nux_transform_local_scale(nux_nid_t e);
nux_v3_t nux_transform_translation(nux_nid_t e);
nux_q4_t nux_transform_rotation(nux_nid_t e);
nux_v3_t nux_transform_scale(nux_nid_t e);
void     nux_transform_set_translation(nux_nid_t e, nux_v3_t position);
void     nux_transform_set_rotation(nux_nid_t e, nux_q4_t rotation);
void     nux_transform_set_rotation_euler(nux_nid_t e, nux_v3_t euler);
void     nux_transform_set_scale(nux_nid_t e, nux_v3_t scale);
void nux_transform_set_ortho(nux_nid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c);
nux_v3_t nux_transform_forward(nux_nid_t e);
nux_v3_t nux_transform_backward(nux_nid_t e);
nux_v3_t nux_transform_left(nux_nid_t e);
nux_v3_t nux_transform_right(nux_nid_t e);
nux_v3_t nux_transform_up(nux_nid_t e);
nux_v3_t nux_transform_down(nux_nid_t e);
void     nux_transform_rotate(nux_nid_t e, nux_v3_t axis, nux_f32_t angle);
void     nux_transform_rotate_x(nux_nid_t e, nux_f32_t angle);
void     nux_transform_rotate_y(nux_nid_t e, nux_f32_t angle);
void     nux_transform_rotate_z(nux_nid_t e, nux_f32_t angle);
void     nux_transform_look_at(nux_nid_t e, nux_v3_t center);

nux_query_t *nux_query_new(nux_arena_t *arena,
                           nux_u32_t    include_count,
                           nux_u32_t    exclude_count);
nux_query_t *nux_query_new_any(nux_arena_t *arena);
void         nux_query_includes(nux_query_t *it, nux_u32_t c);
void         nux_query_excludes(nux_query_t *it, nux_u32_t c);
nux_u32_t    nux_query_next(nux_query_t *it, nux_nid_t e);

nux_scene_t *nux_scene_new(nux_arena_t *arena);
nux_status_t nux_scene_set_active(nux_scene_t *scene);
nux_scene_t *nux_scene_active(void);
nux_u32_t    nux_scene_count(void);
void         nux_scene_clear(void);

nux_nid_t nux_node_create(nux_nid_t parent);
void      nux_node_delete(nux_nid_t e);
nux_b32_t nux_node_valid(nux_nid_t e);
nux_nid_t nux_node_root(void);
nux_nid_t nux_node_parent(nux_nid_t e);
void      nux_node_set_parent(nux_nid_t e, nux_nid_t p);
nux_nid_t nux_node_sibling(nux_nid_t e);
nux_nid_t nux_node_child(nux_nid_t e);
void      nux_node_add(nux_nid_t n, nux_u32_t c);
void      nux_node_remove(nux_nid_t e, nux_u32_t c);
nux_b32_t nux_node_has(nux_nid_t e, nux_u32_t c);
nux_nid_t nux_node_instantiate(nux_scene_t *scene, nux_nid_t parent);

nux_scene_t *nux_scene_load_gltf(nux_arena_t *arena, const nux_c8_t *path);

#endif
