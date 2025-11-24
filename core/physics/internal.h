#ifndef NUX_PHYSICS_INTERNAL_H
#define NUX_PHYSICS_INTERNAL_H

#include <physics/physics.h>

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

typedef struct
{
    nux_u32_t first;
    nux_u32_t count;
} nux_rigidbody_t;

typedef struct
{
    nux_collider_type_t type;
    nux_u32_t           mask;
    nux_u32_t           layer;
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

typedef struct
{
    nux_vec(nux_point_mass_t) point_masses;
    nux_vec(nux_collision_constraint_t) collision_constraints;
    nux_vec(nux_distance_constraint_t) distance_constraints;
    nux_f32_t ground_height;
} nux_physics_module_t;

void                  nux_physics_module_register(void);
nux_physics_module_t *nux_physics(void);

void      nux_physics_add_rigidbody(nux_node_t *node);
nux_u32_t nux_physics_add_pm(nux_v3_t pos, nux_v3_t vel);
void      nux_physics_add_distance_constraint(nux_u32_t a,
                                              nux_u32_t b,
                                              float     distance);

void nux_rigidbody_add(nux_node_t *node);

void nux_collider_add(nux_node_t *node);

nux_b32_t nux_intersect_ray_sphere(nux_ray_t r, nux_sphere_t s, nux_f32_t *t0);
nux_b32_t nux_intersect_segment_plane(nux_segment_t s,
                                      nux_plane_t   p,
                                      nux_v3_t     *i0);
nux_b32_t nux_intersect_ray_box(nux_ray_t  r,
                                nux_b3_t   box,
                                nux_f32_t *t0,
                                nux_f32_t *t1);

#endif
