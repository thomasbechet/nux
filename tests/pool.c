#include "test.h"

void
nux_test_pool (void)
{
    nux_pool(nux_u32_t) p;
    nux_assert((void *)&p.data == (void *)&p.pool.data);
    nux_pool_init(&p, nux_arena_core());
    nux_u32_t *v = nux_pool_add(&p);
    nux_assert(p.size == 1);
    nux_pool_remove(&p, v);
    nux_assert(p.size == 0);
}
