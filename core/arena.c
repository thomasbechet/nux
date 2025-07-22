#include "internal.h"

static void *
arena_push (nux_arena_t *arena, nux_u32_t size)
{
    if (arena->size + size > arena->capa)
    {
        return NUX_NULL; // Out of memory
    }
    void *p = (nux_u8_t *)arena->data + arena->size;
    arena->size += size;
    return p;
}
static void
arena_rewind (nux_ctx_t *ctx, nux_arena_t *arena, nux_type_header_t *to)
{
    nux_type_header_t *header = arena->last_type;
    while (header != to)
    {
        nux_type_t *type = ctx->types + header->type;
        if (type->cleanup)
        {
            type->cleanup(ctx, header + 1);
        }
        if (header->id)
        {
            nux_id_delete(ctx, header->id);
        }
        header = header->prev;
    }
}
static void *
arena_alloc (nux_arena_t *arena, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    if (optr) // realloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(osize);
        if (nsize <= osize) // shrink
        {
            return optr; // nothing to do
        }
        else // grow
        {
            void *p = arena_push(arena, nsize);
            NUX_CHECK(p, return NUX_NULL);
            nux_memcpy(p, optr, osize);
            return p;
        }
    }
    else // malloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(!osize);
        void *p = arena_push(arena, nsize);
        NUX_CHECK(p, return NUX_NULL);
        nux_memset(p, 0, nsize);
        return p;
    }
}

void *
nux_arena_alloc_raw (nux_arena_t *arena, nux_u32_t size)
{
    return arena_alloc(arena, NUX_NULL, 0, size);
}
void *
nux_arena_alloc (nux_ctx_t *ctx, nux_u32_t size)
{
    void *p = nux_arena_alloc_raw(ctx->active_arena, size);
    NUX_CHECKM(p,
               return NUX_NULL,
               "out of memory (allocate %d, remaining %d)",
               size,
               ctx->active_arena->capa - ctx->active_arena->size);
    return p;
}
void *
nux_arena_alloc_type (nux_ctx_t *ctx,
                      nux_u32_t  type,
                      nux_u32_t  size,
                      nux_u32_t *id)
{
    nux_type_header_t *header
        = nux_arena_alloc(ctx, sizeof(nux_type_header_t) + size);
    NUX_CHECK(header, return NUX_NULL);
    nux_arena_t *arena = ctx->active_arena;
    header->type       = type;
    header->prev       = arena->last_type;
    header->next       = NUX_NULL;
    if (!arena->first_type)
    {
        arena->first_type = header;
    }
    arena->last_type = header;
    void *data       = header + 1; // TODO: handle proper memory alignment
    if (id)
    {
        *id = nux_id_create(ctx, type, data);
        if (!(*id))
        {
            // TODO: rewind arena
            return NUX_NULL;
        }
        header->id = *id;
    }
    return data;
}
nux_u32_t
nux_arena_get_active (nux_ctx_t *ctx)
{
    return ctx->active_arena_id;
}
void
nux_arena_set_active (nux_ctx_t *ctx, nux_u32_t id)
{
    if (id)
    {
        nux_arena_t *arena = nux_id_get(ctx, NUX_TYPE_ARENA, id);
        NUX_CHECK(arena, return);
        ctx->active_arena = arena;
    }
    else
    {
        ctx->active_arena = ctx->core_arena;
    }
    ctx->active_arena_id = id;
}

nux_u32_t
nux_arena_new (nux_ctx_t *ctx, nux_u32_t capa)
{
    NUX_CHECK(capa, return NUX_NULL);
    nux_arena_t *arena = nux_arena_pool_add(&ctx->arenas);
    NUX_CHECK(arena, return NUX_NULL);
    arena->id = nux_id_create(ctx, NUX_TYPE_ARENA, arena);
    NUX_CHECK(arena->id, goto cleanup0);
    arena->capa       = capa;
    arena->size       = 0;
    arena->first_type = NUX_NULL;
    arena->last_type  = NUX_NULL;
    arena->data       = nux_arena_alloc(ctx, arena->capa);
    NUX_CHECK(arena->data, goto cleanup1);
    return arena->id;

cleanup1:
    nux_id_delete(ctx, arena->id);
cleanup0:
    nux_arena_pool_remove(&ctx->arenas, arena);
    return NUX_NULL;
}
void
nux_arena_rewind (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_arena_t *arena = nux_id_get(ctx, NUX_TYPE_ARENA, id);
    NUX_CHECK(arena, return);
    arena_rewind(ctx, arena, NUX_NULL);
}
nux_u32_t
nux_arena_frame (nux_ctx_t *ctx)
{
    return ctx->frame_arena;
}
// void
// nux_arena_dump (nux_ctx_t *ctx, nux_u32_t id)
// {
//     nux_arena_t *arena = nux_object_get(ctx, NUX_TYPE_ARENA, id);
//     NUX_CHECK(arena, return);
//     NUX_INFO("ARENA : %d", id);
//     nux_u32_t next = arena->first_object;
//     while (next)
//     {
//         nux_object_t *obj  = &ctx->objects.data[ID_INDEX(next)];
//         nux_type_t   *type = &ctx->types[obj->type];
//         NUX_INFO("- %s : %d", type->name, next);
//         next = obj->next;
//     }
// }
