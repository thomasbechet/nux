#ifndef NUX_PHYSICS_INTERNAL_H
#define NUX_PHYSICS_INTERNAL_H

#include "base/internal.h"

////////////////////////////
///        TYPES         ///
////////////////////////////

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

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// physics,c

nux_status_t nux_physics_init(nux_ctx_t *ctx);
void         nux_physics_free(nux_ctx_t *ctx);

#endif
