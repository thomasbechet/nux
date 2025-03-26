#include "internal.h"

nux_status_t
nux_create_mesh (nux_env_t              env,
                 nux_oid_t              oid,
                 nux_u32_t              count,
                 nux_primitive_t        primitive,
                 nux_vertex_attribute_t attributes)
{
    nux_object_t *object = nux_object_set(env->inst, oid, NUX_OBJECT_MESH);
    NU_CHECK(object, return NUX_FAILURE);
    object->mesh.count      = count;
    object->mesh.primitive  = primitive;
    object->mesh.attributes = attributes;
    nu_byte_t *data         = nux_malloc(
        env->inst, nux_vertex_memsize(attributes, count), &object->mesh.data);
    NU_CHECK(data, return NUX_FAILURE);
    nu_memset(data, 0, nux_vertex_memsize(attributes, count));
    return NUX_SUCCESS;
}
void
nux_update_mesh (nux_env_t              env,
                 nux_oid_t              oid,
                 nux_vertex_attribute_t attributes,
                 nux_u32_t              first,
                 nux_u32_t              count,
                 const void            *p)
{
    nux_object_t *object
        = nux_instance_get_object(env->inst, NUX_OBJECT_MESH, oid);
    NU_CHECK(object, return);
    nu_f32_t       *ptr = nux_instance_get_memory(env->inst, object->mesh.data);
    const nu_f32_t *data = p;
    if (attributes & NUX_VERTEX_POSITION
        && object->mesh.attributes & NUX_VERTEX_POSITION)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_POSITION, count);
        nu_u32_t dst_offset = nux_vertex_offset(
            object->mesh.attributes, NUX_VERTEX_POSITION, object->mesh.count);
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
    if (attributes & NUX_VERTEX_UV && object->mesh.attributes & NUX_VERTEX_UV)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_UV, count);
        nu_u32_t dst_offset = nux_vertex_offset(
            object->mesh.attributes, NUX_VERTEX_UV, object->mesh.count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 2 + 0]
                = data[src_offset + i * 2 + 0];
            ptr[dst_offset + (first + i) * 2 + 1]
                = data[src_offset + i * 2 + 1];
        }
    }
    if (attributes & NUX_VERTEX_COLOR
        && object->mesh.attributes & NUX_VERTEX_COLOR)
    {
        nu_u32_t src_offset
            = nux_vertex_offset(attributes, NUX_VERTEX_COLOR, count);
        nu_u32_t dst_offset = nux_vertex_offset(
            object->mesh.attributes, NUX_VERTEX_COLOR, object->mesh.count);
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
}
