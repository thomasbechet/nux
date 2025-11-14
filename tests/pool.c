#include "test.h"

void
nux_test_pool (void)
{
    nux_pool(nux_u32_t) p;
    NUX_ASSERT((void *)&p.data == (void *)&p.pool.data);
    NUX_ASSERT(nux_pool_init(&p, nux_arena_core()));
    nux_u32_t *v = nux_pool_add(&p);
    NUX_ASSERT(p.size == 1);
    nux_pool_remove(&p, v);
    NUX_ASSERT(p.size == 0);
}
