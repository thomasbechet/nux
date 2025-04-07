#include "internal.h"

nux_id_t
nux_texture_create (nux_env_t env, nux_id_t stack, nux_u32_t size)
{
    if (size < NUX_TEXTURE_MIN_SIZE || size > NUX_TEXTURE_MAX_SIZE)
    {
        nux_set_error(env, NUX_ERROR_INVALID_TEXTURE_SIZE);
        return NUX_FAILURE;
    }
    nux_id_t id
        = nux_stack_push(env, stack, NUX_OBJECT_TEXTURE, sizeof(nux_texture_t));
    NU_CHECK(id, return NUX_FAILURE);
    nux_texture_t *texture = nux_object_get_unchecked(env, id);
    texture->size          = size;
    texture->data          = nux_stack_push(
        env, stack, NUX_OBJECT_MEMORY, nux_texture_memsize(size));
    NU_CHECK(texture->data, return NUX_FAILURE);
    return id;
}
void
nux_texture_update (nux_env_t   env,
                    nux_id_t    id,
                    nux_u32_t   x,
                    nux_u32_t   y,
                    nux_u32_t   w,
                    nux_u32_t   h,
                    const void *p)
{
    nux_texture_t *texture = nux_object_get(env, id, NUX_OBJECT_TEXTURE);
    NU_CHECK(texture, return);
    nu_byte_t *data = nux_object_get_unchecked(env, texture->data);
    NU_ASSERT(x + w <= texture->size);
    NU_ASSERT(y + h <= texture->size);
    for (nu_size_t i = 0; i < h; ++i)
    {
        nu_byte_t *row = data + texture->size * (y + i) + x;
        nu_byte_t *src = ((nu_byte_t *)p) + w * i;
        nu_memcpy(row, src, w);
    }
    ++texture->update_counter;
}
