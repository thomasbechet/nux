#ifndef NUX_PHYSICS_INTERNAL_H
#define NUX_PHYSICS_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_point_mass_vec_t           point_masses;
    nux_collision_constraint_vec_t collision_constraints;
    nux_distance_constraint_vec_t  distance_constraints;
    nux_query_t                   *rigidbody_transform_iter;
    nux_query_t                   *collider_transform_iter;
    nux_f32_t                      ground_height;
} nux_physics_module_t;

void                  nux_physics_module_register(void);
nux_physics_module_t *nux_physics(void);

#endif
