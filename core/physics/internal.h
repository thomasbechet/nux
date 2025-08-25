#ifndef NUX_PHYSICS_INTERNAL_H
#define NUX_PHYSICS_INTERNAL_H

#include <ecs/internal.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct
{
    nux_v3_t p0;
    nux_v3_t p1;
} nux_segment_t;

typedef struct
{
    nux_v3_t p;
    nux_v3_t d;
} nux_ray_t;

typedef struct
{
    nux_v3_t p;
    nux_v3_t n;
} nux_plane_t;

typedef struct
{
    nux_v3_t  p;
    nux_f32_t r;
} nux_sphere_t;

typedef struct
{
    nux_b32_t hit;
    nux_v3_t  p;
} nux_hit_t;

typedef struct
{
    nux_v3_t x; // position
    nux_v3_t v; // velocity
    nux_v3_t f; // force
    float    m; // mass
    float    w; // inv mass
    nux_v3_t p; // previous position
} nux_point_mass_t;

typedef struct
{
    nux_u32_t a; // index of point mass
    nux_v3_t  q; // position of collision
    nux_v3_t  n; // normal
} nux_collision_constraint_t;

typedef struct
{
    nux_u32_t a; // index of first point mass
    nux_u32_t b; // index of second point mass
    float     d; // distance
} nux_distance_constraint_t;

NUX_VEC_DEFINE(nux_point_mass_vec, nux_point_mass_t);
NUX_VEC_DEFINE(nux_collision_constraint_vec, nux_collision_constraint_t);
NUX_VEC_DEFINE(nux_distance_constraint_vec, nux_distance_constraint_t);

typedef struct
{
    nux_u32_t first;
    nux_u32_t count;
} nux_rigidbody_t;

typedef struct
{
    nux_collider_type_t type;
    union
    {
        struct
        {
            nux_f32_t radius;
        } sphere;
        struct
        {
            nux_b3_t box;
        } aabb;
    };
} nux_collider_t;

typedef struct nux_physics_module
{
    nux_point_mass_vec_t           point_masses;
    nux_collision_constraint_vec_t collision_constraints;
    nux_distance_constraint_vec_t  distance_constraints;
    nux_res_t                      rigidbody_transform_iter;
    nux_res_t                      collider_transform_iter;
} nux_physics_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// physics,c

nux_status_t nux_physics_init(nux_ctx_t *ctx);
void         nux_physics_free(nux_ctx_t *ctx);
void         nux_physics_update(nux_ctx_t *ctx);
void         nux_physics_add_rigidbody(nux_ctx_t *ctx, nux_ent_t e);
nux_u32_t    nux_physics_add_pm(nux_ctx_t *ctx, nux_v3_t pos, nux_v3_t vel);
void         nux_physics_add_distance_constraint(nux_ctx_t *ctx,
                                                 nux_u32_t  a,
                                                 nux_u32_t  b,
                                                 float      distance);

// lua_bindings.c

nux_status_t nux_lua_open_physics(nux_ctx_t *ctx);

// intersect.c

nux_b32_t nux_intersect_ray_sphere(nux_ray_t r, nux_sphere_t s, nux_hit_t *hit);
nux_b32_t nux_intersect_segment_plane(nux_segment_t s,
                                      nux_plane_t   p,
                                      nux_hit_t    *hit);
nux_b32_t nux_intersect_ray_box(nux_ray_t r, nux_b3_t box, nux_hit_t *hit);

#endif
