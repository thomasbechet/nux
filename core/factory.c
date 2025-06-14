#include "internal.h"

typedef nux_u32_t (*nux_object_serialize_t)(nux_env_t *env, void *data);

static void
nux_register_object (nux_env_t *env, nux_u32_t id, nux_object_serialize_t ser)
{
}
