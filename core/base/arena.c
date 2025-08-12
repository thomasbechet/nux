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
static void
arena_reset (nux_ctx_t *ctx, nux_arena_t *arena, nux_resource_finalizer_t *to)
{
    nux_resource_finalizer_t *finalizer = arena->last_finalizer;
    while (finalizer != to)
    {
        nux_resource_type_t *type = ctx->resources_types + finalizer->type;
        NUX_DEBUG("cleanup '%s' 0x%08X", type->name, finalizer->self);
        if (type->cleanup)
        {
            type->cleanup(ctx, finalizer + 1);
        }
        if (finalizer->self)
        {
            nux_res_delete(ctx, finalizer->self);
        }
        finalizer = finalizer->prev;
    }
    arena->last_finalizer  = NUX_NULL;
    arena->first_finalizer = NUX_NULL;
    arena->size            = 0;
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
    nux_resource_finalizer_t *finalizer
        = nux_arena_alloc(ctx, arena, sizeof(nux_resource_finalizer_t) + size);
    NUX_CHECK(finalizer, return NUX_NULL);
    finalizer->type = type;
    finalizer->prev = a->last_finalizer;
    finalizer->next = NUX_NULL;
    if (!a->first_finalizer)
    {
        a->first_finalizer = finalizer;
    }
    a->last_finalizer = finalizer;
    void *data        = finalizer + 1; // TODO: handle proper memory alignment
    if (id)
    {
        *id = nux_res_create(ctx, type, data);
        if (!(*id))
        {
            // TODO: rewind arena ?
            return NUX_NULL;
        }
        finalizer->self = *id;
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
void
nux_arena_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_arena_t *arena = data;
    nux_arena_reset_raw(ctx, arena);
    nux_os_alloc(ctx->userdata, arena->data, 0, 0);
}

nux_res_t
nux_arena_new (nux_ctx_t      *ctx,
               nux_res_t       arena,
               const nux_c8_t *name,
               nux_u32_t       capa)
{
    NUX_CHECK(arena && capa, return NUX_NULL);
    nux_res_t    res;
    nux_arena_t *a
        = nux_arena_alloc_res(ctx, arena, NUX_RES_ARENA, sizeof(*a), &res);
    NUX_CHECK(a, return NUX_NULL);
    a->self            = res;
    a->capa            = capa;
    a->size            = 0;
    a->first_finalizer = NUX_NULL;
    a->last_finalizer  = NUX_NULL;
    a->data            = nux_os_alloc(ctx->userdata, NUX_NULL, 0, capa);
    NUX_CHECK(a->data, goto cleanup0);
    nux_strncpy(a->name, name, sizeof(a->name) - 1);
    return a->self;

cleanup0:
    nux_res_delete(ctx, a->self);
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
nux_arena_main (nux_ctx_t *ctx)
{
    return ctx->main_arena;
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
