#include "internal.h"

#define hindex(h, i, osize)                    \
    ((nux_object_header_t *)((nux_intptr_t)(h) \
                             + ((osize) + sizeof(nux_object_header_t)) * (i)))
#define h2d(h) ((h) + 1)
#define d2h(d) ((nux_object_header_t *)d - 1)

typedef struct
{
    nux_u32_t version : 8;
    nux_u32_t index : 24;
} nux_id_t;

static nux_id_t
build_id (nux_id_t old, nux_u32_t index)
{
    return (nux_id_t) { .version = old.version, .index = index };
}

typedef enum
{
    NUX_OBJECT_NULL     = 0,
    NUX_OBJECT_ARENA    = 1,
    NUX_OBJECT_TEXTURE  = 2,
    NUX_OBJECT_TYPE_MAX = 32
} nux_object_types_t;

typedef struct nux_object_header_t
{
    nux_u32_t type; // NUX_OBJECT_NULL if unused
    nux_id_t  id;
    union
    {
        nux_arena_t                *arena;
        struct nux_object_header_t *free;
    };
} nux_object_header_t;

typedef struct
{
    nux_pool(nux_object_header_t *) objects;
    nux_object_header_t *free_object;
    nux_u32_t            osize;
    nux_u32_t            chunk_size;
} nux_object_type_t;

static struct
{
    nux_object_type_t types[32];
} _module;

static void
object_finalize (void *data)
{
}

void *
nux_object_new (nux_arena_t *a, nux_u32_t type)
{
    nux_assert(type < nux_array_size(_module.types));
    nux_object_type_t *t = _module.types + type;

    // generate new chunk (unlikely)
    if (!t->free_object)
    {
        // allocate chunk
        nux_object_header_t *chunk = nux_malloc(
            nux_arena_core(),
            (t->osize + sizeof(nux_object_header_t)) * t->chunk_size);

        // initialize objects
        for (nux_u32_t i = 0; i < t->chunk_size - 1; ++i)
        {
            auto *h = hindex(chunk, i, t->osize);
            h->type = NUX_OBJECT_NULL;
            h->free = hindex(chunk, i + 1, t->osize);
        }
        hindex(chunk, t->osize, t->chunk_size - 1)->free = nullptr;

        // set new free object list
        t->free_object = chunk;
    }

    // setup finalizer
    nux_object_header_t *h
        = nux_mallocf(a, sizeof(*h) + t->osize, object_finalize);

    // find new object slot
    nux_object_header_t *h = t->free_object;
    nux_assert(h->type == NUX_OBJECT_NULL);
    t->free_object = h->free;

    // generate id
    nux_u32_t index;
    *nux_pool_add(&t->objects, &index) = h;
    nux_id_t id                        = build_id(h->id, index);

    // initialize object
    h->type  = type;
    h->id    = id;
    h->arena = a;

    return h2d(h);
}
nux_id_t
nux_object_id (void *data)
{
    return d2h(data)->id;
}
nux_arena_t *
nux_object_arena (void *data)
{
    return d2h(data)->arena;
}
void
nux_object_recycle (void *data)
{
}
void *
nux_object_get (nux_u32_t type, nux_id_t id)
{
    nux_assert(type < nux_array_size(_module.types));
    nux_check(id.index < _module.types[type].objects.size, return nullptr);
    nux_object_header_t *h = _module.types[type].objects.data[id.index];
    nux_check(h->id.version == id.version, return nullptr);
    return h2d(h);
}
void *
nux_object_check (nux_u32_t type, nux_id_t id)
{
    void *data = nux_object_get(type, id);
    nux_ensure(data, return nullptr, "invalid id");
    return data;
}
