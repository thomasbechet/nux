#include "internal.h"

// static void
// arena_clear (nux_arena_t *arena, nux_rid_t rid)
// {
// // clear memory
// if (arena->block_allocator.total_alloc)
// {
//     nux_c8_t alloc_buf[10];
//     nux_c8_t waste_buf[10];
//     NUX_DEBUG("reset arena '%s' - alloc %s - waste %s (%.02lf%)",
//               nux_resource_name(arena),
//               nux_mem_human(arena->block_allocator.total_alloc, alloc_buf),
//               nux_mem_human(arena->block_allocator.total_waste, waste_buf),
//               (nux_f32_t)arena->block_allocator.total_waste
//                   / arena->block_allocator.total_alloc * 100);
// }
// nux_block_arena_clear(&arena->block_allocator);
// }

nux_arena_t *
nux_arena_new (nux_arena_t *arena)
{
    nux_block_arena_t *ba = nux_arena_malloc(arena, sizeof(*ba));
    NUX_CHECK(ba, return NUX_NULL);
    nux_arena_t *a = nux_resource_new(arena, NUX_RESOURCE_ARENA);
    NUX_CHECK(a, return NUX_NULL);
    nux_block_arena_init(a, ba, nux_os_allocator());
    return a;
}
nux_u32_t
nux_arena_block_count (nux_arena_t *arena)
{
    return nux_arena_info(arena).block_count;
}
nux_u32_t
nux_arena_memory_usage (nux_arena_t *arena)
{
    return nux_arena_info(arena).memory_usage;
}
void
nux_arena_cleanup (void *data)
{
    nux_arena_t *arena = data;
    nux_arena_free(arena);
}
