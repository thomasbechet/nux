#include "internal.h"

static nu_size_t
cart_read (nux_instance_t inst, void *p, nu_size_t n)
{
    return nux_platform_read(inst, p, n);
}
static nux_status_t
cart_read_u32 (nux_instance_t inst, nu_u32_t *v)
{
    NU_CHECK(nux_platform_read(inst, v, sizeof(*v)) == sizeof(*v),
             return NUX_FAILURE);
    *v = nu_u32_le(*v);
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_f32 (nux_instance_t inst, nu_f32_t *v)
{
    NU_CHECK(nux_platform_read(inst, v, sizeof(*v)) == sizeof(*v),
             return NUX_FAILURE);
    *v = nu_f32_le(*v);
    return NUX_SUCCESS;
}
nux_status_t
cart_read_m4 (nux_instance_t inst, nu_m4_t *v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(inst, &v->data[i]), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}

nux_status_t
cart_parse_header (const void *data, nux_cart_header_t *header)
{
    const nu_byte_t *ptr = data;
    header->version      = nu_u32_le(*((nu_u32_t *)ptr));
    ptr += sizeof(nu_u32_t);
    header->chunk_count = nu_u32_le(*((nu_u32_t *)ptr));
    // TODO: validate
    return NUX_SUCCESS;
}
nux_status_t
cart_parse_entries (const void             *data,
                    nu_u32_t                count,
                    nux_cart_chunk_entry_t *entries)
{
    const nu_byte_t *ptr = data;
    for (nu_size_t i = 0; i < count; ++i)
    {
        // Type
        nux_cart_chunk_entry_t *entry = entries + i;
        entry->type                   = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Name
        entry->oid = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Offset
        entry->offset = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Length
        entry->length = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
    }
    return NUX_SUCCESS;
}

static nux_status_t
load_wasm (nux_env_t env, const nux_cart_chunk_entry_t *entry)
{
    // Load module data
    NU_ASSERT(entry->length);
    nu_byte_t *buffer
        = nux_platform_malloc(env->inst, NUX_MEMORY_USAGE_CORE, entry->length);
    NU_ASSERT(buffer);
    NU_CHECK(nux_platform_read(env->inst, buffer, entry->length),
             return NUX_FAILURE);
    // NU_CHECK(os_cpu_load_wasm(inst, buffer, entry->length), return
    // NU_FAILURE);
    env->inst->wasm.loaded = NU_TRUE;
    return NUX_SUCCESS;
}
static nux_status_t
load_texture (nux_env_t env, const nux_cart_chunk_entry_t *entry)
{
    nu_u32_t size;
    NU_CHECK(cart_read_u32(env->inst, &size), return NUX_FAILURE);
    // TODO: validate size
    nu_size_t data_length = nux_texture_memsize(size);
    NU_CHECK(nux_create_texture(env, entry->oid, size), return NUX_FAILURE);
    nux_object_t *obj = env->inst->objects + entry->oid;
    NU_CHECK(
        cart_read(env->inst, env->inst->mem + obj->texture.data, data_length),
        return NUX_FAILURE);
    return NUX_SUCCESS;
}
static nux_status_t
load_mesh (nux_env_t env, const nux_cart_chunk_entry_t *entry)
{
    nu_u32_t count, primitive, attributes;
    NU_CHECK(cart_read_u32(env->inst, &count), return NUX_FAILURE);
    NU_CHECK(cart_read_u32(env->inst, &primitive), return NUX_FAILURE);
    NU_CHECK(cart_read_u32(env->inst, &attributes), return NUX_FAILURE);
    NU_CHECK(nux_create_mesh(env, entry->oid, count, primitive, attributes),
             return NUX_FAILURE);
    nux_object_t *obj = env->inst->objects + entry->oid;
    NU_ASSERT(nux_platform_read(env->inst,
                                env->inst->mem + obj->mesh.data,
                                nux_vertex_memsize(attributes, count)));
    return NUX_SUCCESS;
}
static nux_status_t
load_scene (nux_env_t env, const nux_cart_chunk_entry_t *entry)
{
    nu_u32_t node_count;
    NU_CHECK(cart_read_u32(env->inst, &node_count), return NUX_FAILURE);
    NU_CHECK(nux_create_scene(env, entry->oid, node_count), return NUX_FAILURE);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_u32_t mesh, texture, parent;
        nu_m4_t  transform;
        NU_CHECK(cart_read_u32(env->inst, &mesh), return NUX_FAILURE);
        NU_CHECK(cart_read_u32(env->inst, &texture), return NUX_FAILURE);
        NU_CHECK(cart_read_u32(env->inst, &parent), return NUX_FAILURE);
        NU_CHECK(cart_read_m4(env->inst, &transform), return NUX_FAILURE);
        // NU_CHECK(sys_update_model(
        //              inst, entry->id, i, mesh, texture, parent,
        //              transform.data),
        //          return NUX_FAILURE);
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_load_cartridge (nux_instance_t inst, const nu_char_t *name)
{
    nux_status_t status = NUX_SUCCESS;

    // Create bios env
    nux_env_t env = nux_instance_init_env(inst);

    nux_platform_mount(inst, name, nu_strnlen(name, NUX_NAME_MAX));
    nux_platform_seek(inst, 0);

    nu_byte_t header_data[NUX_CART_HEADER_SIZE];
    NU_CHECK(cart_read(inst, header_data, sizeof(header_data))
                 == sizeof(header_data),
             return NUX_FAILURE);
    nux_cart_header_t header;
    NU_CHECK(cart_parse_header(header_data, &header), return NUX_FAILURE);

    nu_u32_t data_offset
        = NUX_CART_HEADER_SIZE + NUX_CART_CHUNK_ENTRY_SIZE * header.chunk_count;

    // Load chunks
    for (nu_u32_t i = 0; i < header.chunk_count; ++i)
    {
        // Seek to entry
        nux_platform_seek(inst,
                          NUX_CART_HEADER_SIZE + NUX_CART_CHUNK_ENTRY_SIZE * i);

        // Read entry
        nux_cart_chunk_entry_t entry;
        nu_byte_t              entry_data[NUX_CART_CHUNK_ENTRY_SIZE];
        NU_CHECK(cart_read(inst, entry_data, sizeof(entry_data))
                     == sizeof(entry_data),
                 return NUX_FAILURE);

        if (!cart_parse_entries(entry_data, 1, &entry))
        {
            return NUX_FAILURE;
        }

        // Seek to chunk
        nux_platform_seek(inst, data_offset + entry.offset);

        // Read chunk
        switch (entry.type)
        {
            case NUX_OBJECT_RAW:
                break;
            case NUX_OBJECT_WASM:
                status = load_wasm(env, &entry);
                break;
            case NUX_OBJECT_TEXTURE:
                status = load_texture(env, &entry);
                break;
            case NUX_OBJECT_MESH:
                status = load_mesh(env, &entry);
                break;
            case NUX_OBJECT_SCENE:
                status = load_scene(env, &entry);
                break;
            default: {
                return NUX_FAILURE;
            }
        }
        if (!status)
        {
            return NUX_FAILURE;
        }
    }

    return status;
}
