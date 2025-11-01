#include "internal.h"

static void
arena_reset (nux_arena_t *arena, nux_rid_t rid)
{
    // delete resources
    nux_rid_t resource = arena->last_resource;
    while (resource && resource != rid)
    {
        nux_rid_t todelete = resource;
        resource           = nux_resource_next_arena(resource);
        nux_resource_delete(todelete);
    }
    arena->first_resource = NUX_NULL;
    arena->last_resource  = NUX_NULL;

    // clear memory
    if (arena->block_allocator.total_alloc)
    {
        nux_c8_t alloc_buf[10];
        nux_c8_t waste_buf[10];
        NUX_DEBUG("reset arena '%s' - alloc %s - waste %s (%.02lf%)",
                  nux_resource_name(arena),
                  nux_mem_human(arena->block_allocator.total_alloc, alloc_buf),
                  nux_mem_human(arena->block_allocator.total_waste, waste_buf),
                  (nux_f32_t)arena->block_allocator.total_waste
                      / arena->block_allocator.total_alloc * 100);
    }
    nux_block_allocator_clear(&arena->block_allocator);
}

void
nux_arena_init (nux_arena_t *arena, nux_allocator_t *allocator)
{
    arena->first_resource = NUX_NULL;
    arena->last_resource  = NUX_NULL;
    nux_block_allocator_init(&arena->block_allocator, allocator);
}
nux_arena_t *
nux_arena_new (nux_arena_t *arena)
{
    nux_arena_t *a = nux_resource_new(arena, NUX_RESOURCE_ARENA);
    NUX_CHECK(a, return NUX_NULL);
    nux_arena_init(a, nux_arena_allocator(arena));
    return a;
}
void
nux_arena_reset (nux_arena_t *arena)
{
    arena_reset(arena, NUX_NULL);
}
nux_allocator_t *
nux_arena_allocator (nux_arena_t *arena)
{
    return nux_block_allocator_interface(&arena->block_allocator);
}
void
nux_arena_cleanup (void *data)
{
    nux_arena_t *arena = data;
    nux_arena_reset(arena);
    nux_block_allocator_free(&arena->block_allocator);
}
nux_u32_t
nux_arena_memory_usage (const nux_arena_t *arena)
{
    return nux_block_allocator_memory_usage(&arena->block_allocator);
}
nux_u32_t
nux_arena_memory_capacity (const nux_arena_t *arena)
{
    return nux_block_allocator_memory_capacity(&arena->block_allocator);
}
nux_u32_t
nux_arena_block_count (const nux_arena_t *arena)
{
    return nux_block_allocator_block_count(&arena->block_allocator);
}
