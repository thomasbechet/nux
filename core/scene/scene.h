#ifndef NUX_SCENE_H
#define NUX_SCENE_H

#include <serde/serde.h>

typedef struct nux_scene_t     nux_scene_t;
typedef struct nux_node_t      nux_node_t;
typedef struct nux_transform_t nux_transform_t;

typedef void (*nux_component_add_callback_t)(void *data, nux_node_t *node);
typedef void (*nux_component_remove_callback_t)(void *data, nux_node_t *node);

typedef struct
{
    nux_component_add_callback_t    add;
    nux_component_remove_callback_t remove;
} nux_component_info_t;

void nux_component_register(nux_u32_t type, nux_component_info_t info);

nux_node_t *nux_node_new(nux_node_t *parent);
void       *nux_node_get(nux_node_t *node, nux_u32_t type);
void       *nux_node_add(nux_node_t *node, nux_u32_t type);
void        nux_node_remove(nux_node_t *node, nux_u32_t type);
void       *nux_node_get(nux_node_t *node, nux_u32_t type);

nux_m4_t nux_transform_matrix(nux_transform_t *transform);
nux_v3_t nux_transform_local_translation(nux_transform_t *transform);
nux_q4_t nux_transform_local_rotation(nux_transform_t *transform);
nux_v3_t nux_transform_local_scale(nux_transform_t *transform);
nux_v3_t nux_transform_translation(nux_transform_t *transform);
nux_q4_t nux_transform_rotation(nux_transform_t *transform);
nux_v3_t nux_transform_scale(nux_transform_t *transform);
void     nux_transform_set_translation(nux_transform_t *transform,
                                       nux_v3_t         position);
void nux_transform_set_rotation(nux_transform_t *transform, nux_q4_t rotation);
void nux_transform_set_rotation_euler(nux_transform_t *transform,
                                      nux_v3_t         euler);
void nux_transform_set_scale(nux_transform_t *transform, nux_v3_t scale);
void nux_transform_set_ortho(nux_transform_t *transform,
                             nux_v3_t         a,
                             nux_v3_t         b,
                             nux_v3_t         c);
nux_v3_t nux_transform_forward(nux_transform_t *transform);
nux_v3_t nux_transform_backward(nux_transform_t *transform);
nux_v3_t nux_transform_left(nux_transform_t *transform);
nux_v3_t nux_transform_right(nux_transform_t *transform);
nux_v3_t nux_transform_up(nux_transform_t *transform);
nux_v3_t nux_transform_down(nux_transform_t *transform);
void     nux_transform_rotate(nux_transform_t *transform,
                              nux_v3_t         axis,
                              nux_f32_t        angle);
void     nux_transform_rotate_x(nux_transform_t *transform, nux_f32_t angle);
void     nux_transform_rotate_y(nux_transform_t *transform, nux_f32_t angle);
void     nux_transform_rotate_z(nux_transform_t *transform, nux_f32_t angle);
void     nux_transform_look_at(nux_transform_t *transform, nux_v3_t center);

nux_scene_t *nux_scene_new(nux_arena_t *arena);
nux_status_t nux_scene_set_active(nux_scene_t *scene);
nux_scene_t *nux_scene_active(void);
nux_u32_t    nux_scene_count(void);
void         nux_scene_clear(void);

nux_node_t *nux_node_root(void);
nux_node_t *nux_node_parent(nux_node_t *node);
void        nux_node_set_parent(nux_node_t *node, nux_node_t *parent);
nux_node_t *nux_node_sibling(nux_node_t *node);
nux_node_t *nux_node_child(nux_node_t *node);
nux_node_t *nux_node_instantiate(nux_scene_t *scene, nux_node_t *parent);

nux_scene_t *nux_scene_load_gltf(nux_arena_t *arena, const nux_c8_t *path);

#endif
