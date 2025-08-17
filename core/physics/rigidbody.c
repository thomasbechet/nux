#include "nux_internal.h"

void
nux_rigidbody_add (nux_ctx_t *ctx, nux_ent_t e)
{
    nux_rigidbody_t *body = nux_ecs_set(ctx, e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);

    nux_v3_t    pos = nux_v3(0, 10, 0);
    nux_v3_t    vel = NUX_V3_ZEROS;
    const float s   = 1.0;
    const float h   = s / 2;
    const float s2  = nux_sqrt(s + s);

    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    if (t)
    {
        nux_transform_update_matrix(ctx, e);
        pos = t->local_translation;
    }
    const float s3 = nux_sqrt(s + s + s);

    nux_u32_t p0
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(-h, h, h)), vel);
    nux_u32_t p1
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(-h, h, -h)), vel);
    nux_u32_t p2
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(h, h, -h)), vel);
    nux_u32_t p3
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(h, h, h)), vel);
    nux_u32_t p4
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(-h, -h, h)), vel);
    nux_u32_t p5
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(-h, -h, -h)), vel);
    nux_u32_t p6
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(h, -h, -h)), vel);
    nux_u32_t p7
        = nux_physics_add_pm(ctx, nux_v3_add(pos, nux_v3(h, -h, h)), vel);

    nux_physics_add_distance_constraint(ctx, p0, p1, s);
    nux_physics_add_distance_constraint(ctx, p1, p2, s);
    nux_physics_add_distance_constraint(ctx, p2, p3, s);
    nux_physics_add_distance_constraint(ctx, p3, p0, s);

    nux_physics_add_distance_constraint(ctx, p4, p5, s);
    nux_physics_add_distance_constraint(ctx, p5, p6, s);
    nux_physics_add_distance_constraint(ctx, p6, p7, s);
    nux_physics_add_distance_constraint(ctx, p7, p4, s);

    nux_physics_add_distance_constraint(ctx, p0, p4, s);
    nux_physics_add_distance_constraint(ctx, p1, p5, s);
    nux_physics_add_distance_constraint(ctx, p2, p6, s);
    nux_physics_add_distance_constraint(ctx, p3, p7, s);

    nux_physics_add_distance_constraint(ctx, p0, p2, s2);
    // nux_physics_add_distance_constraictx, nt(p1, p3, s2);

    nux_physics_add_distance_constraint(ctx, p4, p6, s2);
    // nux_physics_add_distance_constraictx, nt(p5, p7, s2);

    nux_physics_add_distance_constraint(ctx, p0, p7, s2);
    // nux_physics_add_distance_constraictx, nt(p4, p3, s2);

    nux_physics_add_distance_constraint(ctx, p5, p2, s2);
    // nux_physics_add_distance_constraictx, nt(p1, p6, s2);

    nux_physics_add_distance_constraint(ctx, p4, p1, s2);
    // nux_physics_add_distance_constraictx, nt(p0, p5, s2);

    nux_physics_add_distance_constraint(ctx, p3, p6, s2);
    // nux_physics_add_distance_constraictx, nt(p7, p2, s2);

    nux_physics_add_distance_constraint(ctx, p4, p2, s3);
    nux_physics_add_distance_constraint(ctx, p0, p6, s3);
    nux_physics_add_distance_constraint(ctx, p7, p1, s3);
    nux_physics_add_distance_constraint(ctx, p3, p5, s3);

    body->first = p0;
    body->count = 8;
}
void
nux_rigidbody_remove (nux_ctx_t *ctx, nux_ent_t e)
{
    nux_ecs_unset(ctx, e, NUX_COMPONENT_RIGIDBODY);
}
void
nux_rigidbody_set_velocity (nux_ctx_t *ctx, nux_ent_t e, nux_v3_t velocity)
{
    nux_rigidbody_t *body = nux_ecs_get(ctx, e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);
    for (nux_u32_t i = 0; i < body->count; ++i)
    {
        nux_point_mass_t *pm = ctx->point_masses.data + body->first + i;
        pm->v                = velocity;
    }
}
