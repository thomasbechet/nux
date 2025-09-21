#ifndef NUX_ECS_API_H
#define NUX_ECS_API_H

#include <base/api.h>

void nux_transform_add(nux_ctx_t *ctx, nux_eid_t e);
void nux_transform_remove(nux_ctx_t *ctx, nux_eid_t e);
void nux_transform_set_parent(nux_ctx_t *ctx, nux_eid_t e, nux_eid_t parent);
nux_eid_t nux_transform_get_parent(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t  nux_transform_get_local_translation(nux_ctx_t *ctx, nux_eid_t e);
nux_q4_t  nux_transform_get_local_rotation(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t  nux_transform_get_local_scale(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t  nux_transform_get_translation(nux_ctx_t *ctx, nux_eid_t e);
nux_q4_t  nux_transform_get_rotation(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t  nux_transform_get_scale(nux_ctx_t *ctx, nux_eid_t e);
void      nux_transform_set_translation(nux_ctx_t *ctx,
                                        nux_eid_t  e,
                                        nux_v3_t   position);
void nux_transform_set_rotation(nux_ctx_t *ctx, nux_eid_t e, nux_q4_t rotation);
void nux_transform_set_rotation_euler(nux_ctx_t *ctx,
                                      nux_eid_t  e,
                                      nux_v3_t   euler);
void nux_transform_set_scale(nux_ctx_t *ctx, nux_eid_t e, nux_v3_t scale);
void nux_transform_set_ortho(
    nux_ctx_t *ctx, nux_eid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c);
nux_v3_t nux_transform_forward(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t nux_transform_backward(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t nux_transform_left(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t nux_transform_right(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t nux_transform_up(nux_ctx_t *ctx, nux_eid_t e);
nux_v3_t nux_transform_down(nux_ctx_t *ctx, nux_eid_t e);
void     nux_transform_rotate(nux_ctx_t *ctx,
                              nux_eid_t  e,
                              nux_v3_t   axis,
                              nux_f32_t  angle);
void     nux_transform_rotate_x(nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle);
void     nux_transform_rotate_y(nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle);
void     nux_transform_rotate_z(nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle);
void     nux_transform_look_at(nux_ctx_t *ctx, nux_eid_t e, nux_v3_t center);

nux_rid_t nux_ecs_new_iter(nux_ctx_t *ctx,
                           nux_rid_t  arena,
                           nux_u32_t  include_count,
                           nux_u32_t  exclude_count);
nux_rid_t nux_ecs_new_iter_any(nux_ctx_t *ctx, nux_rid_t arena);
void      nux_ecs_includes(nux_ctx_t *ctx, nux_rid_t iter, nux_u32_t c);
void      nux_ecs_excludes(nux_ctx_t *ctx, nux_rid_t iter, nux_u32_t c);
nux_u32_t nux_ecs_next(nux_ctx_t *ctx, nux_rid_t iter, nux_eid_t e);

nux_rid_t    nux_ecs_new(nux_ctx_t *ctx, nux_rid_t arena);
nux_status_t nux_ecs_set_active(nux_ctx_t *ctx, nux_rid_t ecs);
nux_eid_t    nux_ecs_create(nux_ctx_t *ctx);
void         nux_ecs_create_at(nux_ctx_t *ctx, nux_eid_t e);
void         nux_ecs_delete(nux_ctx_t *ctx, nux_eid_t e);
nux_b32_t    nux_ecs_valid(nux_ctx_t *ctx, nux_eid_t e);
nux_u32_t    nux_ecs_count(nux_ctx_t *ctx);
void         nux_ecs_clear(nux_ctx_t *ctx);
void         nux_ecs_remove(nux_ctx_t *ctx, nux_eid_t e, nux_u32_t c);
nux_b32_t    nux_ecs_has(nux_ctx_t *ctx, nux_eid_t e, nux_u32_t c);

nux_rid_t nux_ecs_load_gltf(nux_ctx_t      *ctx,
                            nux_rid_t       arena,
                            const nux_c8_t *path);

#endif
