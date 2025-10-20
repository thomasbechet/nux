#include "internal.h"

void
nux_collider_add (nux_nid_t e, void *data)
{
    nux_collider_t *collider = data;
    collider->type           = NUX_COLLIDER_SPHERE;
    collider->sphere.radius  = 1;
}

void
nux_collider_set_sphere (nux_nid_t e, nux_f32_t radius)
{
    nux_collider_t *collider = nux_component_get(e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type          = NUX_COLLIDER_SPHERE;
    collider->sphere.radius = radius;
}
void
nux_collider_set_aabb (nux_nid_t e, nux_v3_t min, nux_v3_t max)
{
    nux_collider_t *collider = nux_component_get(e, NUX_COMPONENT_COLLIDER);
    NUX_CHECK(collider, return);
    collider->type     = NUX_COLLIDER_AABB;
    collider->aabb.box = nux_b3(min, max);
    if (nux_node_has(e, NUX_COMPONENT_RIGIDBODY))
    {
        nux_physics_add_rigidbody(e);
    }
}
