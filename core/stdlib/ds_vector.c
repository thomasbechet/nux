#include "stdlib.h"

static nux_vector_header_t *
vector_to_header (void *p)
{
    return (nux_vector_header_t *)p - 1;
}
static void *
header_to_vector (nux_vector_header_t *h)
{
    return h + 1;
}
static nux_vector_header_t *
vector_reserve (nux_vector_header_t *h, nux_u32_t capa)
{
    if (capa > h->capa)
    {
        h = nux_arena_realloc(h->arena,
                              h,
                              sizeof(*h) + h->capa * h->object_size,
                              sizeof(*h) + capa * h->object_size);
        NUX_CHECK(h, return NUX_NULL);
        h->capa = capa;
    }
    return h;
}
static void *
value_at (nux_vector_header_t *h, nux_u32_t v)
{
    return (nux_u8_t *)(header_to_vector(h)) + h->size * h->object_size;
}

void *
nux_vector_init_capa (nux_arena_t *arena, nux_u32_t capa, nux_u32_t object_size)
{
    nux_vector_header_t *header
        = nux_arena_malloc(arena, capa * object_size + sizeof(*header));
    NUX_CHECK(header, return NUX_NULL);
    header->arena       = arena;
    header->object_size = object_size;
    header->capa        = 0;
    header->size        = 0;
    return header + 1;
}
nux_u32_t
nux_vector_size (void *v)
{
    return vector_to_header(v)->size;
}
nux_u32_t
nux_vector_capa (void *v)
{
    return vector_to_header(v)->capa;
}
void *
nux_vector_push (void **v)
{
    nux_vector_header_t *h = vector_to_header(*v);
    if (h->size >= h->capa)
    {
        h = vector_reserve(h, h->capa ? h->capa * 2 : 1);
        NUX_CHECK(h, return NUX_NULL);
        *v = header_to_vector(h);
    }
    ++h->size;
    return value_at(h, h->size - 1);
}
void *
nux_vector_pop (void *v)
{
    nux_vector_header_t *h = vector_to_header(v);
    NUX_CHECK(h->size, return NUX_NULL);
    void *ret = value_at(h, h->size - 1);
    --h->size;
    return ret;
}
void
nux_vector_clear (void *v)
{
    nux_vector_header_t *h = vector_to_header(v);
    h->size                = 0;
}
nux_status_t
nux_vector_reserve (void **v, nux_u32_t capa)
{
    nux_vector_header_t *h = vector_to_header(*v);
    h                      = vector_reserve(h, capa);
    NUX_CHECK(h, return NUX_FAILURE);
    *v = header_to_vector(h);
    return NUX_SUCCESS;
}

#define NUX_VECTOR_INIT_CAPA(arena, v, capa) \
    nux_vector_init_capa(arena, capa, sizeof(*v))
