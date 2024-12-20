#include "gpu.h"
#include "common.h"
#include "memory.h"

static struct
{
    nu_gpu_state_t state;
} _gpu;

void
push_gpu_state (nu_byte_t *p)
{
    nu_memcpy(p, &_gpu.state, sizeof(_gpu.state));
}
void
pop_gpu_state (const nu_byte_t *p)
{
    nu_memcpy(&_gpu.state, p, sizeof(_gpu.state));
}
