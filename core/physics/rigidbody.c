#include "internal.h"

void
nux_rigidbody_add (nux_eid_t e)
{
    nux_rigidbody_t *body = nux_ecs_add(e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);
    body->first = 0;
    body->count = 0;
    if (nux_ecs_has(e, NUX_COMPONENT_COLLIDER))
    {
        nux_physics_add_rigidbody(e);
    }
}
void
nux_rigidbody_remove (nux_eid_t e)
{
    nux_ecs_remove(e, NUX_COMPONENT_RIGIDBODY);
}
void
nux_rigidbody_set_velocity (nux_eid_t e, nux_v3_t velocity)
{
    nux_rigidbody_t *body = nux_ecs_get(e, NUX_COMPONENT_RIGIDBODY);
    NUX_CHECK(body, return);
    for (nux_u32_t i = 0; i < body->count; ++i)
    {
        nux_point_mass_t *pm
            = nux_physics_module()->point_masses.data + body->first + i;
        pm->v = velocity;
    }
}
