#include "internal.h"

void
nux_collider_add_sphere (nux_ctx_t *ctx, nux_eid_t e, nux_f32_t radius)
{
    nux_collider_t *collider = nux_ecs_add(ctx, e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type          = NUX_COLLIDER_SPHERE;
    collider->sphere.radius = radius;
}
void
nux_collider_add_aabb (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t min, nux_v3_t max)
{
    nux_collider_t *collider = nux_ecs_add(ctx, e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type     = NUX_COLLIDER_AABB;
    collider->aabb.box = nux_b3(min, max);
    if (nux_ecs_has(ctx, e, NUX_COMPONENT_RIGIDBODY))
    {
        nux_physics_add_rigidbody(ctx, e);
    }
}
void
nux_collider_remove (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_ecs_remove(ctx, e, NUX_COMPONENT_COLLIDER);
}
