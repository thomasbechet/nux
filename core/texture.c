#include "internal.h"

nux_status_t
nux_create_texture (nux_env_t env, nux_oid_t oid, nux_u32_t size)
{
    if (size < NUX_TEXTURE_MIN_SIZE || size > NUX_TEXTURE_MAX_SIZE)
    {
        nux_set_error(env, NUX_ERROR_INVALID_TEXTURE_SIZE);
        return NUX_FAILURE;
    }
    nux_object_t *object = nux_object_set(env, oid, NUX_OBJECT_TEXTURE);
    NU_CHECK(object, return NUX_FAILURE);
    object->texture.size = size;
    NU_CHECK(nux_malloc(env, nux_texture_memsize(size), &object->texture.data),
             return NUX_FAILURE);
    return NUX_SUCCESS;
}
void
nux_update_texture (nux_env_t   env,
                    nux_oid_t   oid,
                    nux_u32_t   x,
                    nux_u32_t   y,
                    nux_u32_t   w,
                    nux_u32_t   h,
                    const void *p)
{
    nux_object_t *object = nux_validate_object(env, NUX_OBJECT_TEXTURE, oid);
    NU_CHECK(object, return);
    nu_byte_t *data = nux_instance_get_memory(env->inst, object->texture.data);
    NU_ASSERT(x + w <= object->texture.size);
    NU_ASSERT(y + h <= object->texture.size);
    for (nu_size_t i = 0; i < h; ++i)
    {
        nu_byte_t *row = data + object->texture.size * (y + i) + x;
        nu_byte_t *src = ((nu_byte_t *)p) + w * i;
        nu_memcpy(row, src, w);
    }
    ++object->mesh.update_counter;
}
