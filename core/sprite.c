#include "internal.h"

nux_id_t
nux_spritesheet_create (nux_env_t env,
                        nux_id_t  stack,
                        nux_u32_t texture,
                        nux_u32_t row,
                        nux_u32_t col,
                        nux_u32_t fwidth,
                        nux_u32_t fheight)
{
    nux_id_t id = nux_stack_push(
        env, stack, NUX_OBJECT_SPRITESHEET, sizeof(nux_spritesheet_t));
    NU_CHECK(id, return NU_NULL);
    nux_spritesheet_t *spritesheet = nux_object_get_unchecked(env, id);
    spritesheet->texture           = texture;
    spritesheet->row               = row;
    spritesheet->col               = col;
    spritesheet->fwidth            = fwidth;
    spritesheet->fheight           = fheight;
    return NUX_SUCCESS;
}
