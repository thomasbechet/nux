#include "internal.h"

nux_id_t
nux_create_mesh (nux_env_t              env,
                 nux_id_t               stack,
                 nux_u32_t              count,
                 nux_primitive_t        primitive,
                 nux_vertex_attribute_t attributes)
{
    nux_id_t id
        = nux_stack_push(env, stack, NUX_OBJECT_MESH, sizeof(nux_mesh_t));
    NU_CHECK(id, return NU_NULL);
    nux_mesh_t *mesh     = nux_object_get_unchecked(env, id);
    mesh->count          = count;
    mesh->primitive      = primitive;
    mesh->attributes     = attributes;
    mesh->update_counter = 0;
    mesh->data           = nux_stack_push(
        env, stack, NUX_OBJECT_MEMORY, nux_vertex_memsize(attributes, count));
    NU_CHECK(mesh->data, return NUX_FAILURE);
    nux_u8_t *data = nux_object_get_unchecked(env, mesh->data);
    nu_memset(data, 0, nux_vertex_memsize(attributes, count));
    return NUX_SUCCESS;
}
void
nux_update_mesh (nux_env_t              env,
                 nux_id_t               id,
                 nux_vertex_attribute_t attributes,
                 nux_u32_t              first,
                 nux_u32_t              count,
                 const void            *p)
{
    nux_mesh_t *mesh = nux_object_get(env, id, NUX_OBJECT_MESH);
    NU_CHECK(mesh, return);
    nu_f32_t       *ptr  = nux_object_get_unchecked(env, mesh->data);
    const nu_f32_t *data = p;
    if (attributes & NUX_VERTEX_POSITION
        && mesh->attributes & NUX_VERTEX_POSITION)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_POSITION, count);
        nu_u32_t dst_offset = nux_vertex_offset(
            mesh->attributes, NUX_VERTEX_POSITION, mesh->count);
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
    if (attributes & NUX_VERTEX_UV && mesh->attributes & NUX_VERTEX_UV)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_UV, count);
        nu_u32_t dst_offset
            = nux_vertex_offset(mesh->attributes, NUX_VERTEX_UV, mesh->count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 2 + 0]
                = data[src_offset + i * 2 + 0];
            ptr[dst_offset + (first + i) * 2 + 1]
                = data[src_offset + i * 2 + 1];
        }
    }
    if (attributes & NUX_VERTEX_COLOR && mesh->attributes & NUX_VERTEX_COLOR)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_COLOR, count);
        nu_u32_t dst_offset = nux_vertex_offset(
            mesh->attributes, NUX_VERTEX_COLOR, mesh->count);
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
    ++mesh->update_counter;
}
