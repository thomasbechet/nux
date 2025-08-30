#include "internal.h"

void
nux_rigidbody_add (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_rigidbody_t *body = nux_ecs_add(ctx, e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);
    body->first = 0;
    body->count = 0;
    if (nux_ecs_has(ctx, e, NUX_COMPONENT_COLLIDER))
    {
        nux_physics_add_rigidbody(ctx, e);
    }
}
void
nux_rigidbody_remove (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_ecs_remove(ctx, e, NUX_COMPONENT_RIGIDBODY);
}
void
nux_rigidbody_set_velocity (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t velocity)
{
    nux_rigidbody_t *body = nux_ecs_get(ctx, e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);
    for (nux_u32_t i = 0; i < body->count; ++i)
    {
        nux_point_mass_t *pm
            = ctx->physics->point_masses.data + body->first + i;
        pm->v = velocity;
    }
}
