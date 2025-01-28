#include "gpu.h"

#include "iou.h"
#include "platform.h"

static nu_status_t
check_pool (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_POOL || !vm->gpu.pools[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive pool %d", index);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static nu_status_t
check_mesh (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MESH || !vm->gpu.meshes[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive mesh %d", index);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static nu_status_t
check_texture (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_TEXTURE || !vm->gpu.textures[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive texture %d", index);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static nu_status_t
check_model (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MODEL)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid model %d", index);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static nu_status_t
reserve_memory (vm_t *vm, nu_u32_t size)
{
    if (vm->gpu.state.pool == GPU_GLOBAL_POOL)
    {
        if (vm->gpu.vram_remaining < size)
        {
            iou_log(vm,
                    NU_LOG_ERROR,
                    "GPU out of global memory (requested %d)",
                    size);
            return NU_FAILURE;
        }
        vm->gpu.vram_remaining -= size;
    }
    else
    {
        gpu_pool_t *pool = vm->gpu.pools + vm->gpu.state.pool;
        if (pool->remaining < size)
        {
            iou_log(vm,
                    NU_LOG_ERROR,
                    "GPU out of memory (requested %d on pool %d)",
                    size,
                    vm->gpu.state.pool);
            return NU_FAILURE;
        }
        pool->remaining -= size;
    }
    return NU_SUCCESS;
}

nu_status_t
gpu_init (vm_t *vm, const gpu_config_t *config)
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
    for (nu_size_t i = 0; i < GPU_MAX_MODEL; ++i)
    {
        vm->gpu.models[i].texture         = -1;
        vm->gpu.models[i].mesh            = -1;
        vm->gpu.models[i].child           = -1;
        vm->gpu.models[i].sibling         = -1;
        vm->gpu.models[i].local_to_parent = nu_m4_identity();
    }
    vm->gpu.config           = *config;
    vm->gpu.vram_remaining   = config->vram_capacity;
    vm->gpu.state.pool       = GPU_GLOBAL_POOL;
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
    nu_u32_t  width       = (64 << size);
    nu_size_t data_length = width * width * 4;
    NU_CHECK(iou_read(vm, vm->iou.heap, data_length), return NU_FAILURE);
    gpu_alloc_texture(vm, index, size);
    gpu_update_texture(vm, index, 0, 0, width, width, vm->iou.heap);
    return NU_SUCCESS;
}
nu_status_t
gpu_load_mesh (vm_t *vm, const cart_chunk_header_t *header)
{
    nu_u32_t index, count, primitive, attributes;
    NU_CHECK(iou_read_u32(vm, &index), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &count), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &primitive), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &attributes), return NU_FAILURE);
    iou_log(vm, NU_LOG_INFO, "- index %d", index);
    iou_log(vm, NU_LOG_INFO, "- count %d", count);
    iou_log(vm, NU_LOG_INFO, "- primitive %d", primitive);
    iou_log(vm, NU_LOG_INFO, "- attributes %x", attributes);
    NU_ASSERT(
        os_iou_read(vm,
                    vm->iou.heap,
                    gpu_vertex_size(attributes) * count * sizeof(nu_f32_t)));
    gpu_alloc_mesh(vm, index, count, primitive, attributes);
    gpu_update_mesh(vm, index, attributes, 0, count, vm->iou.heap);
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
    iou_log(vm, NU_LOG_INFO, "- index %d", index);
    iou_log(vm, NU_LOG_INFO, "- mesh %d", mesh);
    iou_log(vm, NU_LOG_INFO, "- texture %d", texture);
    iou_log(vm, NU_LOG_INFO, "- parent %d", parent);
    gpu_set_model_mesh(vm, index, mesh);
    gpu_set_model_texture(vm, index, texture);
    gpu_set_model_transform(vm, index, transform.data);
    gpu_set_model_parent(vm, index, parent);
    return NU_SUCCESS;
}

nu_status_t
gpu_alloc_pool (vm_t *vm, nu_u32_t index, nu_u32_t size)
{
    if (index >= GPU_MAX_POOL || vm->gpu.pools[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Pool %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    NU_CHECK(reserve_memory(vm, size), return NU_FAILURE);
    vm->gpu.pools[index].active    = NU_TRUE;
    vm->gpu.pools[index].size      = size;
    vm->gpu.pools[index].remaining = size;
    return NU_SUCCESS;
}
nu_status_t
gpu_bind_pool (vm_t *vm, nu_u32_t index)
{
    NU_CHECK(check_pool(vm, index), return NU_FAILURE);
    vm->gpu.state.pool = index;
    return NU_SUCCESS;
}
nu_status_t
gpu_clear_pool (vm_t *vm, nu_u32_t index)
{
    NU_CHECK(check_pool(vm, index), return NU_FAILURE);
    vm->gpu.state.pool = index;
    return NU_SUCCESS;
}

nu_status_t
gpu_alloc_texture (vm_t *vm, nu_u32_t index, gpu_texture_size_t size)
{
    if (index >= GPU_MAX_TEXTURE || vm->gpu.textures[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Texture %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    NU_CHECK(reserve_memory(vm, gpu_texture_memsize(size)), return NU_FAILURE);
    vm->gpu.textures[index].active = NU_TRUE;
    vm->gpu.textures[index].size   = size;
    os_gpu_init_texture(vm, index);
    return NU_SUCCESS;
}
nu_status_t
gpu_update_texture (vm_t       *vm,
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
        return NU_FAILURE;
    }
    os_gpu_update_texture(vm, index, x, y, w, h, p);
    return NU_SUCCESS;
}
nu_status_t
gpu_alloc_mesh (vm_t                  *vm,
                nu_u32_t               index,
                nu_u32_t               count,
                gpu_primitive_t        primitive,
                gpu_vertex_attribute_t attributes)
{
    if (index >= GPU_MAX_MESH || vm->gpu.meshes[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Mesh %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    vm->gpu.meshes[index].active     = NU_TRUE;
    vm->gpu.meshes[index].count      = count;
    vm->gpu.meshes[index].primitive  = primitive;
    vm->gpu.meshes[index].attributes = attributes;
    os_gpu_init_mesh(vm, index);
    return NU_SUCCESS;
}
nu_status_t
gpu_update_mesh (vm_t                  *vm,
                 nu_u32_t               index,
                 gpu_vertex_attribute_t attributes,
                 nu_u32_t               first,
                 nu_u32_t               count,
                 const void            *p)
{
    NU_CHECK(check_mesh(vm, index), return NU_FAILURE);
    os_gpu_update_mesh(vm, index, attributes, first, count, p);
    return NU_SUCCESS;
}

nu_status_t
gpu_set_model_mesh (vm_t *vm, nu_u32_t index, nu_u32_t mesh)
{
    NU_CHECK(check_model(vm, index), return NU_FAILURE);
    vm->gpu.models[index].mesh = mesh;
    return NU_SUCCESS;
}
nu_status_t
gpu_set_model_texture (vm_t *vm, nu_u32_t index, nu_u32_t texture)
{
    NU_CHECK(check_model(vm, index), return NU_FAILURE);
    vm->gpu.models[index].texture = texture;
    return NU_SUCCESS;
}
nu_status_t
gpu_set_model_transform (vm_t *vm, nu_u32_t index, const nu_f32_t *m)
{
    NU_CHECK(check_model(vm, index), return NU_FAILURE);
    vm->gpu.models[index].local_to_parent = nu_m4(m);
    return NU_SUCCESS;
}
nu_status_t
gpu_set_model_parent (vm_t *vm, nu_u32_t index, nu_u32_t parent)
{
    NU_CHECK(check_model(vm, index), return NU_FAILURE);
    gpu_model_t *m = vm->gpu.models + index;
    if (parent != (nu_u32_t)-1)
    {
        check_model(vm, parent);
        gpu_model_t *p = vm->gpu.models + parent;
        m->sibling     = p->child;
        p->child       = index;
    }
    return NU_SUCCESS;
}

void
gpu_push_transform (vm_t *vm, gpu_transform_t transform, const nu_f32_t *m)
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
    os_gpu_push_transform(vm, transform);
}
void
gpu_draw_model (vm_t *vm, nu_u32_t index)
{
    check_model(vm, index);
    os_gpu_draw_model(vm, index);
}

nu_u32_t
gpu_vertex_size (gpu_vertex_attribute_t attributes)
{
    nu_u32_t vertex_stride = 0;
    vertex_stride += attributes & GPU_VERTEX_POSTIION ? NU_V3_SIZE : 0;
    vertex_stride += attributes & GPU_VERTEX_UV ? NU_V2_SIZE : 0;
    vertex_stride += attributes & GPU_VERTEX_COLOR ? NU_V3_SIZE : 0;
    return vertex_stride;
}
nu_u32_t
gpu_vertex_offset (gpu_vertex_attribute_t attributes,
                   gpu_vertex_attribute_t attribute)
{
    NU_ASSERT(attribute & attributes);
    nu_u32_t offset = 0;
    if (attributes & GPU_VERTEX_POSTIION)
    {
        if (attribute == GPU_VERTEX_POSTIION)
        {
            return offset;
        }
        offset += NU_V3_SIZE;
    }
    if (attributes & GPU_VERTEX_UV)
    {
        if (attribute == GPU_VERTEX_UV)
        {
            return offset;
        }
        offset += NU_V2_SIZE;
    }
    if (attributes & GPU_VERTEX_COLOR)
    {
        if (attribute == GPU_VERTEX_COLOR)
        {
            return offset;
        }
        offset += NU_V2_SIZE;
    }
    return offset;
}
nu_u32_t
gpu_texture_memsize (gpu_texture_size_t size)
{
    nu_u32_t width = gpu_texture_width(size);
    return width * width * 4;
}
nu_u32_t
gpu_texture_width (gpu_texture_size_t size)
{
    return (64 << size);
}
