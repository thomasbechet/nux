#include "bios.h"

#include "cartridge.h"
#include "vm.h"
#include "platform.h"

static NU_ENUM_MAP(cart_chunk_type_map,
                   NU_ENUM_NAME(CART_CHUNK_RAW, "raw"),
                   NU_ENUM_NAME(CART_CHUNK_MESH, "mesh"),
                   NU_ENUM_NAME(CART_CHUNK_WASM, "wasm"),
                   NU_ENUM_NAME(CART_CHUNK_TEXTURE, "texture"),
                   NU_ENUM_NAME(CART_CHUNK_MODEL, "model"));

static nu_status_t
load_wasm (vm_t *vm, const cart_chunk_entry_t *entry)
{
    // Load module data
    NU_ASSERT(entry->length);
    nu_byte_t *buffer = os_malloc(vm, entry->length);
    NU_ASSERT(buffer);
    NU_CHECK(os_cart_read(vm, buffer, entry->length), return NU_FAILURE);
    NU_CHECK(os_cpu_load_wasm(vm, buffer, entry->length), return NU_FAILURE);
    vm->wasm.loaded = NU_TRUE;
    return NU_SUCCESS;
}
static nu_status_t
load_texture (vm_t *vm, const cart_chunk_entry_t *entry)
{
    nu_u32_t size;
    NU_CHECK(cart_read_u32(vm, &size), return NU_FAILURE);
    vm_log(vm, NU_LOG_INFO, "texture hash:0x%x, size:%d", entry->hash, size);
    // TODO: validate size
    nu_size_t data_length = gfx_texture_memsize(size);
    nu_u32_t  id          = sys_add_texture(vm, size);
    NU_CHECK(id, return NU_FAILURE);
    resource_t *res = vm->res + ID_TO_INDEX(id);
    res->hash       = entry->hash;
    NU_CHECK(cart_read(vm, vm->mem + res->texture.data, data_length),
             return NU_FAILURE);
    os_gpu_update_texture(vm, id);
    return NU_SUCCESS;
}
static nu_status_t
load_mesh (vm_t *vm, const cart_chunk_entry_t *entry)
{
    nu_u32_t count, primitive, attributes;
    NU_CHECK(cart_read_u32(vm, &count), return NU_FAILURE);
    NU_CHECK(cart_read_u32(vm, &primitive), return NU_FAILURE);
    NU_CHECK(cart_read_u32(vm, &attributes), return NU_FAILURE);
    vm_log(vm,
           NU_LOG_INFO,
           "mesh hash:0x%x, count:%d, primitive:%d, attribute:%d",
           entry->hash,
           count,
           primitive,
           attributes);
    nu_u32_t id = sys_add_mesh(vm, count, primitive, attributes);
    NU_CHECK(id, return NU_FAILURE);
    resource_t *res = vm->res + ID_TO_INDEX(id);
    res->hash       = entry->hash;
    NU_ASSERT(os_cart_read(
        vm, vm->mem + res->mesh.data, gfx_vertex_memsize(attributes, count)));
    os_gpu_update_mesh(vm, id);
    return NU_SUCCESS;
}
static nu_status_t
load_model (vm_t *vm, const cart_chunk_entry_t *entry)
{
    nu_u32_t node_count;
    NU_CHECK(cart_read_u32(vm, &node_count), return NU_FAILURE);
    nu_u32_t id                   = sys_add_model(vm, node_count);
    vm->res[ID_TO_INDEX(id)].hash = entry->hash;
    NU_CHECK(id, return NU_FAILURE);
    vm_log(vm,
           NU_LOG_INFO,
           "model hash:0x%x, node_count:%d",
           entry->hash,
           node_count);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_u32_t mesh, texture, parent;
        nu_m4_t  transform;
        NU_CHECK(cart_read_u32(vm, &mesh), return NU_FAILURE);
        NU_CHECK(cart_read_u32(vm, &texture), return NU_FAILURE);
        NU_CHECK(cart_read_u32(vm, &parent), return NU_FAILURE);
        NU_CHECK(cart_read_m4(vm, &transform), return NU_FAILURE);
        nu_u32_t mesh_id = sys_find_hash(vm, mesh);
        if (!mesh_id)
        {
            vm_log(vm, NU_LOG_ERROR, "mesh 0x%x not found", mesh);
            return NU_FAILURE;
        }
        nu_u32_t texture_id = 0;
        if (texture)
        {
            texture_id = sys_find_hash(vm, texture);
            if (!texture_id)
            {
                vm_log(vm, NU_LOG_ERROR, "texture 0x%x not found", texture);
                return NU_FAILURE;
            }
        }
        vm_log(vm,
               NU_LOG_INFO,
               "   node:%d, mesh:0x%x, texture:0x%x, parent:%d",
               i,
               mesh,
               texture,
               parent);
        NU_CHECK(sys_write_model(
                     vm, id, i, mesh_id, texture_id, parent, transform.data),
                 return NU_FAILURE);
    }

    return NU_SUCCESS;
}

nu_status_t
bios_load_cart (vm_t *vm, const nu_char_t *name)
{
    nu_status_t status = NU_SUCCESS;

    vm_log(vm, NU_LOG_INFO, "Loading cartridge %s", name);

    // Load cart header
    if (!os_cart_mount(vm, name))
    {
        vm_log(vm, NU_LOG_ERROR, "Failed to mount cartridge");
        return NU_FAILURE;
    }
    NU_CHECK(os_cart_seek(vm, 0), return NU_FAILURE);

    nu_byte_t header_data[CART_HEADER_SIZE];
    NU_CHECK(cart_read(vm, header_data, sizeof(header_data))
                 == sizeof(header_data),
             return NU_FAILURE);
    cart_header_t header;
    NU_CHECK(cart_parse_header(header_data, &header), return NU_FAILURE);

    nu_u32_t data_offset
        = CART_HEADER_SIZE + CART_CHUNK_ENTRY_SIZE * header.chunk_count;

    // Load chunks
    for (nu_u32_t i = 0; i < header.chunk_count; ++i)
    {
        // Seek to entry
        os_cart_seek(vm, CART_HEADER_SIZE + CART_CHUNK_ENTRY_SIZE * i);

        // Read entry
        cart_chunk_entry_t entry;
        nu_byte_t          entry_data[CART_CHUNK_ENTRY_SIZE];
        NU_CHECK(cart_read(vm, entry_data, sizeof(entry_data))
                     == sizeof(entry_data),
                 return NU_FAILURE);

        if (!cart_parse_entries(entry_data, 1, &entry))
        {
            vm_log(vm, NU_LOG_ERROR, "Failed to read chunk entry %d", i);
            return NU_FAILURE;
        }

        // Seek to chunk
        os_cart_seek(vm, data_offset + entry.offset);

        // Read chunk
        switch (entry.type)
        {
            case CART_CHUNK_RAW:
                break;
            case CART_CHUNK_WASM:
                status = load_wasm(vm, &entry);
                break;
            case CART_CHUNK_TEXTURE:
                status = load_texture(vm, &entry);
                break;
            case CART_CHUNK_MESH:
                status = load_mesh(vm, &entry);
                break;
            case CART_CHUNK_MODEL:
                status = load_model(vm, &entry);
                break;
        }
        if (!status)
        {
            vm_log(vm, NU_LOG_ERROR, "Failed to load chunk %d");
            return NU_FAILURE;
        }
    }

    vm_log(vm, NU_LOG_INFO, "Cartridge sucessfully loaded", name);

    return status;
}
