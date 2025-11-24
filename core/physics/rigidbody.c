#include "internal.h"

void
nux_rigidbody_set_velocity (nux_node_t *node, nux_v3_t velocity)
{
    nux_physics_module_t *module = nux_physics();
    nux_rigidbody_t      *body   = nux_node_check(NUX_NODE_RIGIDBODY, node);
    nux_check(body, return);
    for (nux_u32_t i = 0; i < body->count; ++i)
    {
        nux_point_mass_t *pm = module->point_masses.data + body->first + i;
        pm->v                = velocity;
    }
}

void
nux_rigidbody_add (nux_node_t *node)
{
    nux_rigidbody_t *body = nux_node_check(NUX_NODE_RIGIDBODY, node);
    body->first           = 0;
    body->count           = 0;
}
