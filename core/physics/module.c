#include "internal.h"

static nux_physics_module_t _module;

NUX_VEC_IMPL(nux_point_mass_vec, nux_point_mass_t);
NUX_VEC_IMPL(nux_collision_constraint_vec, nux_collision_constraint_t);
NUX_VEC_IMPL(nux_distance_constraint_vec, nux_distance_constraint_t);

static nux_v3_t
compute_sum_forces (nux_point_mass_t *pm)
{
    return nux_v3(0, -9.81, 0);
}
static void
integrate (void)
{
    nux_f32_t dt = nux_time_delta();

    const nux_u32_t substep = 10;
    nux_f32_t       subdt   = dt / substep;
    for (nux_u32_t n = 0; n < substep; ++n)
    {
        // (7) integrate position (TODO: integrate rotation)
        for (nux_u32_t i = 0; i < _module.point_masses.size; ++i)
        {
            nux_point_mass_t *pm        = _module.point_masses.data + i;
            nux_v3_t          sum_force = compute_sum_forces(pm);

            // keep previous position
            pm->p = pm->x;

            // integrate position
            pm->x = nux_v3_add(pm->x, nux_v3_muls(pm->v, subdt));
            pm->x = nux_v3_add(pm->x,
                               nux_v3_muls(sum_force, subdt * subdt * pm->w));
        }

        // (8) generate collision constraints
        nux_collision_constraint_vec_clear(&_module.collision_constraints);
        for (nux_u32_t i = 0; i < _module.point_masses.size; ++i)
        {
            nux_point_mass_t *pm = _module.point_masses.data + i;

            // ground collision
            if (pm->x.y < _module.ground_height)
            {
                nux_collision_constraint_t *c
                    = nux_collision_constraint_vec_push(
                        &_module.collision_constraints);
                NUX_ASSERT(c);
                c->q = nux_v3(pm->x.x, _module.ground_height, pm->x.z);
                c->n = NUX_V3_UP;
                c->a = i;
            }
        }

        // (9) solve constraints
        // solve collision constraints
        for (nux_u32_t i = 0; i < _module.collision_constraints.size; ++i)
        {
            nux_collision_constraint_t *c
                = _module.collision_constraints.data + i;
            nux_point_mass_t *a     = _module.point_masses.data + c->a;
            nux_v3_t          v     = nux_v3_sub(a->x, c->q);
            nux_f32_t         depth = -nux_v3_dot(v, c->n);
            if (depth > 0)
            {
                a->x = nux_v3_add(a->x, nux_v3_muls(c->n, depth));
            }
        }
        // solve distance constraints
        for (nux_u32_t i = 0; i < _module.distance_constraints.size; ++i)
        {
            nux_distance_constraint_t *c
                = _module.distance_constraints.data + i;
            nux_point_mass_t *a = _module.point_masses.data + c->a;
            nux_point_mass_t *b = _module.point_masses.data + c->b;

            nux_v3_t  delta    = nux_v3_sub(b->x, a->x);
            nux_f32_t distance = nux_v3_norm(delta);
            if (distance > NUX_F32_EPSILON)
            {
                nux_v3_t required_delta = nux_v3_muls(delta, c->d / distance);

                nux_v3_t offset = nux_v3_sub(required_delta, delta);

                nux_v3_t ca = nux_v3_muls(offset, -0.5);
                nux_v3_t cb = nux_v3_muls(offset, 0.5);

                a->x = nux_v3_add(a->x, ca);
                b->x = nux_v3_add(b->x, cb);
            }
        }
        // (12) compute new velocity
        for (nux_u32_t i = 0; i < _module.point_masses.size; ++i)
        {
            nux_point_mass_t *pm = _module.point_masses.data + i;
            pm->v                = nux_v3_divs(nux_v3_sub(pm->x, pm->p), subdt);
        }
        // (16) solve velocities
        for (nux_u32_t i = 0; i < _module.collision_constraints.size; ++i)
        {
            const nux_f32_t elasticity = 0.1;
            // const nux_f32_t elasticity = 2;
            const nux_f32_t friction = 60;

            nux_collision_constraint_t *c
                = _module.collision_constraints.data + i;
            nux_point_mass_t *pm = _module.point_masses.data + c->a;

            nux_v3_t vn = nux_v3_muls(c->n, nux_v3_dot(c->n, pm->v));
            nux_v3_t vt = nux_v3_sub(pm->v, vn);
            vn          = nux_v3_muls(vn, -elasticity);
            vt          = nux_v3_muls(vt, nux_exp(-friction * subdt));
            pm->v       = nux_v3_add(vn, vt);
        }
    }
}
static void
compute_transforms (void)
{
    nux_nid_t it = NUX_NULL;
    while ((it = nux_query_next(_module.rigidbody_transform_iter, it)))
    {
        nux_rigidbody_t *body = nux_component_get(it, NUX_COMPONENT_RIGIDBODY);
        NUX_ASSERT(body);
        nux_point_mass_t *pm = &_module.point_masses.data[body->first];

        nux_v3_t a = _module.point_masses.data[body->first + 0].x;
        nux_v3_t b = _module.point_masses.data[body->first + 4].x;
        nux_v3_t c = _module.point_masses.data[body->first + 1].x;
        nux_transform_set_ortho(it, a, b, c);

        // nux_v3_t positions[8];
        // for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(positions); ++i)
        // {
        //     positions[i] = _module.point_masses.data[body->first + i].x;
        // }
        // nux_renderer_draw_rect(ctx, positions);
    }
}

static nux_status_t
module_init (void)
{
    // Register components
    nux_component_t *comp;
    comp = nux_component_register(
        NUX_COMPONENT_RIGIDBODY, "rigidbody", sizeof(nux_rigidbody_t));
    comp->add = nux_rigidbody_add;
    comp      = nux_component_register(
        NUX_COMPONENT_COLLIDER, "collider", sizeof(nux_collider_t));
    comp->add = nux_collider_add;

    // Initialize values
    nux_arena_t *a = nux_arena_core();
    NUX_CHECK(nux_point_mass_vec_init(a, &_module.point_masses),
              return NUX_FAILURE);
    NUX_CHECK(
        nux_collision_constraint_vec_init(a, &_module.collision_constraints),
        return NUX_FAILURE);
    NUX_CHECK(
        nux_distance_constraint_vec_init(a, &_module.distance_constraints),
        return NUX_FAILURE);

    nux_arena_t *arena               = nux_arena_core();
    _module.rigidbody_transform_iter = nux_query_new(arena, 2, 0);
    NUX_CHECK(_module.rigidbody_transform_iter, return NUX_FAILURE);
    nux_query_includes(_module.rigidbody_transform_iter,
                       NUX_COMPONENT_RIGIDBODY);
    nux_query_includes(_module.rigidbody_transform_iter,
                       NUX_COMPONENT_TRANSFORM);

    _module.collider_transform_iter = nux_query_new(arena, 2, 0);
    NUX_CHECK(_module.collider_transform_iter, return NUX_FAILURE);
    nux_query_includes(_module.collider_transform_iter, NUX_COMPONENT_COLLIDER);
    nux_query_includes(_module.collider_transform_iter,
                       NUX_COMPONENT_TRANSFORM);

    _module.ground_height = 0;

    return NUX_SUCCESS;
}
static nux_status_t
module_update (void)
{
    integrate();
    compute_transforms();
    return NUX_SUCCESS;
}
const nux_module_info_t *
nux_physics_module_info (void)
{
    static const nux_module_info_t info = {
        .name   = "physics",
        .size   = sizeof(_module),
        .data   = &_module,
        .init   = module_init,
        .update = module_update,
    };
    return &info;
}
nux_physics_module_t *
nux_physics (void)
{
    return &_module;
}

void
nux_physics_add_rigidbody (nux_nid_t e)
{
    nux_rigidbody_t *rigidbody = nux_component_get(e, NUX_COMPONENT_RIGIDBODY);
    nux_collider_t  *collider  = nux_component_get(e, NUX_COMPONENT_COLLIDER);
    if (collider)
    {
        switch (collider->type)
        {
            case NUX_COLLIDER_SPHERE: {
            }
            break;
            case NUX_COLLIDER_AABB: {
                nux_v3_t pos = nux_transform_get_translation(e);
                nux_v3_t vel = NUX_V3_ZEROS;

                nux_v3_t min = nux_v3_add(pos, collider->aabb.box.min);
                nux_v3_t max = nux_v3_add(pos, collider->aabb.box.max);

                nux_u32_t p0
                    = nux_physics_add_pm(nux_v3(min.x, min.y, min.z), vel);
                nux_u32_t p1
                    = nux_physics_add_pm(nux_v3(max.x, min.y, min.z), vel);
                nux_u32_t p2
                    = nux_physics_add_pm(nux_v3(max.x, min.y, max.z), vel);
                nux_u32_t p3
                    = nux_physics_add_pm(nux_v3(min.x, min.y, max.z), vel);

                nux_u32_t p4
                    = nux_physics_add_pm(nux_v3(min.x, max.y, min.z), vel);
                nux_u32_t p5
                    = nux_physics_add_pm(nux_v3(max.x, max.y, min.z), vel);
                nux_u32_t p6
                    = nux_physics_add_pm(nux_v3(max.x, max.y, max.z), vel);
                nux_u32_t p7
                    = nux_physics_add_pm(nux_v3(min.x, max.y, max.z), vel);

                const float s  = 1;
                const float s2 = nux_sqrt(s + s);
                const float s3 = nux_sqrt(s + s + s);

                nux_physics_add_distance_constraint(p0, p1, s);
                nux_physics_add_distance_constraint(p1, p2, s);
                nux_physics_add_distance_constraint(p2, p3, s);
                nux_physics_add_distance_constraint(p3, p0, s);

                nux_physics_add_distance_constraint(p4, p5, s);
                nux_physics_add_distance_constraint(p5, p6, s);
                nux_physics_add_distance_constraint(p6, p7, s);
                nux_physics_add_distance_constraint(p7, p4, s);

                nux_physics_add_distance_constraint(p0, p4, s);
                nux_physics_add_distance_constraint(p1, p5, s);
                nux_physics_add_distance_constraint(p2, p6, s);
                nux_physics_add_distance_constraint(p3, p7, s);

                nux_physics_add_distance_constraint(p0, p2, s2);
                // nux_physics_add_distance_constraictx, nt(p1, p3, s2);

                nux_physics_add_distance_constraint(p4, p6, s2);
                // nux_physics_add_distance_constraictx, nt(p5, p7, s2);

                nux_physics_add_distance_constraint(p0, p7, s2);
                // nux_physics_add_distance_constraictx, nt(p4, p3, s2);

                nux_physics_add_distance_constraint(p5, p2, s2);
                // nux_physics_add_distance_constraictx, nt(p1, p6, s2);

                nux_physics_add_distance_constraint(p4, p1, s2);
                // nux_physics_add_distance_constraictx, nt(p0, p5, s2);

                nux_physics_add_distance_constraint(p3, p6, s2);
                // nux_physics_add_distance_constraictx, nt(p7, p2, s2);

                nux_physics_add_distance_constraint(p4, p2, s3);
                nux_physics_add_distance_constraint(p0, p6, s3);
                nux_physics_add_distance_constraint(p7, p1, s3);
                nux_physics_add_distance_constraint(p3, p5, s3);

                rigidbody->first = p0;
                rigidbody->count = 8;
            }
            break;
        }
    }
}
nux_u32_t
nux_physics_add_pm (nux_v3_t pos, nux_v3_t vel)
{
    nux_point_mass_t *pm = nux_point_mass_vec_push(&_module.point_masses);
    NUX_ASSERT(pm);
    pm->x = pos;
    pm->v = vel;
    pm->f = NUX_V3_ZEROS;
    pm->m = 1;
    pm->w = 1.0 / pm->m;
    pm->p = NUX_V3_ZEROS;
    return _module.point_masses.size - 1;
}
void
nux_physics_add_distance_constraint (nux_u32_t a, nux_u32_t b, float distance)
{
    nux_distance_constraint_t *c
        = nux_distance_constraint_vec_push(&_module.distance_constraints);
    NUX_ASSERT(c);
    c->a        = a;
    c->b        = b;
    nux_v3_t pa = _module.point_masses.data[a].x;
    nux_v3_t pb = _module.point_masses.data[b].x;
    nux_v3_t ab = nux_v3_sub(pb, pa);
    c->d        = nux_sqrt(nux_v3_dot(ab, ab));
}

nux_raycast_hit_t
nux_physics_raycast (nux_v3_t pos, nux_v3_t dir)
{
    nux_ray_t         r       = { .p = pos, .d = nux_v3_normalize(dir) };
    nux_nid_t         it      = NUX_NULL;
    nux_f32_t         nearest = NUX_FLT_MAX;
    nux_raycast_hit_t hit;
    hit.node = NUX_NULL;
    hit.p    = NUX_V3_ZEROS;
    hit.n    = NUX_V3_ZEROS;
    while ((it = nux_query_next(_module.collider_transform_iter, it)))
    {
        nux_transform_t *transform
            = nux_component_get(it, NUX_COMPONENT_TRANSFORM);
        nux_collider_t *collider
            = nux_component_get(it, NUX_COMPONENT_COLLIDER);

        nux_v3_t translation = nux_transform_get_translation(it);
        switch (collider->type)
        {
            case NUX_COLLIDER_SPHERE: {
                nux_sphere_t s = {
                    .p = translation,
                    .r = collider->sphere.radius,
                };
                nux_f32_t t0;
                if (nux_intersect_ray_sphere(r, s, &t0) && t0 < nearest)
                {
                    nearest  = t0;
                    hit.node = it;
                    hit.p    = nux_v3_add(r.p, nux_v3_muls(r.d, t0));
                }
            }
            break;
            case NUX_COLLIDER_AABB: {
                nux_b3_t box
                    = nux_b3(nux_v3_add(collider->aabb.box.min, translation),
                             nux_v3_add(collider->aabb.box.max, translation));
                nux_f32_t t0, t1;
                if (nux_intersect_ray_box(r, box, &t0, &t1) && t0 < nearest)
                {
                    nearest  = t0;
                    hit.node = it;
                    hit.p    = nux_v3_add(r.p, nux_v3_muls(r.d, t0));
                }
            }
            break;
        }
    }
    return hit;
}
void
nux_physics_set_ground_height (nux_f32_t height)
{
    _module.ground_height = height;
}
