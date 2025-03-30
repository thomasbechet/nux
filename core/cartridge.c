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
cart_read_v3 (nux_instance_t inst, nu_v3_t *v)
{
    for (nu_size_t i = 0; i < NU_V3_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(inst, &v->data[i]), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
cart_read_q4 (nux_instance_t inst, nu_q4_t *v)
{
    for (nu_size_t i = 0; i < NU_Q4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(inst, &v->data[i]), return NUX_FAILURE);
    }
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
nux_cart_parse_header (const void *data, nux_cart_header_t *header)
{
    const nu_byte_t *ptr = data;
    header->version      = nu_u32_le(*((nu_u32_t *)ptr));
    ptr += sizeof(nu_u32_t);
    header->object_count = nu_u32_le(*((nu_u32_t *)ptr));
    // TODO: validate
    return NUX_SUCCESS;
}
nux_status_t
nux_cart_parse_entries (const void              *data,
                        nux_u32_t                count,
                        nux_cart_object_entry_t *entries)
{
    const nu_byte_t *ptr = data;
    for (nu_size_t i = 0; i < count; ++i)
    {
        // Type
        nux_cart_object_entry_t *entry = entries + i;
        entry->type                    = nu_u32_le(*(nu_u32_t *)ptr);
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
load_wasm (nux_env_t env, const nux_cart_object_entry_t *entry)
{
    // Load module data
    NU_ASSERT(entry->length);
    nu_byte_t *buffer
        = nux_platform_malloc(env->inst, NUX_MEMORY_USAGE_CORE, entry->length);
    NU_ASSERT(buffer);
    NU_CHECK(nux_platform_read(env->inst, buffer, entry->length),
             return NUX_FAILURE);
    NU_CHECK(nux_wasm_load(env->inst, buffer, entry->length),
             return NUX_FAILURE);
    return NUX_SUCCESS;
}
static nux_status_t
load_texture (nux_env_t env, const nux_cart_object_entry_t *entry)
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
load_mesh (nux_env_t env, const nux_cart_object_entry_t *entry)
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
load_scene (nux_env_t env, const nux_cart_object_entry_t *entry)
{
    nu_u32_t node_count;
    NU_CHECK(cart_read_u32(env->inst, &node_count), return NUX_FAILURE);
    NU_CHECK(nux_create_scene(env, entry->oid, node_count), return NUX_FAILURE);
    NU_CHECK(nux_bind_scene(env, entry->oid), return NUX_FAILURE);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nux_nid_t parent;
        nu_v3_t   position;
        nu_q4_t   rotation;
        nu_v3_t   scale;
        nux_u32_t components;

        NU_CHECK(cart_read_u32(env->inst, &parent), return NUX_FAILURE);
        NU_CHECK(cart_read_v3(env->inst, &position), return NUX_FAILURE);
        NU_CHECK(cart_read_q4(env->inst, &rotation), return NUX_FAILURE);
        NU_CHECK(cart_read_v3(env->inst, &scale), return NUX_FAILURE);
        NU_CHECK(cart_read_u32(env->inst, &components), return NUX_FAILURE);

        nux_nid_t node = nux_node_add(env, parent);
        NU_CHECK(node, return NUX_FAILURE);
        nux_node_set_translation(env, node, position.data);
        nux_node_set_rotation(env, node, rotation.data);
        nux_node_set_scale(env, node, scale.data);

        if (components & NUX_COMPONENT_MODEL)
        {
            nux_oid_t mesh, texture, parent;
            NU_CHECK(cart_read_u32(env->inst, &mesh), return NUX_FAILURE);
            NU_CHECK(cart_read_u32(env->inst, &texture), return NUX_FAILURE);
            NU_CHECK(nux_model_add(env, node, mesh, texture),
                     return NUX_FAILURE);
        }
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_load_cartridge (nux_instance_t inst, const nux_c8_t *cart, nux_u32_t n)
{
    nux_status_t status = NUX_SUCCESS;

    // Create bios env
    nux_env_t env = nux_instance_init_env(inst);

    nux_platform_mount(inst, cart, n);
    nux_platform_seek(inst, 0);

    nu_byte_t header_data[NUX_CART_HEADER_SIZE];
    NU_CHECK(cart_read(inst, header_data, sizeof(header_data))
                 == sizeof(header_data),
             return NUX_FAILURE);
    nux_cart_header_t header;
    NU_CHECK(nux_cart_parse_header(header_data, &header), return NUX_FAILURE);

    nu_u32_t data_offset = NUX_CART_HEADER_SIZE
                           + NUX_CART_OBJECT_ENTRY_SIZE * header.object_count;

    // Load objects
    for (nu_u32_t i = 0; i < header.object_count; ++i)
    {
        // Seek to entry
        nux_platform_seek(
            inst, NUX_CART_HEADER_SIZE + NUX_CART_OBJECT_ENTRY_SIZE * i);

        // Read entry
        nux_cart_object_entry_t entry;
        nu_byte_t               entry_data[NUX_CART_OBJECT_ENTRY_SIZE];
        NU_CHECK(cart_read(inst, entry_data, sizeof(entry_data))
                     == sizeof(entry_data),
                 return NUX_FAILURE);

        if (!nux_cart_parse_entries(entry_data, 1, &entry))
        {
            return NUX_FAILURE;
        }

        // Seek to object
        nux_platform_seek(inst, data_offset + entry.offset);

        // Read object
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
