#include "common.h"

void
nux__vec_init_capa (nux_vec_t   *vec,
                    nux_arena_t *a,
                    nux_u32_t    osize,
                    nux_u32_t    capa)
{
    vec->data  = nux_arena_malloc(a, osize * capa);
    vec->arena = a;
    vec->osize = osize;
    vec->size  = 0;
    vec->capa  = capa;
}
void
nux__vec_reserve (nux_vec_t *vec, nux_u32_t capa)
{
    if (capa > vec->capa)
    {
        nux_u32_t old_capa = vec->capa;
        vec->capa          = capa;
        vec->data          = nux_arena_realloc(
            vec->arena, vec->data, vec->osize * old_capa, vec->osize * capa);
    }
}
void
nux__vec_resize (nux_vec_t *vec, nux_u32_t size)
{
    nux__vec_reserve(vec, size);
    vec->size = size;
}
void *
nux__vec_get (nux_vec_t *vec, nux_u32_t i)
{
    NUX_CHECK(i < vec->size, return nullptr);
    return (nux_u8_t *)vec->data + vec->osize * i;
}
void *
nux__vec_last (nux_vec_t *vec)
{
    if (vec->size)
    {
        return nux__vec_get(vec, vec->size - 1);
    }
    return nullptr;
}
void *
nux__vec_push (nux_vec_t *vec)
{
    if (vec->size >= vec->capa)
    {
        nux__vec_reserve(vec, vec->capa ? vec->capa * 2 : 1);
    }
    ++vec->size;
    return nux__vec_last(vec);
}
void *
nux__vec_pop (nux_vec_t *vec)
{
    NUX_CHECK(vec->size, return nullptr);
    void *ret = nux__vec_last(vec);
    --vec->size;
    return ret;
}
void
nux__vec_swap (nux_vec_t *vec, nux_u32_t a, nux_u32_t b)
{
    NUX_CHECK(a < vec->size && b < vec->size && a != b, return);
    nux_memswp(
        vec->data + vec->osize * a, vec->data + vec->osize * b, vec->osize);
}
void *
nux__vec_swap_pop (nux_vec_t *vec, nux_u32_t i)
{
    NUX_CHECK(i < vec->size, return NUX_NULL);
    nux__vec_swap(vec, i, vec->size - 1);
    return nux__vec_pop(vec);
}
