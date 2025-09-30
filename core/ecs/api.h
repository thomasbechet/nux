#ifndef NUX_ECS_API_H
#define NUX_ECS_API_H

#include <base/api.h>

typedef struct nux_ecs_t      nux_ecs_t;
typedef struct nux_ecs_iter_t nux_ecs_iter_t;

void     nux_transform_add(nux_eid_t e);
void     nux_transform_remove(nux_eid_t e);
nux_v3_t nux_transform_get_local_translation(nux_eid_t e);
nux_q4_t nux_transform_get_local_rotation(nux_eid_t e);
nux_v3_t nux_transform_get_local_scale(nux_eid_t e);
nux_v3_t nux_transform_get_translation(nux_eid_t e);
nux_q4_t nux_transform_get_rotation(nux_eid_t e);
nux_v3_t nux_transform_get_scale(nux_eid_t e);
void     nux_transform_set_translation(nux_eid_t e, nux_v3_t position);
void     nux_transform_set_rotation(nux_eid_t e, nux_q4_t rotation);
void     nux_transform_set_rotation_euler(nux_eid_t e, nux_v3_t euler);
void     nux_transform_set_scale(nux_eid_t e, nux_v3_t scale);
void nux_transform_set_ortho(nux_eid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c);
nux_v3_t nux_transform_forward(nux_eid_t e);
nux_v3_t nux_transform_backward(nux_eid_t e);
nux_v3_t nux_transform_left(nux_eid_t e);
nux_v3_t nux_transform_right(nux_eid_t e);
nux_v3_t nux_transform_up(nux_eid_t e);
nux_v3_t nux_transform_down(nux_eid_t e);
void     nux_transform_rotate(nux_eid_t e, nux_v3_t axis, nux_f32_t angle);
void     nux_transform_rotate_x(nux_eid_t e, nux_f32_t angle);
void     nux_transform_rotate_y(nux_eid_t e, nux_f32_t angle);
void     nux_transform_rotate_z(nux_eid_t e, nux_f32_t angle);
void     nux_transform_look_at(nux_eid_t e, nux_v3_t center);

nux_ecs_iter_t *nux_ecs_new_iter(nux_arena_t *arena,
                                 nux_u32_t    include_count,
                                 nux_u32_t    exclude_count);
nux_ecs_iter_t *nux_ecs_new_iter_any(nux_arena_t *arena);
void            nux_ecs_includes(nux_ecs_iter_t *it, nux_u32_t c);
void            nux_ecs_excludes(nux_ecs_iter_t *it, nux_u32_t c);
nux_u32_t       nux_ecs_next(nux_ecs_iter_t *it, nux_eid_t e);

nux_ecs_t   *nux_ecs_new(nux_arena_t *arena);
nux_status_t nux_ecs_set_active(nux_ecs_t *ecs);
nux_ecs_t   *nux_ecs_active(void);
nux_eid_t    nux_ecs_create(nux_eid_t parent);
void         nux_ecs_delete(nux_eid_t e);
nux_b32_t    nux_ecs_valid(nux_eid_t e);
nux_eid_t    nux_ecs_root(void);
nux_eid_t    nux_ecs_parent(nux_eid_t e);
void         nux_ecs_set_parent(nux_eid_t e, nux_eid_t p);
nux_eid_t    nux_ecs_sibling(nux_eid_t e);
nux_eid_t    nux_ecs_child(nux_eid_t e);
nux_u32_t    nux_ecs_count(void);
void         nux_ecs_clear(void);
void         nux_ecs_remove(nux_eid_t e, nux_u32_t c);
nux_b32_t    nux_ecs_has(nux_eid_t e, nux_u32_t c);
nux_status_t nux_ecs_instantiate(nux_ecs_t *ecs);

nux_ecs_t *nux_ecs_load_gltf(nux_arena_t *arena, const nux_c8_t *path);

#endif
