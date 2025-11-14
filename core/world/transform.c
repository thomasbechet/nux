#include "internal.h"

static nux_u32_vec_t transforms;

static nux_u32_t
transform_add (nux_oid_t oid)
{
    nux_u32_t index = transforms.size;
    void     *t     = nux_vec_push(&transforms);
    // nux_object_set(oid, NUX_COMPONENT_TRANSFORM, index);
}
static void
transform_remove (nux_oid_t oid, nux_u32_t handle)
{
}
