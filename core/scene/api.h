#ifndef NUX_SCENE_API_H
#define NUX_SCENE_API_H

#include <base/api.h>

typedef struct nux_scene nux_scene_t;
typedef struct nux_query nux_query_t;

void     nux_transform_add(nux_nid_t e);
void     nux_transform_remove(nux_nid_t e);
nux_v3_t nux_transform_get_local_translation(nux_nid_t e);
nux_q4_t nux_transform_get_local_rotation(nux_nid_t e);
nux_v3_t nux_transform_get_local_scale(nux_nid_t e);
nux_v3_t nux_transform_get_translation(nux_nid_t e);
nux_q4_t nux_transform_get_rotation(nux_nid_t e);
nux_v3_t nux_transform_get_scale(nux_nid_t e);
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
nux_nid_t    nux_node_create(nux_nid_t parent);
void         nux_node_delete(nux_nid_t e);
nux_b32_t    nux_node_valid(nux_nid_t e);
nux_nid_t    nux_node_root(void);
nux_nid_t    nux_node_parent(nux_nid_t e);
void         nux_node_set_parent(nux_nid_t e, nux_nid_t p);
nux_nid_t    nux_node_sibling(nux_nid_t e);
nux_nid_t    nux_node_child(nux_nid_t e);
nux_u32_t    nux_scene_count(void);
void         nux_scene_clear(void);
void         nux_node_remove(nux_nid_t e, nux_u32_t c);
nux_b32_t    nux_node_has(nux_nid_t e, nux_u32_t c);
nux_status_t nux_node_instantiate(nux_scene_t *scene);

nux_scene_t *nux_scene_load_gltf(nux_arena_t *arena, const nux_c8_t *path);

#endif
