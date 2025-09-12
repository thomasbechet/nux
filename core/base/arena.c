#include "internal.h"

static void *
arena_alloc (nux_arena_t *arena, nux_u32_t size)
{
    nux_ctx_t *ctx = arena->ctx;
    NUX_ASSERT(ctx);
    if (!size)
    {
        return NUX_NULL;
    }
    arena->total_alloc += size;
    if (arena->stack)
    {
        NUX_ENSURE(
            arena->head + size > arena->end, return NUX_NULL, "out of memory");
    }
    else
    {
        // check available space in current block
        if (!arena->head || (arena->head + size) > arena->end)
        {
            // check next block
            if (arena->last_block && arena->last_block->next)
            {
                // reuse block
                nux_arena_block_t *block = arena->last_block->next;
                arena->head              = (nux_u8_t *)(block + 1);
                arena->end               = arena->head + arena->block_size;
                arena->last_block        = block;
            }
            else
            {
                // allocate new block
                NUX_ASSERT(arena->block_size);
                nux_arena_block_t *new_block = nux_os_alloc(
                    ctx->userdata, NUX_NULL, 0, arena->block_size);
                NUX_ENSURE(new_block,
                           return NUX_NULL,
                           "failed to allocate new arena block");
                if (!arena->first_block)
                {
                    arena->first_block = new_block;
                }
                if (arena->last_block)
                {
                    arena->last_block->next = new_block;
                }
                new_block->prev   = arena->last_block;
                new_block->next   = NUX_NULL;
                arena->last_block = new_block;
                arena->head       = (nux_u8_t *)(new_block + 1);
                arena->end        = arena->head + arena->block_size;
            }
            NUX_ASSERT(arena->head < arena->end);
        }
    }
    void *p = arena->head;
    arena->head += size;
    return p;
}
static void
arena_reset (nux_arena_t *arena, nux_resource_finalizer_t *to, nux_rid_t rid)
{
    if (!arena->head)
    {
        return;
    }
    nux_resource_finalizer_t *finalizer = arena->last_finalizer;
    while (finalizer != to)
    {
        nux_resource_delete(arena->ctx, finalizer->res);
        finalizer = finalizer->prev;
    }
    nux_ctx_t *ctx = arena->ctx;
    nux_c8_t   alloc_buf[10];
    nux_c8_t   waste_buf[10];
    NUX_DEBUG("reset arena '%s' - alloc %s - waste %s (%.02lf%)",
              nux_resource_get_name(ctx, rid),
              nux_mem_human(arena->total_alloc, alloc_buf),
              nux_mem_human(arena->total_waste, waste_buf),
              (nux_f32_t)arena->total_waste / arena->total_alloc * 100);
    arena->last_finalizer  = NUX_NULL;
    arena->first_finalizer = NUX_NULL;
    arena->last_block      = arena->first_block;
    arena->head            = NUX_NULL;
    arena->end             = NUX_NULL;
    arena->total_alloc     = 0;
    arena->total_waste     = 0;
}

void *
nux_arena_alloc (nux_arena_t *arena,
                 void        *optr,
                 nux_u32_t    osize,
                 nux_u32_t    nsize)
{
    void *p = NUX_NULL;
    if (osize) // realloc
    {
        if (nsize <= osize) // shrink
        {
            arena->total_waste += (osize - nsize);
            p = optr; // nothing to do
        }
        else // grow
        {
            NUX_ASSERT(nsize);
            p = arena_alloc(arena, nsize);
            NUX_CHECK(p, return NUX_NULL);
            arena->total_waste += osize;
            nux_memset(p, 0, nsize);
            if (optr) // copy previous memory
            {
                nux_memcpy(p, optr, osize);
            }
        }
    }
    else if (nsize)
    {
        p = arena_alloc(arena, nsize);
        NUX_CHECK(p, return NUX_NULL);
        nux_memset(p, 0, nsize);
    }
    return p;
}
nux_c8_t *
nux_arena_alloc_string (nux_arena_t *arena, const nux_c8_t *s)
{
    if (!s)
    {
        return NUX_NULL;
    }
    nux_u32_t len = nux_strnlen(s, NUX_PATH_MAX);
    if (!len)
    {
        return NUX_NULL;
    }
    nux_c8_t *p = nux_arena_alloc(arena, 0, NUX_NULL, len + 1);
    NUX_CHECK(p, return NUX_NULL);
    nux_memcpy(p, s, len + 1); // include '\0'
    return p;
}
void *
nux_arena_malloc (nux_arena_t *arena, nux_u32_t size)
{
    return nux_arena_alloc(arena, NUX_NULL, 0, size);
}
void
nux_arena_cleanup (nux_ctx_t *ctx, nux_rid_t res)
{
    if (res != ctx->core_arena_rid)
    {
        nux_arena_t *arena = nux_resource_check(ctx, NUX_RESOURCE_ARENA, res);
        nux_arena_reset(ctx, res);
        nux_arena_free(arena);
    }
}

void
nux_arena_init (nux_ctx_t *ctx, nux_arena_t *arena)
{
    NUX_ASSERT(ctx);
    arena->ctx             = ctx;
    arena->first_finalizer = NUX_NULL;
    arena->last_finalizer  = NUX_NULL;
    arena->first_block     = NUX_NULL;
    arena->last_block      = NUX_NULL;
    arena->block_size      = NUX_MEM_4M;
    arena->head            = NUX_NULL;
    arena->end             = NUX_NULL;
    arena->stack           = NUX_NULL;
    arena->total_alloc     = 0;
    arena->total_waste     = 0;
}
void
nux_arena_init_stack (nux_ctx_t   *ctx,
                      nux_arena_t *arena,
                      void        *data,
                      nux_u32_t    capa)
{
    nux_arena_init(ctx, arena);
    arena->stack = data;
    arena->head  = data;
    arena->end   = ((nux_u8_t *)data) + capa;
}
void
nux_arena_free (nux_arena_t *arena)
{
    if (!arena->stack)
    {
        // free memory blocks
        nux_arena_block_t *block = arena->first_block;
        while (block)
        {
            void *p = block;
            block   = block->next;
            nux_os_alloc(arena->ctx->userdata, p, 0, 0);
        }
    }
}
nux_rid_t
nux_arena_new (nux_ctx_t *ctx, nux_rid_t arena)
{
    nux_rid_t    res;
    nux_arena_t *a = nux_resource_new(ctx, arena, NUX_RESOURCE_ARENA, &res);
    NUX_CHECK(a, return NUX_NULL);
    nux_arena_init(ctx, a);
    return res;
}
void
nux_arena_reset (nux_ctx_t *ctx, nux_rid_t arena)
{
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return);
    arena_reset(a, NUX_NULL, arena);
}
nux_u32_t
nux_arena_memory_usage (nux_ctx_t *ctx, nux_rid_t arena)
{
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return 0);
    return a->total_alloc;
}
nux_u32_t
nux_arena_memory_capacity (nux_ctx_t *ctx, nux_rid_t arena)
{
    return 0;
}
nux_u32_t
nux_arena_block_count (nux_ctx_t *ctx, nux_rid_t arena)
{
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return 0);
    nux_arena_block_t *b = a->first_block;
    nux_u32_t          i = 0;
    while (a)
    {
        b = b->next;
        ++i;
    }
    return i;
}
