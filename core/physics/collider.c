#include "internal.h"

void
nux_collider_add (nux_node_t *node)
{
    nux_collider_t *collider = nux_node_check(NUX_NODE_COLLIDER, node);
    collider->type           = NUX_COLLIDER_SPHERE;
    collider->sphere.radius  = 1;
}

void
nux_collider_set_sphere (nux_node_t *node, nux_f32_t radius)
{
    nux_collider_t *collider = nux_node_check(NUX_NODE_COLLIDER, node);
    nux_check(collider, return);
    collider->type          = NUX_COLLIDER_SPHERE;
    collider->sphere.radius = radius;
}
void
nux_collider_set_aabb (nux_node_t *node, nux_v3_t min, nux_v3_t max)
{
    nux_collider_t *collider = nux_node_check(NUX_NODE_COLLIDER, node);
    nux_check(collider, return);
    collider->type     = NUX_COLLIDER_AABB;
    collider->aabb.box = nux_b3(min, max);
}
