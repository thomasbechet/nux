#include "nux_internal.h"

NUX_VEC_IMPL(nux_point_mass_vec, nux_point_mass_t);
NUX_VEC_IMPL(nux_collision_constraint_vec, nux_collision_constraint_t);
NUX_VEC_IMPL(nux_distance_constraint_vec, nux_distance_constraint_t);

static nux_v3_t
compute_sum_forces (nux_point_mass_t *pm)
{
    return nux_v3(0, -9.81, 0);
}
static void
integrate (nux_ctx_t *ctx)
{
    nux_f32_t dt = nux_time_delta(ctx);

    const nux_u32_t substep = 10;
    nux_f32_t       subdt   = dt / substep;
    for (nux_u32_t n = 0; n < substep; ++n)
    {
        // (7) integrate position (TODO: integrate rotation)
        for (nux_u32_t i = 0; i < ctx->point_masses.size; ++i)
        {
            nux_point_mass_t *pm        = ctx->point_masses.data + i;
            nux_v3_t          sum_force = compute_sum_forces(pm);

            // keep previous position
            pm->p = pm->x;

            // integrate position
            pm->x = nux_v3_add(pm->x, nux_v3_muls(pm->v, subdt));
            pm->x = nux_v3_add(pm->x,
                               nux_v3_muls(sum_force, subdt * subdt * pm->w));
        }

        // (8) generate collision constraints
        nux_collision_constraint_vec_clear(&ctx->collision_constraints);
        for (nux_u32_t i = 0; i < ctx->point_masses.size; ++i)
        {
            nux_point_mass_t *pm = ctx->point_masses.data + i;

            // ground collision
            const nux_f32_t ground = 0;
            if (pm->x.y < ground)
            {
                nux_collision_constraint_t *c
                    = nux_collision_constraint_vec_push(
                        &ctx->collision_constraints);
                NUX_ASSERT(c);
                c->q = nux_v3(pm->x.x, ground, pm->x.z);
                c->n = NUX_V3_UP;
                c->a = i;
            }

            // // box collision
            // for (nux_u32_t b = 0; b < NUX_ARRAY_SIZE(g_boxes); ++b)
            // {
            //     nu_b3_t box = g_boxes[b];
            //     if (nu_b3_contains(box, pm->x))
            //     {
            //         nux_v3_t rel = pm->x;
            //
            //         // Left
            //         nux_f32_t d, maxd = NU_FLT_MAX;
            //         nux_v3_t  q, n;
            //         q = n = NU_V3_ZEROS;
            //
            //         d = box.max.x - rel.x;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(box.max.x, rel.y, rel.z);
            //             n    = NU_V3_RIGHT;
            //         }
            //         d = rel.x - box.min.x;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(box.min.x, rel.y, rel.z);
            //             n    = NU_V3_LEFT;
            //         }
            //
            //         d = box.max.y - rel.y;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(rel.x, box.max.y, rel.z);
            //             n    = NU_V3_UP;
            //         }
            //         d = rel.y - box.min.y;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(rel.x, box.min.y, rel.z);
            //             n    = NU_V3_DOWN;
            //         }
            //
            //         d = box.max.z - rel.z;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(rel.x, rel.y, box.max.z);
            //             n    = NU_V3_BACKWARD;
            //         }
            //         d = rel.z - box.min.z;
            //         if (d < maxd)
            //         {
            //             maxd = d;
            //             q    = nux_v3(rel.x, rel.y, box.min.z);
            //             n    = NU_V3_FORWARD;
            //         }
            //
            //         collision_constraint_t *c
            //             = NU_VEC_PUSH(&g_ctx.collision_constraints);
            //         NU_ASSERT(c);
            //         c->q = q;
            //         c->n = n;
            //         c->a = i;
            //     }
            // }
        }

        // (9) solve constraints
        // solve collision constraints
        for (nux_u32_t i = 0; i < ctx->collision_constraints.size; ++i)
        {
            nux_collision_constraint_t *c = ctx->collision_constraints.data + i;
            nux_point_mass_t           *a = ctx->point_masses.data + c->a;
            nux_v3_t                    v = nux_v3_sub(a->x, c->q);
            nux_f32_t                   depth = -nux_v3_dot(v, c->n);
            if (depth > 0)
            {
                a->x = nux_v3_add(a->x, nux_v3_muls(c->n, depth));
            }
        }
        // solve distance constraints
        for (nux_u32_t i = 0; i < ctx->distance_constraints.size; ++i)
        {
            nux_distance_constraint_t *c = ctx->distance_constraints.data + i;
            nux_point_mass_t          *a = ctx->point_masses.data + c->a;
            nux_point_mass_t          *b = ctx->point_masses.data + c->b;

            nux_v3_t  delta          = nux_v3_sub(b->x, a->x);
            nux_f32_t distance       = nux_v3_norm(delta);
            nux_v3_t  required_delta = nux_v3_muls(delta, c->d / distance);

            nux_v3_t offset = nux_v3_sub(required_delta, delta);

            nux_v3_t ca = nux_v3_muls(offset, -0.5);
            nux_v3_t cb = nux_v3_muls(offset, 0.5);

            a->x = nux_v3_add(a->x, ca);
            b->x = nux_v3_add(b->x, cb);
        }
        // (12) compute new velocity
        for (nux_u32_t i = 0; i < ctx->point_masses.size; ++i)
        {
            nux_point_mass_t *pm = ctx->point_masses.data + i;
            pm->v                = nux_v3_divs(nux_v3_sub(pm->x, pm->p), subdt);
        }
        // (16) solve velocities
        for (nux_u32_t i = 0; i < ctx->collision_constraints.size; ++i)
        {
            // const nux_f32_t elasticity = 0.1;
            const nux_f32_t elasticity = 3;
            const nux_f32_t friction   = 30;

            nux_collision_constraint_t *c = ctx->collision_constraints.data + i;
            nux_point_mass_t           *pm = ctx->point_masses.data + c->a;

            nux_v3_t vn = nux_v3_muls(c->n, nux_v3_dot(c->n, pm->v));
            nux_v3_t vt = nux_v3_sub(pm->v, vn);
            vn          = nux_v3_muls(vn, -elasticity);
            vt          = nux_v3_muls(vt, nux_exp(-friction * subdt));
            pm->v       = nux_v3_add(vn, vt);
        }
    }
}
void
pm_to_transforms (nux_ctx_t *ctx)
{
    nux_ent_t e = nux_ecs_begin(ctx, ctx->rigidbody_transform_iter);
    while (e)
    {
        nux_transform_t *transform
            = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
        nux_rigidbody_t  *body = nux_ecs_get(ctx, e, NUX_COMPONENT_RIGIDBODY);
        nux_point_mass_t *pm   = &ctx->point_masses.data[body->first];
        nux_v3_t          v    = pm->v;
        transform->local_translation = pm->x;
        transform->dirty             = NUX_TRUE;
        e = nux_ecs_next(ctx, ctx->rigidbody_transform_iter);
    }
}

nux_status_t
nux_physics_init (nux_ctx_t *ctx)
{
    const nux_u32_t count = 1024 * 10;

    NUX_CHECK(nux_point_mass_vec_alloc(
                  ctx, &ctx->core_arena, count, &ctx->point_masses),
              return NUX_FAILURE);
    NUX_CHECK(nux_collision_constraint_vec_alloc(
                  ctx, &ctx->core_arena, count, &ctx->collision_constraints),
              return NUX_FAILURE);
    NUX_CHECK(nux_distance_constraint_vec_alloc(
                  ctx, &ctx->core_arena, count, &ctx->distance_constraints),
              return NUX_FAILURE);

    nux_lua_open_physics(ctx);

    ctx->rigidbody_transform_iter
        = nux_ecs_new_iter(ctx, ctx->core_arena_res, 2, 0);
    NUX_CHECK(ctx->rigidbody_transform_iter, return NUX_FAILURE);
    nux_ecs_includes(
        ctx, ctx->rigidbody_transform_iter, NUX_COMPONENT_RIGIDBODY);
    nux_ecs_includes(
        ctx, ctx->rigidbody_transform_iter, NUX_COMPONENT_TRANSFORM);

    return NUX_SUCCESS;
}
void
nux_physics_free (nux_ctx_t *ctx)
{
}
void
nux_physics_update (nux_ctx_t *ctx)
{
    integrate(ctx);
    pm_to_transforms(ctx);
}
nux_u32_t
nux_physics_add_pm (nux_ctx_t *ctx, nux_v3_t pos, nux_v3_t vel)
{
    nux_point_mass_t *pm = nux_point_mass_vec_push(&ctx->point_masses);
    NUX_ASSERT(pm);
    pm->x = pos;
    pm->v = vel;
    pm->f = NUX_V3_ZEROS;
    pm->m = 1;
    pm->w = 1.0 / pm->m;
    pm->p = NUX_V3_ZEROS;
    return ctx->point_masses.size - 1;
}
void
nux_physics_add_distance_constraint (nux_ctx_t *ctx,
                                     nux_u32_t  a,
                                     nux_u32_t  b,
                                     float      distance)
{
    nux_distance_constraint_t *c
        = nux_distance_constraint_vec_push(&ctx->distance_constraints);
    NUX_ASSERT(c);
    c->a = a;
    c->b = b;
    c->d = distance;
}
