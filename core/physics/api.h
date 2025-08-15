#ifndef NUX_PHYSICS_API_H
#define NUX_PHYSICS_API_H

#include "base/api.h"

void nux_rigidbody_add(nux_ctx_t *ctx, nux_ent_t e);
void nux_rigidbody_remove(nux_ctx_t *ctx, nux_ent_t e);
void nux_rigidbody_set_velocity(nux_ctx_t *ctx, nux_ent_t e, nux_v3_t velocity);

#endif
