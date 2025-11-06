#ifndef NUX_PHYSICS_H
#define NUX_PHYSICS_H

#include <scene/api.h>

typedef enum
{
    NUX_COLLIDER_SPHERE = 0,
    NUX_COLLIDER_AABB   = 1,
} nux_collider_type_t;

typedef struct
{
    nux_nid_t node;
    nux_v3_t  p;
    nux_v3_t  n;
} nux_raycast_hit_t;

void nux_rigidbody_set_velocity(nux_nid_t e, nux_v3_t velocity);

void nux_collider_set_sphere(nux_nid_t e, nux_f32_t radius);
void nux_collider_set_aabb(nux_nid_t e, nux_v3_t min, nux_v3_t max);

nux_raycast_hit_t nux_physics_raycast(nux_v3_t pos, nux_v3_t dir);
void              nux_physics_set_ground_height(nux_f32_t height);

#endif
