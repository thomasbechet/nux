#ifndef NUX_PHYSICS_INTERNAL_H
#define NUX_PHYSICS_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_point_mass_vec_t           point_masses;
    nux_collision_constraint_vec_t collision_constraints;
    nux_distance_constraint_vec_t  distance_constraints;
    nux_ecs_iter_t                *rigidbody_transform_iter;
    nux_ecs_iter_t                *collider_transform_iter;
} nux_physics_module_t;

nux_physics_module_t *nux_physics_module(void);

nux_status_t nux_physics_init(void);
void         nux_physics_free(void);
void         nux_physics_update(void);

#endif
