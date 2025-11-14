#include "test.h"

void
nux_test_vector (void)
{
    nux_vec(nux_u32_t) v;
    NUX_ASSERT(&v.vec.arena == &v.arena);
    NUX_ASSERT(&v.vec.osize == &v.osize);
    NUX_ASSERT(&v.vec.capa == &v.capa);
    NUX_ASSERT(&v.vec.size == &v.size);
    NUX_ASSERT((void *)&v.vec.data == (void *)&v.data);
    NUX_ASSERT(nux_vec_init(&v, nux_arena_core()));
    NUX_ASSERT(v.size == 0 && v.capa == 0);
    *nux_vec_push(&v) = 1;
    *nux_vec_push(&v) = 2;
    *nux_vec_push(&v) = 3;
    NUX_ASSERT(v.data[0] == 1);
    NUX_ASSERT(v.data[1] == 2);
    NUX_ASSERT(v.data[2] == 3);

    // swap
    nux_vec_swap(&v, 0, 1);
    NUX_ASSERT(v.data[0] == 2);
    NUX_ASSERT(v.data[1] == 1);
    nux_vec_swap_pop(&v, 1);
    NUX_ASSERT(v.size == 2);
    NUX_ASSERT(v.data[1] == 3);

    // get / last
    NUX_ASSERT(*nux_vec_get(&v, 1) == 3);
    NUX_ASSERT(nux_vec_get(&v, 2) == nullptr);

    // pop
    nux_vec_pop(&v);
    nux_vec_pop(&v);
    nux_vec_pop(&v);
    NUX_ASSERT(v.size == 0);
    nux_vec_pop(&v);
    NUX_ASSERT(v.size == 0);
}
