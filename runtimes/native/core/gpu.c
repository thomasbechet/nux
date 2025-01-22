#include "gpu.h"

#include "iou.h"
#include "platform.h"

nu_status_t
gpu_init (vm_t *vm, const vm_config_t *config)
{
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        vm->gpu.textures[i].active = NU_FALSE;
    }
    for (nu_size_t i = 0; i < GPU_MAX_VBUFFER; ++i)
    {
        vm->gpu.vbuffers[i].active = NU_FALSE;
    }
    vm->gpu.config = config->gpu;
    os_gpu_init(vm);
    return NU_SUCCESS;
}
nu_status_t
gpu_free (vm_t *vm)
{
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        if (vm->gpu.textures[i].active)
        {
            os_gpu_free_texture(vm, i);
            vm->gpu.textures[i].active = NU_FALSE;
        }
    }
    for (nu_size_t i = 0; i < GPU_MAX_VBUFFER; ++i)
    {
        if (vm->gpu.vbuffers[i].active)
        {
            os_gpu_free_vbuffer(vm, i);
            vm->gpu.vbuffers[i].active = NU_FALSE;
        }
    }
    return NU_SUCCESS;
}
void
gpu_render (vm_t *vm)
{
}

void
gpu_alloc_texture (vm_t              *vm,
                   nu_u32_t           index,
                   gpu_texture_size_t size,
                   const void        *p)
{
    if (index >= GPU_MAX_TEXTURE || vm->gpu.textures[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Texture %d already allocated or invalid", index);
        return;
    }
    vm->gpu.textures[index].active = NU_TRUE;
    vm->gpu.textures[index].size   = size;
    os_gpu_init_texture(vm, index, p);
}
void
gpu_alloc_vbuffer (vm_t *vm, nu_u32_t index, nu_u32_t count, const void *p)
{
    if (index >= GPU_MAX_VBUFFER || vm->gpu.vbuffers[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "VBuffer %d already allocated or invalid", index);
        return;
    }
    vm->gpu.vbuffers[index].active = NU_TRUE;
    vm->gpu.vbuffers[index].count  = count;
    os_gpu_init_vbuffer(vm, index, p);
}
void
gpu_write_texture (vm_t       *vm,
                   nu_u32_t    index,
                   nu_u32_t    x,
                   nu_u32_t    y,
                   nu_u32_t    w,
                   nu_u32_t    h,
                   const void *p)
{
    if (index >= GPU_MAX_TEXTURE || !vm->gpu.textures[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive texture %d", index);
        return;
    }
    os_gpu_write_texture(vm, index, x, y, w, h, p);
}
void
gpu_write_vbuffer (
    vm_t *vm, nu_u32_t index, nu_u32_t first, nu_u32_t count, const void *p)
{
    if (index >= GPU_MAX_VBUFFER || !vm->gpu.vbuffers[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Trying to write to inactive vbuffer %d", index);
        return;
    }
    os_gpu_write_vbuffer(vm, index, first, count, p);
}

void
gpu_draw (vm_t *vm, nu_u32_t first, nu_u32_t count)
{
    os_gpu_draw(vm, first, count);
}
