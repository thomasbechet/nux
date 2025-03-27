#include "internal.h"

void *
nux_malloc (nux_instance_t inst, nux_u32_t n, nux_ptr_t *ptr)
{
    if (inst->memhead + n > inst->memcapa)
    {
        nux_set_error(inst, NUX_ERROR_ALLOCATION);
        return NU_NULL;
    }
    *ptr = inst->memhead;
    inst->memhead += n;
    return inst->mem + (*ptr);
}
void *
nux_instance_get_memory (nux_instance_t inst, nux_ptr_t ptr)
{
    // TODO: check boundary
    return inst->mem + ptr;
}
