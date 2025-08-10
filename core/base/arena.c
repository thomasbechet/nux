#include "nux_internal.h"

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
arena_reset (nux_ctx_t *ctx, nux_arena_t *arena, nux_resource_header_t *to)
{
    nux_resource_header_t *header = arena->last_resource;
    while (header != to)
    {
        nux_resource_type_t *type = ctx->resources_types + header->type;
        if (type->cleanup)
        {
            type->cleanup(ctx, header + 1);
        }
        if (header->self)
        {
            nux_res_delete(ctx, header->self);
        }
        header = header->prev;
    }
    arena->last_resource  = NUX_NULL;
    arena->first_resource = NUX_NULL;
    arena->size           = 0;
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
            if (!p)
            {
                return NUX_NULL;
            }
            nux_memcpy(p, optr, osize);
            return p;
        }
    }
    else // malloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(!osize);
        void *p = arena_push(arena, nsize);
        if (!p)
        {
            return NUX_NULL;
        }
        nux_memset(p, 0, nsize);
        return p;
    }
}

void *
nux_arena_alloc_raw (nux_ctx_t *ctx, nux_arena_t *arena, nux_u32_t size)
{
    void *p = arena_alloc(arena, NUX_NULL, 0, size);
    NUX_ENSURE(
        p,
        return NUX_NULL,
        "out of memory for '%s' (allocate %d, remaining %d, capacity %d)",
        arena->name,
        size,
        arena->capa - arena->size,
        arena->capa);
    return p;
}
void *
nux_arena_alloc (nux_ctx_t *ctx, nux_res_t arena, nux_u32_t size)
{
    nux_arena_t *a = nux_res_check(ctx, NUX_RES_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    return nux_arena_alloc_raw(ctx, a, size);
}
void *
nux_arena_alloc_res (nux_ctx_t *ctx,
                     nux_res_t  arena,
                     nux_u32_t  type,
                     nux_u32_t  size,
                     nux_res_t *id)
{
    nux_arena_t *a = nux_res_check(ctx, NUX_RES_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    nux_resource_header_t *header
        = nux_arena_alloc(ctx, arena, sizeof(nux_resource_header_t) + size);
    NUX_CHECK(header, return NUX_NULL);
    header->type = type;
    header->prev = a->last_resource;
    header->next = NUX_NULL;
    if (!a->first_resource)
    {
        a->first_resource = header;
    }
    a->last_resource = header;
    void *data       = header + 1; // TODO: handle proper memory alignment
    if (id)
    {
        *id = nux_res_create(ctx, type, data);
        if (!(*id))
        {
            // TODO: rewind arena ?
            return NUX_NULL;
        }
        header->self = *id;
    }
    return data;
}
nux_c8_t *
nux_arena_alloc_path (nux_ctx_t *ctx, nux_res_t arena, const nux_c8_t *path)
{
    if (!path)
    {
        return NUX_NULL;
    }
    nux_u32_t len = nux_strnlen(path, NUX_PATH_MAX);
    if (!len)
    {
        return NUX_NULL;
    }
    nux_c8_t *p = nux_arena_alloc(ctx, arena, len + 1);
    NUX_CHECK(p, return NUX_NULL);
    nux_memcpy(p, path, len + 1); // include '\0'
    return p;
}
void
nux_arena_reset_raw (nux_ctx_t *ctx, nux_arena_t *arena)
{
    arena_reset(ctx, arena, NUX_NULL);
}

nux_res_t
nux_arena_new (nux_ctx_t *ctx, const nux_c8_t *name, nux_u32_t capa)
{
    NUX_CHECK(capa, return NUX_NULL);
    nux_arena_t *arena = nux_arena_pool_add(&ctx->arenas);
    NUX_CHECK(arena, return NUX_NULL);
    arena->self = nux_res_create(ctx, NUX_RES_ARENA, arena);
    NUX_CHECK(arena->self, goto cleanup0);
    arena->capa           = capa;
    arena->size           = 0;
    arena->first_resource = NUX_NULL;
    arena->last_resource  = NUX_NULL;
    arena->data           = nux_os_alloc(ctx->userdata, NUX_NULL, 0, capa);
    NUX_CHECK(arena->data, goto cleanup1);
    nux_strncpy(arena->name, name, sizeof(arena->name) - 1);
    return arena->self;

cleanup1:
    nux_res_delete(ctx, arena->self);
cleanup0:
    nux_arena_pool_remove(&ctx->arenas, arena);
    return NUX_NULL;
}
void
nux_arena_reset (nux_ctx_t *ctx, nux_res_t arena)
{
    nux_arena_t *a = nux_res_check(ctx, NUX_RES_ARENA, arena);
    NUX_CHECK(a, return);
    arena_reset(ctx, a, NUX_NULL);
}
nux_res_t
nux_arena_frame (nux_ctx_t *ctx)
{
    return ctx->frame_arena;
}
nux_res_t
nux_arena_scratch (nux_ctx_t *ctx)
{
    return nux_arena_frame(ctx);
}
