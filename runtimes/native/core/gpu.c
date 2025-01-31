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
        vm->gpu.models[i].active = NU_FALSE;
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
    iou_log(vm, NU_LOG_INFO, "texture[%d] size:%d", index, size);
    // TODO: validate size
    nu_size_t data_length = gpu_texture_memsize(size);
    NU_CHECK(iou_read(vm, vm->iou.heap, data_length), return NU_FAILURE);
    gpu_alloc_texture(vm, index, size);
    gpu_update_texture(vm, index, 0, 0, size, size, vm->iou.heap);
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
    iou_log(vm,
            NU_LOG_INFO,
            "mesh[%d] count:%d, primitive:%d, attribute:%d",
            index,
            count,
            primitive,
            attributes);
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
    nu_u32_t index, node_count;
    NU_CHECK(iou_read_u32(vm, &index), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &node_count), return NU_FAILURE);
    NU_CHECK(gpu_alloc_model(vm, index, node_count), return NU_FAILURE);
    iou_log(vm, NU_LOG_INFO, "model[%d] node_count:%d", index, node_count);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_u32_t mesh, texture, parent;
        nu_m4_t  transform;
        NU_CHECK(iou_read_u32(vm, &mesh), return NU_FAILURE);
        NU_CHECK(iou_read_u32(vm, &texture), return NU_FAILURE);
        NU_CHECK(iou_read_u32(vm, &parent), return NU_FAILURE);
        NU_CHECK(iou_read_m4(vm, &transform), return NU_FAILURE);
        iou_log(vm,
                NU_LOG_INFO,
                "   node:%d, mesh:%d, texture:%d, parent:%d",
                i,
                mesh,
                texture,
                parent);
        NU_CHECK(gpu_update_model(
                     vm, index, i, mesh, texture, parent, transform.data),
                 return NU_FAILURE);
    }

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
gpu_alloc_texture (vm_t *vm, nu_u32_t index, nu_u32_t size)
{
    if (index >= GPU_MAX_TEXTURE || vm->gpu.textures[index].active)
    {
        iou_log(
            vm, NU_LOG_ERROR, "Texture %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    if (size < GPU_MIN_TEXTURE_SIZE || size > GPU_MAX_TEXTURE_SIZE)
    {
        iou_log(vm,
                NU_LOG_ERROR,
                "Invalid texture size %d (min %d max %d)",
                GPU_MIN_TEXTURE_SIZE,
                GPU_MAX_TEXTURE_SIZE,
                size);
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
    NU_CHECK(reserve_memory(vm, gpu_vertex_memsize(attributes, count)),
             return NU_FAILURE);
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
gpu_alloc_model (vm_t *vm, nu_u32_t index, nu_u32_t node_count)
{
    if (index >= GPU_MAX_MODEL || vm->gpu.models[index].active)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid or inactive model %d", index);
        return NU_FAILURE;
    }
    nu_u32_t memsize = node_count * sizeof(gpu_model_node_t);
    NU_CHECK(reserve_memory(vm, memsize), return NU_FAILURE);
    vm->gpu.models[index].active     = NU_TRUE;
    vm->gpu.models[index].node_count = node_count;
    os_gpu_init_model(vm, index);
    return NU_SUCCESS;
}
nu_status_t
gpu_update_model (vm_t           *vm,
                  nu_u32_t        index,
                  nu_u32_t        node_index,
                  nu_u32_t        mesh,
                  nu_u32_t        texture,
                  nu_u32_t        parent,
                  const nu_f32_t *transform)
{
    NU_CHECK(check_model(vm, index), return NU_FAILURE);
    if (node_index >= vm->gpu.models[index].node_count)
    {
        iou_log(vm, NU_LOG_ERROR, "Invalid model node index %d", node_index);
        return NU_FAILURE;
    }
    gpu_model_node_t node = { .texture         = texture,
                              .mesh            = mesh,
                              .parent          = parent,
                              .local_to_parent = nu_m4(transform) };
    os_gpu_update_model(vm, index, node_index, &node);
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
    NU_CHECK(check_model(vm, index), return);
    os_gpu_draw_model(vm, index);
}

nu_u32_t
gpu_vertex_memsize (gpu_vertex_attribute_t attributes, nu_u32_t count)
{
    return gpu_vertex_size(attributes) * sizeof(nu_f32_t) * count;
}
nu_u32_t
gpu_vertex_size (gpu_vertex_attribute_t attributes)
{
    nu_u32_t vertex_stride = 0;
    vertex_stride += attributes & GPU_VERTEX_POSITION ? NU_V3_SIZE : 0;
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
    if (attributes & GPU_VERTEX_POSITION)
    {
        if (attribute == GPU_VERTEX_POSITION)
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
gpu_texture_memsize (nu_u32_t size)
{
    return size * size * 4;
}
