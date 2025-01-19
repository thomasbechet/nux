#include "gpu.h"

#include "platform.h"

nu_status_t
gpu_init (vm_t *vm)
{
    return NU_SUCCESS;
}
nu_status_t
gpu_free (vm_t *vm)
{
    return NU_SUCCESS;
}

void
gpu_write_texture (vm_t       *vm,
                   nu_u32_t    slot,
                   nu_u32_t    x,
                   nu_u32_t    y,
                   nu_u32_t    w,
                   nu_u32_t    h,
                   const void *p)
{
    NU_ASSERT(slot <= vm->config.gpu_texture_count);
    os_gpu_write_texture(vm, slot, x, y, w, h, p);
}
void
gpu_write_vertex (vm_t *vm, nu_u32_t first, nu_u32_t count, const void *p)
{
    NU_ASSERT(first + count <= vm->config.gpu_vertex_count);
    os_gpu_write_vertex(vm, first, count, p);
}
void
gpu_bind_texture (vm_t *vm, nu_u32_t slot)
{
    os_gpu_bind_texture(vm, slot);
}
void
gpu_draw (vm_t *vm, nu_u32_t first, nu_u32_t count)
{
    os_gpu_draw(vm, first, count);
}
void
gpu_render (vm_t *vm)
{
}
