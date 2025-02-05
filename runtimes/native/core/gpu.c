#include "gpu.h"

#include "vm.h"
#include "platform.h"

static nu_status_t
check_pool (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_POOL || vm->gpu.pools[index].addr == GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid or inactive pool %d", index);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static void *
mesh_data (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MESH || vm->gpu.meshes[index].addr == GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid or inactive mesh %d", index);
        return NU_NULL;
    }
    return vm->gpu.vram + vm->gpu.meshes[index].addr;
}
static void *
texture_data (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_TEXTURE
        || vm->gpu.textures[index].addr == GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid or inactive texture %d", index);
        return NU_NULL;
    }
    return vm->gpu.vram + vm->gpu.textures[index].addr;
}
static void *
model_data (vm_t *vm, nu_u32_t index)
{
    if (index >= GPU_MAX_MODEL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid model %d", index);
        return NU_NULL;
    }
    return vm->gpu.vram + vm->gpu.models[index].addr;
}
static gpu_addr_t
pool_malloc (vm_t *vm, nu_u32_t n)
{
    if (vm->gpu.state.pool == GPU_GLOBAL_POOL)
    {
        return gpu_malloc(vm, n);
    }
    else
    {
        gpu_pool_t *pool = vm->gpu.pools + vm->gpu.state.pool;
        if (pool->size + n >= pool->capa)
        {
            vm_log(vm,
                   NU_LOG_ERROR,
                   "out of pool memory (requested %d on pool %d)",
                   n,
                   vm->gpu.state.pool);
            return GPU_ADDR_NULL;
        }
        gpu_addr_t addr = pool->addr + pool->size;
        pool->size += n;
        return addr;
    }
}

nu_status_t
gpu_init (vm_t *vm, const gpu_config_t *config)
{
    for (nu_size_t i = 0; i < GPU_MAX_POOL; ++i)
    {
        vm->gpu.pools[i].addr = GPU_ADDR_NULL;
    }
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        vm->gpu.textures[i].addr = GPU_ADDR_NULL;
    }
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        vm->gpu.meshes[i].addr = GPU_ADDR_NULL;
    }
    for (nu_size_t i = 0; i < GPU_MAX_MODEL; ++i)
    {
        vm->gpu.models[i].addr = GPU_ADDR_NULL;
    }
    vm->gpu.vram = os_malloc(vm, config->vram_capacity);
    NU_ASSERT(vm->gpu.vram);
    vm->gpu.vram_capa        = config->vram_capacity;
    vm->gpu.vram_size        = 0;
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
        if (vm->gpu.textures[i].addr != GPU_ADDR_NULL)
        {
            os_gpu_free_texture(vm, i);
            vm->gpu.textures[i].addr = GPU_ADDR_NULL;
        }
    }
    for (nu_size_t i = 0; i < GPU_MAX_MESH; ++i)
    {
        if (vm->gpu.meshes[i].addr != GPU_ADDR_NULL)
        {
            os_gpu_free_mesh(vm, i);
            vm->gpu.meshes[i].addr = GPU_ADDR_NULL;
        }
    }
    return NU_SUCCESS;
}
void
gpu_begin_frame (vm_t *vm)
{
    os_gpu_begin_frame(vm);
}
void
gpu_end_frame (vm_t *vm)
{
    os_gpu_end_frame(vm);
}
gpu_addr_t
gpu_malloc (vm_t *vm, nu_u32_t n)
{
    if (vm->gpu.vram_size + n >= vm->gpu.vram_capa)
    {
        vm_log(vm, NU_LOG_ERROR, "out of gpu memory");
        return NU_NULL;
    }
    gpu_addr_t addr = vm->gpu.vram_size;
    vm->gpu.vram_size += n;
    return addr;
}

nu_status_t
gpu_alloc_pool (vm_t *vm, nu_u32_t index, nu_u32_t size)
{
    if (index >= GPU_MAX_POOL || vm->gpu.pools[index].addr != GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Pool %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    gpu_addr_t addr           = pool_malloc(vm, size);
    vm->gpu.pools[index].addr = addr;
    vm->gpu.pools[index].size = 0;
    vm->gpu.pools[index].capa = size;
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
    if (index >= GPU_MAX_TEXTURE
        || vm->gpu.textures[index].addr != GPU_ADDR_NULL)
    {
        vm_log(
            vm, NU_LOG_ERROR, "Texture %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    if (size < GPU_MIN_TEXTURE_SIZE || size > GPU_MAX_TEXTURE_SIZE)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Invalid texture size %d (min %d max %d)",
               GPU_MIN_TEXTURE_SIZE,
               GPU_MAX_TEXTURE_SIZE,
               size);
        return NU_FAILURE;
    }
    gpu_addr_t addr = pool_malloc(vm, gpu_texture_memsize(size));
    NU_CHECK(addr != GPU_ADDR_NULL, return NU_FAILURE);
    vm->gpu.textures[index].addr = addr;
    vm->gpu.textures[index].size = size;
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
    nu_byte_t     *data    = texture_data(vm, index);
    gpu_texture_t *texture = vm->gpu.textures + index;
    NU_ASSERT(x + w <= texture->size);
    NU_ASSERT(y + h <= texture->size);
    for (nu_size_t i = 0; i < h; ++i)
    {
        nu_byte_t *row = data + texture->size * (y + i) + x;
        nu_byte_t *src = ((nu_byte_t *)p) + w * i;
        nu_memcpy(row, src, w);
    }
    os_gpu_update_texture(vm, index);
    return NU_SUCCESS;
}
nu_status_t
gpu_alloc_mesh (vm_t                  *vm,
                nu_u32_t               index,
                nu_u32_t               count,
                gpu_primitive_t        primitive,
                gpu_vertex_attribute_t attributes)
{
    if (index >= GPU_MAX_MESH || vm->gpu.meshes[index].addr != GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Mesh %d already allocated or invalid", index);
        return NU_FAILURE;
    }
    gpu_addr_t addr = pool_malloc(vm, gpu_vertex_memsize(attributes, count));
    NU_CHECK(addr != GPU_ADDR_NULL, return NU_FAILURE);
    vm->gpu.meshes[index].addr       = addr;
    vm->gpu.meshes[index].count      = count;
    vm->gpu.meshes[index].primitive  = primitive;
    vm->gpu.meshes[index].attributes = attributes;
    nu_memset(mesh_data(vm, index), 0, gpu_vertex_memsize(attributes, count));
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
    nu_f32_t *ptr = mesh_data(vm, index);
    NU_CHECK(ptr, return NU_FAILURE);
    gpu_mesh_t     *mesh = vm->gpu.meshes + index;
    const nu_f32_t *data = p;
    if (attributes & GPU_VERTEX_POSITION
        && mesh->attributes & GPU_VERTEX_POSITION)
    {
        nu_u32_t src_offset
            = gpu_vertex_offset(attributes, GPU_VERTEX_POSITION, count);
        nu_u32_t dst_offset = gpu_vertex_offset(
            mesh->attributes, GPU_VERTEX_POSITION, mesh->count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 3 + 0]
                = data[src_offset + i * 3 + 0];
            ptr[dst_offset + (first + i) * 3 + 1]
                = data[src_offset + i * 3 + 1];
            ptr[dst_offset + (first + i) * 3 + 2]
                = data[src_offset + i * 3 + 2];
        }
    }
    if (attributes & GPU_VERTEX_UV && mesh->attributes & GPU_VERTEX_UV)
    {
        nu_u32_t src_offset
            = gpu_vertex_offset(attributes, GPU_VERTEX_UV, count);
        nu_u32_t dst_offset
            = gpu_vertex_offset(mesh->attributes, GPU_VERTEX_UV, mesh->count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 2 + 0]
                = data[src_offset + i * 2 + 0];
            ptr[dst_offset + (first + i) * 2 + 1]
                = data[src_offset + i * 2 + 1];
        }
    }
    if (attributes & GPU_VERTEX_COLOR && mesh->attributes & GPU_VERTEX_COLOR)
    {
        nu_u32_t src_offset
            = gpu_vertex_offset(attributes, GPU_VERTEX_COLOR, count);
        nu_u32_t dst_offset = gpu_vertex_offset(
            mesh->attributes, GPU_VERTEX_COLOR, mesh->count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 3 + 0]
                = data[src_offset + i * 3 + 0];
            ptr[dst_offset + (first + i) * 3 + 1]
                = data[src_offset + i * 3 + 1];
            ptr[dst_offset + (first + i) * 3 + 2]
                = data[src_offset + i * 3 + 2];
        }
    }
    os_gpu_update_mesh(vm, index);
    return NU_SUCCESS;
}

nu_status_t
gpu_alloc_model (vm_t *vm, nu_u32_t index, nu_u32_t node_count)
{
    if (index >= GPU_MAX_MODEL || vm->gpu.models[index].addr != GPU_ADDR_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid or inactive model %d", index);
        return NU_FAILURE;
    }
    nu_u32_t   memsize = node_count * sizeof(gpu_model_node_t);
    gpu_addr_t addr    = pool_malloc(vm, memsize);
    NU_CHECK(addr != GPU_ADDR_NULL, return NU_FAILURE);
    vm->gpu.models[index].addr       = addr;
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
    NU_CHECK(model_data(vm, index), return NU_FAILURE);
    if (node_index >= vm->gpu.models[index].node_count)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid model node index %d", node_index);
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
    NU_CHECK(model_data(vm, index), return);
    os_gpu_draw_model(vm, index);
}
void
gpu_draw_text (vm_t *vm, nu_u32_t x, nu_u32_t y, const void *text)
{
    os_gpu_draw_text(vm, x, y, text, nu_strlen(text));
}

nu_u32_t
gpu_texture_memsize (nu_u32_t size)
{
    return size * size * 4;
}
nu_u32_t
gpu_vertex_memsize (gpu_vertex_attribute_t attributes, nu_u32_t count)
{
    nu_u32_t size = 0;
    if (attributes & GPU_VERTEX_POSITION)
    {
        size += NU_V3_SIZE * count;
    }
    if (attributes & GPU_VERTEX_UV)
    {
        size += NU_V2_SIZE * count;
    }
    if (attributes & GPU_VERTEX_COLOR)
    {
        size += NU_V3_SIZE * count;
    }
    return size * sizeof(nu_f32_t);
}
nu_u32_t
gpu_vertex_offset (gpu_vertex_attribute_t attributes,
                   gpu_vertex_attribute_t attribute,
                   nu_u32_t               count)
{
    NU_ASSERT(attribute & attributes);
    nu_u32_t offset = 0;
    if (attributes & GPU_VERTEX_POSITION)
    {
        if (attribute == GPU_VERTEX_POSITION)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    if (attributes & GPU_VERTEX_UV)
    {
        if (attribute == GPU_VERTEX_UV)
        {
            return offset;
        }
        offset += NU_V2_SIZE * count;
    }
    if (attributes & GPU_VERTEX_COLOR)
    {
        if (attribute == GPU_VERTEX_COLOR)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    return offset;
}
