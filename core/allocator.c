#include "internal.h"

void *
nux_malloc (nux_env_t env, nux_u32_t n, nux_ptr_t *ptr)
{
    if (env->inst->memhead + n > env->inst->memcapa)
    {
        nux_set_error(env, NUX_ERROR_OUT_OF_MEMORY);
        return NU_NULL;
    }
    *ptr = env->inst->memhead;
    env->inst->memhead += n;
    return env->inst->mem + (*ptr);
}
void *
nux_instance_get_memory (nux_instance_t inst, nux_ptr_t ptr)
{
    // TODO: check boundary
    return inst->mem + ptr;
}
