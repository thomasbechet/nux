#include "internal.h"

nux_u32_t
nux_entity_new (nux_env_t *env, nux_u32_t world)
{
    nux_world_t *w = nux_object_get(env, NUX_OBJECT_WORLD, world);
    NUX_CHECKM(w, "Invalid world id", return NUX_NULL);
    return NUX_NULL;
}
void
nux_entity_cleanup (nux_env_t *env, void *data)
{
}
