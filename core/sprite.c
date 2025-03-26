#include "internal.h"

nux_status_t
nux_create_spritesheet (nux_env_t env,
                        nux_oid_t oid,
                        nux_u32_t texture,
                        nux_u32_t row,
                        nux_u32_t col,
                        nux_oid_t fwidth,
                        nux_u32_t fheight)
{
    nux_object_t *object
        = nux_object_set(env->inst, oid, NUX_OBJECT_SPRITESHEET);
    NU_CHECK(object, return NUX_FAILURE);
    object->spritesheet.texture = texture;
    object->spritesheet.row     = row;
    object->spritesheet.col     = col;
    object->spritesheet.fwidth  = fwidth;
    object->spritesheet.fheight = fheight;
    return NUX_SUCCESS;
}
