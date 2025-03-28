#include "platform.h"

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
    vm_log(vm, NU_LOG_DEBUG, "texture %u, size:%d", entry->id, size);
    // TODO: validate size
    nu_size_t data_length = gfx_texture_memsize(size);
    NU_CHECK(sys_create_texture(vm, entry->id, size), return NU_FAILURE);
    resource_t *res = vm->res + entry->id;
    NU_CHECK(cart_read(vm, vm->mem + res->texture.data, data_length),
             return NU_FAILURE);
    os_gfx_update_texture(vm, entry->id);
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
           NU_LOG_DEBUG,
           "mesh %u, count:%d, primitive:%d, attribute:%d",
           entry->id,
           count,
           primitive,
           attributes);
    NU_CHECK(sys_create_mesh(vm, entry->id, count, primitive, attributes),
             return NU_FAILURE);
    resource_t *res = vm->res + entry->id;
    NU_ASSERT(os_cart_read(
        vm, vm->mem + res->mesh.data, gfx_vertex_memsize(attributes, count)));
    os_gfx_update_mesh(vm, entry->id);
    return NU_SUCCESS;
}
static nu_status_t
load_model (vm_t *vm, const cart_chunk_entry_t *entry)
{
    nu_u32_t node_count;
    NU_CHECK(cart_read_u32(vm, &node_count), return NU_FAILURE);
    NU_CHECK(sys_create_model(vm, entry->id, node_count), return NU_FAILURE);
    vm_log(vm, NU_LOG_DEBUG, "model %u, node_count:%d", entry->id, node_count);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_u32_t mesh, texture, parent;
        nu_m4_t  transform;
        NU_CHECK(cart_read_u32(vm, &mesh), return NU_FAILURE);
        NU_CHECK(cart_read_u32(vm, &texture), return NU_FAILURE);
        NU_CHECK(cart_read_u32(vm, &parent), return NU_FAILURE);
        NU_CHECK(cart_read_m4(vm, &transform), return NU_FAILURE);
        vm_log(vm,
               NU_LOG_DEBUG,
               "   node:%d, mesh:%u, texture:%u, parent:%d",
               i,
               mesh,
               texture,
               parent);
        NU_CHECK(sys_update_model(
                     vm, entry->id, i, mesh, texture, parent, transform.data),
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
            case RESOURCE_RAW:
                break;
            case RESOURCE_WASM:
                status = load_wasm(vm, &entry);
                break;
            case RESOURCE_TEXTURE:
                status = load_texture(vm, &entry);
                break;
            case RESOURCE_MESH:
                status = load_mesh(vm, &entry);
                break;
            case RESOURCE_MODEL:
                status = load_model(vm, &entry);
                break;
            default: {
                vm_log(
                    vm, NU_LOG_ERROR, "Invalid resource type %d", entry.type);
                return NU_FAILURE;
            }
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
