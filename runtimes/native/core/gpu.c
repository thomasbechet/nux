#include "gpu.h"

#include "iou.h"
#include "platform.h"

nu_status_t
gpu_init (vm_t *vm)
{
    for (nu_size_t i = 0; i < GPU_MAX_POOL; ++i)
    {
        vm->gpu.pools[i].active = NU_FALSE;
    }
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        vm->gpu.textures[i].active = NU_FALSE;
    }
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        vm->gpu.meshes[i].active = NU_FALSE;
    }
    return NU_SUCCESS;
}
nu_status_t
gpu_free (vm_t *vm)
{
    return NU_SUCCESS;
}
void
gpu_render (vm_t *vm)
{
}

void
gpu_init_pool (vm_t *vm, nu_u32_t index, nu_u32_t mem_size)
{
    if (index >= GPU_MAX_POOL || vm->gpu.pools[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid pool index or already reserved");
        return;
    }
    vm->gpu.pools[index].active    = NU_TRUE;
    vm->gpu.pools[index].size      = mem_size;
    vm->gpu.pools[index].remaining = mem_size;
}
void
gpu_free_pool (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_POOL || !vm->gpu.pools[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid pool index or already reserved");
        return;
    }
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        if (vm->gpu.textures[i].active)
        {
            os_gpu_free_texture(vm, i);
            vm->gpu.textures[i].active = NU_FALSE;
        }
    }
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        if (vm->gpu.meshes[i].active)
        {
            os_gpu_free_mesh(vm, i);
            vm->gpu.meshes[i].active = NU_FALSE;
        }
    }
    vm->gpu.pools[index].active = NU_FALSE;
}

void
gpu_alloc_texture (vm_t       *vm,
                   nu_u32_t    texture_index,
                   nu_u32_t    pool_index,
                   nu_u32_t    w,
                   nu_u32_t    h,
                   const void *p)
{
    if (texture_index >= GPU_MAX_TEXTURE
        || vm->gpu.textures[texture_index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid texture allocation");
        return;
    }
    gpu_texture_t *tex = vm->gpu.textures + texture_index;
    tex->active        = NU_TRUE;
    tex->pool_index    = pool_index;
    tex->width         = w;
    tex->height        = h;
    os_gpu_init_texture(vm, texture_index, p);
}
void
gpu_write_texture (vm_t       *vm,
                   nu_u32_t    texture_index,
                   nu_u32_t    x,
                   nu_u32_t    y,
                   nu_u32_t    w,
                   nu_u32_t    h,
                   const void *p)
{
    if (texture_index >= GPU_MAX_TEXTURE
        || !vm->gpu.textures[texture_index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid texture writing");
        return;
    }
    // TODO: validate writing area
    os_gpu_write_texture(vm, texture_index, x, y, w, h, p);
}

void
gpu_alloc_mesh (vm_t       *vm,
                nu_u32_t    mesh_index,
                nu_u32_t    pool_index,
                nu_u32_t    count,
                const void *p)
{
    if (mesh_index >= GPU_MAX_MESH || vm->gpu.meshes[mesh_index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid mesh allocation");
        return;
    }
    gpu_mesh_t *mesh = vm->gpu.meshes + mesh_index;
    mesh->active     = NU_TRUE;
    mesh->pool_index = pool_index;
    mesh->count      = count;
    os_gpu_init_mesh(vm, mesh_index, p);
}
void
gpu_write_mesh (vm_t       *vm,
                nu_u32_t    mesh_index,
                nu_u32_t    first,
                nu_u32_t    count,
                const void *p)
{
    if (mesh_index > +GPU_MAX_MESH || !vm->gpu.meshes[mesh_index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid mesh writing");
        return;
    }
    // TODO: validate writing area
    os_gpu_write_mesh(vm, mesh_index, first, count, p);
}

void
gpu_draw (vm_t *vm, nu_u32_t first, nu_u32_t count)
{
    os_gpu_draw(vm, first, count);
}
