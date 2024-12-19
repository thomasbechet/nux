#ifndef NU_GPU_IMPL_H
#define NU_GPU_IMPL_H

#include <internal.h>

void
push_gpu_state (nu_byte_t *p)
{
    nu_memcpy(p, &_ctx.gpu.state, sizeof(_ctx.gpu.state));
}
void
pop_gpu_state (const nu_byte_t *p)
{
    nu_memcpy(&_ctx.gpu.state, p, sizeof(_ctx.gpu.state));
}

#endif
