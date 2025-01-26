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
    for (nu_size_t i = 0; i < GPU_MAX_MODEL; ++i)
    {
        vm->gpu.models[i].texture         = -1;
        vm->gpu.models[i].mesh            = -1;
        vm->gpu.models[i].child           = -1;
        vm->gpu.models[i].sibling         = -1;
        vm->gpu.models[i].local_to_parent = nu_m4_identity();
    }
    vm->gpu.config           = config->gpu;
    vm->gpu.state.model      = nu_m4_identity();
    vm->gpu.state.view       = nu_m4_identity();
    vm->gpu.state.projection = nu_m4_identity();
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
gpu_begin (vm_t *vm)
{
    os_gpu_begin(vm);
}
void
gpu_end (vm_t *vm)
{
    os_gpu_end(vm);
}

nu_status_t
gpu_load_texture (vm_t *vm, const cart_chunk_header_t *header)
{
    nu_u32_t index, size;
    NU_CHECK(iou_read_u32(vm, &index), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &size), return NU_FAILURE);
    iou_log(vm, NU_LOG_INFO, "- index: %d", index);
    iou_log(vm, NU_LOG_INFO, "- size: %d", size);
    // TODO: validate size
    nu_size_t data_length = (64 << size) * (64 << size) * 4;
    NU_CHECK(iou_read(vm, vm->iou.heap, data_length), return NU_FAILURE);
    gpu_alloc_texture(vm, index, size, vm->iou.heap);
    return NU_SUCCESS;
}
nu_status_t
gpu_load_mesh (vm_t *vm, const cart_chunk_header_t *header)
{
    nu_u32_t index, count, primitive, flags;
    NU_CHECK(iou_read_u32(vm, &index), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &count), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &primitive), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &flags), return NU_FAILURE);
    iou_log(vm, NU_LOG_INFO, "Loading mesh index %d", index);
    NU_ASSERT(
        os_iou_read(vm, vm->iou.heap, header->length - sizeof(nu_u32_t) * 4));
    gpu_alloc_mesh(vm, index, count, primitive, flags, vm->iou.heap);
    return NU_SUCCESS;
}
nu_status_t
gpu_load_model (vm_t *vm, const cart_chunk_header_t *header)
{
    nu_u32_t index, mesh, texture, parent;
    nu_m4_t  transform;
    NU_CHECK(iou_read_u32(vm, &index), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &mesh), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &texture), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &parent), return NU_FAILURE);
    NU_CHECK(iou_read_m4(vm, &transform), return NU_FAILURE);
    gpu_set_model_mesh(vm, index, mesh);
    gpu_set_model_texture(vm, index, texture);
    gpu_set_model_transform(vm, index, transform.data);
    gpu_set_model_parent(vm, index, parent);
    return NU_SUCCESS;
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
check_model (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MODEL)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid model %d", index);
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
gpu_set_model_mesh (vm_t *vm, nu_u32_t index, nu_u32_t mesh)
{
    check_model(vm, index);
    vm->gpu.models[index].mesh = mesh;
}
void
gpu_set_model_texture (vm_t *vm, nu_u32_t index, nu_u32_t texture)
{
    check_model(vm, index);
    vm->gpu.models[index].texture = texture;
}
void
gpu_set_model_transform (vm_t *vm, nu_u32_t index, const nu_f32_t *m)
{
    check_model(vm, index);
    vm->gpu.models[index].local_to_parent = nu_m4(m);
}
void
gpu_set_model_parent (vm_t *vm, nu_u32_t index, nu_u32_t parent)
{
    check_model(vm, index);
    check_model(vm, parent);
    gpu_model_t *p = vm->gpu.models + parent;
    gpu_model_t *m = vm->gpu.models + index;
    m->sibling     = p->child;
    p->child       = index;
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
gpu_draw_model (vm_t *vm, nu_u32_t index)
{
    check_model(vm, index);
    os_gpu_draw_model(vm, index);
}
