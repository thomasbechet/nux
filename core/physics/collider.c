#include "nux_internal.h"

void
nux_collider_add_sphere (nux_ctx_t *ctx, nux_ent_t e, nux_f32_t radius)
{
    nux_collider_t *collider = nux_ecs_set(ctx, e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type          = NUX_COLLIDER_SPHERE;
    collider->sphere.radius = radius;
}
void
nux_collider_add_aabb (nux_ctx_t *ctx, nux_ent_t e, nux_v3_t min, nux_v3_t max)
{
    nux_collider_t *collider = nux_ecs_set(ctx, e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type     = NUX_COLLIDER_AABB;
    collider->aabb.box = nux_b3(min, max);
}
void
nux_collider_remove (nux_ctx_t *ctx, nux_ent_t e)
{
    nux_ecs_unset(ctx, e, NUX_COMPONENT_COLLIDER);
}
