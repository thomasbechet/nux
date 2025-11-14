#include "test.h"

void
nux_test_vector (void)
{
    nux_vec(nux_u32_t) v;
    nux_assert(&v.vec.arena == &v.arena);
    nux_assert(&v.vec.osize == &v.osize);
    nux_assert(&v.vec.capa == &v.capa);
    nux_assert(&v.vec.size == &v.size);
    nux_assert((void *)&v.vec.data == (void *)&v.data);
    nux_vec_init(&v, nux_arena_core());
    nux_assert(v.size == 0 && v.capa == 0);
    *nux_vec_push(&v) = 1;
    *nux_vec_push(&v) = 2;
    *nux_vec_push(&v) = 3;
    nux_assert(v.data[0] == 1);
    nux_assert(v.data[1] == 2);
    nux_assert(v.data[2] == 3);

    // swap
    nux_vec_swap(&v, 0, 1);
    nux_assert(v.data[0] == 2);
    nux_assert(v.data[1] == 1);
    nux_vec_swap_pop(&v, 1);
    nux_assert(v.size == 2);
    nux_assert(v.data[1] == 3);

    // get / last
    nux_assert(*nux_vec_get(&v, 1) == 3);
    nux_assert(nux_vec_get(&v, 2) == nullptr);

    // pop
    nux_vec_pop(&v);
    nux_vec_pop(&v);
    nux_vec_pop(&v);
    nux_assert(v.size == 0);
    nux_vec_pop(&v);
    nux_assert(v.size == 0);
}
