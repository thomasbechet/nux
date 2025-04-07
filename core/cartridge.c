#include "internal.h"

static nux_status_t
cart_read (nux_env_t env, void *p, nu_size_t n)
{
    if (nux_platform_read(env->inst, p, n) != n)
    {
        nux_set_error(env, NUX_ERROR_CART_EOF);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_u32 (nux_env_t env, nu_u32_t *v)
{
    NU_CHECK(cart_read(env, v, sizeof(*v)), return NUX_FAILURE);
    *v = nu_u32_le(*v);
    return NUX_SUCCESS;
}
static nux_status_t
cart_read_f32 (nux_env_t env, nu_f32_t *v)
{
    NU_CHECK(cart_read(env, v, sizeof(*v)), return NUX_FAILURE);
    *v = nu_f32_le(*v);
    return NUX_SUCCESS;
}
nux_status_t
cart_read_v3 (nux_env_t env, nu_v3_t *v)
{
    for (nu_size_t i = 0; i < NU_V3_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
cart_read_q4 (nux_env_t env, nu_q4_t *v)
{
    for (nu_size_t i = 0; i < NU_Q4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
cart_read_m4 (nux_env_t env, nu_m4_t *v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_cart_parse_header (const void *data, nux_cart_header_t *header)
{
    const nu_byte_t *ptr = data;
    header->version      = nu_u32_le(*((nu_u32_t *)ptr));
    ptr += sizeof(nu_u32_t);
    header->entry_count = nu_u32_le(*((nu_u32_t *)ptr));
    // TODO: validate
    return NUX_SUCCESS;
}
nux_status_t
nux_cart_parse_entries (const void       *data,
                        nux_u32_t         count,
                        nux_cart_entry_t *entries)
{
    const nu_byte_t *ptr = data;
    for (nu_size_t i = 0; i < count; ++i)
    {
        // Type
        nux_cart_entry_t *entry = entries + i;
        entry->type             = nu_u32_le(*(nu_u32_t *)ptr);
        ptr += sizeof(nu_u32_t);
        // Slot
        entry->slot = nu_u32_le(*(nu_u32_t *)ptr);
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
load_wasm (nux_env_t env, nux_id_t stack, const nux_cart_entry_t *entry)
{
    // Load module data
    NU_ASSERT(entry->length);
    nu_byte_t *buffer
        = nux_platform_malloc(env->inst, NUX_MEMORY_USAGE_CORE, entry->length);
    NU_ASSERT(buffer);
    NU_CHECK(nux_platform_read(env->inst, buffer, entry->length),
             return NUX_FAILURE);
    NU_CHECK(nux_wasm_load(env, buffer, entry->length), return NUX_FAILURE);
    return NUX_SUCCESS;
}
static nux_status_t
load_texture (nux_env_t env, nux_id_t stack, const nux_cart_entry_t *entry)
{
    nu_u32_t size;
    NU_CHECK(cart_read_u32(env, &size), return NUX_FAILURE);
    nux_id_t id = nux_texture_create(env, stack, size);
    NU_CHECK(id, return NUX_FAILURE);
    NU_CHECK(nux_object_slot_set(env, entry->slot, id), return NUX_FAILURE);
    nux_texture_t *texture     = nux_object_get_unchecked(env, id);
    void          *data        = nux_object_get_unchecked(env, texture->data);
    nu_size_t      data_length = nux_texture_memsize(size);
    NU_CHECK(cart_read(env, data, data_length), return NUX_FAILURE);
    return NUX_SUCCESS;
}
static nux_status_t
load_mesh (nux_env_t env, nux_id_t stack, const nux_cart_entry_t *entry)
{
    nu_u32_t count, primitive, attributes;
    NU_CHECK(cart_read_u32(env, &count), return NUX_FAILURE);
    NU_CHECK(cart_read_u32(env, &primitive), return NUX_FAILURE);
    NU_CHECK(cart_read_u32(env, &attributes), return NUX_FAILURE);
    nux_id_t id = nux_mesh_create(env, stack, count, primitive, attributes);
    NU_CHECK(id, return NUX_FAILURE);
    NU_CHECK(nux_object_slot_set(env, entry->slot, id), return NUX_FAILURE);
    nux_mesh_t *mesh = nux_object_get_unchecked(env, id);
    void       *data = nux_object_get_unchecked(env, mesh->data);
    NU_CHECK(nux_platform_read(
                 env->inst, data, nux_vertex_memsize(attributes, count)),
             return NUX_FAILURE);
    return NUX_SUCCESS;
}
static nux_status_t
load_scene (nux_env_t env, const nux_cart_entry_t *entry)
{
    nu_u32_t slab_capa, node_count;
    NU_CHECK(cart_read_u32(env, &slab_capa), return NUX_FAILURE);
    NU_CHECK(cart_read_u32(env, &node_count), return NUX_FAILURE);
    NU_CHECK(nux_scene_create(env, entry->oid, slab_capa), return NUX_FAILURE);
    NU_CHECK(nux_bind_scene(env, entry->oid), return NUX_FAILURE);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nux_nid_t parent;
        nu_v3_t   position;
        nu_q4_t   rotation;
        nu_v3_t   scale;
        nux_u32_t components;

        NU_CHECK(cart_read_u32(env, &parent), return NUX_FAILURE);
        NU_CHECK(cart_read_v3(env, &position), return NUX_FAILURE);
        NU_CHECK(cart_read_q4(env, &rotation), return NUX_FAILURE);
        NU_CHECK(cart_read_v3(env, &scale), return NUX_FAILURE);
        NU_CHECK(cart_read_u32(env, &components), return NUX_FAILURE);

        nux_nid_t node = nux_node_create(env, parent);
        NU_CHECK(node, return NUX_FAILURE);
        nux_set_node_translation(env, node, position.data);
        nux_set_node_rotation(env, node, rotation.data);
        nux_set_node_scale(env, node, scale.data);

        if (components & NUX_NODE_MODEL)
        {
            nux_oid_t mesh, texture;
            NU_CHECK(cart_read_u32(env, &mesh), return NUX_FAILURE);
            NU_CHECK(cart_read_u32(env, &texture), return NUX_FAILURE);
            NU_CHECK(nux_model_add(env, node, mesh, texture),
                     return NUX_FAILURE);
        }
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_load_cartridge (nux_env_t       env,
                    nux_id_t        stack,
                    const nux_c8_t *cart,
                    nux_u32_t       n)
{
    nux_status_t status = NUX_SUCCESS;

    if (!nux_platform_mount(env->inst, cart, n))
    {
        nux_set_error(env, NUX_ERROR_CART_MOUNT);
        return NUX_FAILURE;
    }
    nux_platform_seek(env->inst, 0);

    nu_byte_t header_data[NUX_CART_HEADER_SIZE];
    NU_CHECK(cart_read(env, header_data, sizeof(header_data)),
             return NUX_FAILURE);
    nux_cart_header_t header;
    NU_CHECK(nux_cart_parse_header(header_data, &header), return NUX_FAILURE);

    nu_u32_t data_offset = NUX_CART_HEADER_SIZE
                           + NUX_CART_OBJECT_ENTRY_SIZE * header.entry_count;

    // Load objects
    for (nu_u32_t i = 0; i < header.entry_count; ++i)
    {
        // Seek to entry
        nux_platform_seek(
            env->inst, NUX_CART_HEADER_SIZE + NUX_CART_OBJECT_ENTRY_SIZE * i);

        // Read entry
        nux_cart_entry_t entry;
        nu_byte_t        entry_data[NUX_CART_OBJECT_ENTRY_SIZE];
        NU_CHECK(cart_read(env, entry_data, sizeof(entry_data)),
                 return NUX_FAILURE);

        if (!nux_cart_parse_entries(entry_data, 1, &entry))
        {
            return NUX_FAILURE;
        }

        // Seek to object
        nux_platform_seek(env->inst, data_offset + entry.offset);

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
