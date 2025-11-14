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

static void
oom_panic (void *userdata)
{
    NUX_ERROR("out of memory");
    nux_os_panic();
}
static void *
os_alloc (void *userdata, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    return nux_os_alloc(optr, osize, nsize);
}

nux_arena_t *
nux_arena_new (nux_arena_t *arena)
{
    nux_arena_t *a = nux_resource_new(arena, NUX_RESOURCE_ARENA);
    NUX_CHECK(a, return nullptr);
    nux_arena_init_core(a);
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
nux_arena_init_core (nux_arena_t *arena)
{
    nux_arena_init(arena, nullptr, os_alloc, oom_panic);
}
void
nux_arena_cleanup (void *data)
{
    nux_arena_t *arena = data;
    nux_arena_free(arena);
}
