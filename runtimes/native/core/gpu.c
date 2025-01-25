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
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        vm->gpu.meshes[i].active = NU_FALSE;
    }
    for (nu_size_t i = 0; i < GPU_MAX_NODE; ++i)
    {
        vm->gpu.nodes[i].transform = nu_m4_identity();
    }
    vm->gpu.config           = config->gpu;
    vm->gpu.state.model      = nu_m4_identity();
    vm->gpu.state.view       = nu_m4_identity();
    vm->gpu.state.projection = nu_m4_identity();
    vm->gpu.state.texture    = -1;
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
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        if (vm->gpu.meshes[i].active)
        {
            os_gpu_free_mesh(vm, i);
            vm->gpu.meshes[i].active = NU_FALSE;
        }
    }
    return NU_SUCCESS;
}
void
gpu_render (vm_t *vm)
{
}
void
gpu_begin (vm_t *vm)
{
    os_gpu_begin(vm);
}
void
gpu_end (vm_t *vm)
{
    os_gpu_end(vm);
}

static void
check_mesh (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MESH || !vm->gpu.meshes[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive mesh %d", index);
    }
}
static void
check_texture (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_TEXTURE || !vm->gpu.textures[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive texture %d", index);
    }
}
static void
check_node (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_NODE)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive node %d", index);
    }
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
gpu_alloc_mesh (vm_t                  *vm,
                nu_u32_t               index,
                nu_u32_t               count,
                gpu_primitive_t        primitive,
                gpu_vertex_attribute_t attributes,
                const void            *p)
{
    if (index >= GPU_MAX_MESH || vm->gpu.meshes[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Mesh %d already allocated or invalid", index);
        return;
    }
    vm->gpu.meshes[index].active     = NU_TRUE;
    vm->gpu.meshes[index].count      = count;
    vm->gpu.meshes[index].primitive  = primitive;
    vm->gpu.meshes[index].attributes = attributes;
    os_gpu_init_mesh(vm, index, p);
}
void
gpu_write_mesh (vm_t                  *vm,
                nu_u32_t               index,
                gpu_vertex_attribute_t attributes,
                nu_u32_t               first,
                nu_u32_t               count,
                const void            *p)
{
    check_mesh(vm, index);
    os_gpu_write_mesh(vm, index, attributes, first, count, p);
}

void
gpu_set_node_mesh (vm_t *vm, nu_u32_t index, nu_u32_t mesh)
{
    check_node(vm, index);
    vm->gpu.nodes[index].mesh = mesh;
}
void
gpu_set_node_texture (vm_t *vm, nu_u32_t index, nu_u32_t texture)
{
    check_node(vm, index);
    vm->gpu.nodes[index].texture = texture;
}
void
gpu_set_node_transform (vm_t *vm, nu_u32_t index, const nu_f32_t *m)
{
    check_node(vm, index);
    vm->gpu.nodes[index].transform = nu_m4(m);
}

void
gpu_set_transform (vm_t *vm, gpu_transform_t transform, const nu_f32_t *m)
{
    switch (transform)
    {
        case GPU_TRANSFORM_MODEL:
            vm->gpu.state.model = nu_m4(m);
            break;
        case GPU_TRANSFORM_VIEW:
            vm->gpu.state.view = nu_m4(m);
            break;
        case GPU_TRANSFORM_PROJECTION:
            vm->gpu.state.projection = nu_m4(m);
            break;
    }
    os_gpu_set_transform(vm, transform);
}
void
gpu_set_texture (vm_t *vm, nu_u32_t index)
{
    check_texture(vm, index);
    os_gpu_set_texture(vm, index);
}
void
gpu_draw_mesh (vm_t *vm, nu_u32_t mesh)
{
    check_mesh(vm, mesh);
    gpu_draw_submesh(vm, mesh, 0, vm->gpu.meshes[mesh].count);
}
void
gpu_draw_submesh (vm_t *vm, nu_u32_t mesh, nu_u32_t first, nu_u32_t count)
{
    check_mesh(vm, mesh);
    os_gpu_draw_submesh(vm, mesh, first, count);
}
void
gpu_draw_nodes (vm_t *vm, nu_u32_t first, nu_u32_t count)
{
    for (nu_u32_t i = first; i < (first + count); ++i)
    {
        check_node(vm, i);
    }
    os_gpu_draw_nodes(vm, first, count);
}
