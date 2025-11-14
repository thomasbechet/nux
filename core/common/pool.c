#include "common.h"

void
nux__pool_init_capa (nux_pool_t  *pool,
                     nux_arena_t *arena,
                     nux_u32_t    osize,
                     nux_u32_t    capa)
{
    pool->osize = osize;
    pool->capa  = capa;
    pool->size  = 0;
    nux_vec_init_capa(&pool->freelist, arena, capa);
    pool->data = nux_arena_malloc(arena, osize * capa);
}
void *
nux__pool_add (nux_pool_t *pool)
{
    if (pool->freelist.size)
    {
        nux_u32_t free = *nux_vec_pop(&pool->freelist);
        return pool->data + pool->osize * free;
    }
    else
    {
        if (pool->size >= pool->capa)
        {
            nux_u32_t old_capa = pool->capa;
            void     *old_data = pool->data;
            nux_u32_t new_capa = old_capa * 2;
            if (!new_capa)
            {
                new_capa = 1;
            }
            pool->capa = new_capa;
            pool->data = nux_arena_realloc(pool->freelist.arena,
                                           old_data,
                                           pool->osize * old_capa,
                                           pool->osize * new_capa);
            nux_check(pool->data, return nullptr);
        }
        void *data = pool->data + pool->size * pool->osize;
        ++pool->size;
        return data;
    }
}
void
nux__pool_remove (nux_pool_t *pool, void *i)
{
    nux_u32_t index
        = ((nux_intptr_t)i - (nux_intptr_t)pool->data) / pool->osize;
    nux_assert(index < pool->size);
    *nux_vec_push(&pool->freelist) = index;
}
