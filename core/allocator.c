#include "internal.h"

#define MEM2BLOCK(size) ((((size) - 1) / (NUX_BLOCK_SIZE)) + 1)

static nux_id_t
allocator_add (nux_env_t         env,
               nux_id_t          allocator,
               nux_object_type_t type,
               nux_u32_t         size)
{
    nux_id_t id = NU_NULL;
    if (allocator)
    {
        switch (nux_object_type(env, allocator))
        {
            case NUX_OBJECT_STACK:
                id = nux_stack_push(env, allocator, type, size);
                break;
            case NUX_OBJECT_POOL:
                id = nux_pool_add(env, allocator, type);
                break;
            default: {
                nux_set_error(env, NUX_ERROR_INVALID_OBJECT_TYPE);
                return NU_NULL;
            }
            break;
        }
    }
    else
    {
    }
    return id;
}

nux_id_t
nux_stack_new (nux_env_t env, nux_id_t stack, nux_u32_t size)
{
    nux_u32_t block_count = MEM2BLOCK(size);
    nux_id_t  id
        = allocator_add(env, stack, NUX_OBJECT_STACK, sizeof(nux_stack_t));
    NU_CHECK(id, return NU_NULL);
    nux_stack_t *s = nux_object_get_unchecked(env, id);
    s->blocks_capa = block_count;
    s->blocks_size = 0;
    s->blocks      = allocator_add(env, stack, NUX_OBJECT_MEMORY, size);
    NU_CHECK(s->blocks, return NU_NULL);
    s->blocks_first = nux_object_get_block_unchecked(env, s->blocks);
    return id;
}
nux_id_t
nux_stack_push (nux_env_t         env,
                nux_id_t          stack,
                nux_object_type_t type,
                nux_u32_t         size)
{
    nux_stack_t *s = nux_object_get(env, stack, NUX_OBJECT_STACK);
    NU_CHECK(s, return NU_NULL);
    nux_u32_t block_count = MEM2BLOCK(size);
    if (s->blocks_size + block_count >= s->blocks_capa)
    {
        nux_set_error(env, NUX_ERROR_OUT_OF_MEMORY);
        return NU_NULL;
    }
    nux_id_t id = nux_object_add(env, type, s->blocks_first + s->blocks_size);
    NU_CHECK(id, return NU_NULL);
    s->blocks_size += block_count;
    return id;
}

nux_id_t
nux_pool_new (nux_env_t env,
              nux_id_t  allocator,
              nux_u32_t item_size,
              nux_u32_t item_capa)
{
    nux_id_t id
        = allocator_add(env, allocator, NUX_OBJECT_POOL, sizeof(nux_pool_t));
    NU_CHECK(id, return NU_NULL);
    nux_pool_t *pool = nux_object_get_unchecked(env, id);
    pool->items_capa = item_capa;
    pool->blocks     = allocator_add(
        env, allocator, NUX_OBJECT_MEMORY, MEM2BLOCK(item_size) * item_capa);
    NU_CHECK(pool->blocks, return NU_NULL);
    pool->blocks_first = nux_object_get_block_unchecked(env, pool->blocks);
    pool->free_block   = 0;
    return id;
}
nux_id_t
nux_pool_add (nux_env_t env, nux_id_t pool, nux_object_type_t type)
{
    nux_pool_t *p = nux_object_get(env, pool, NUX_OBJECT_POOL);
    NU_CHECK(p, return NU_NULL);
    nux_u32_t block_index = NU_NULL;
    if (p->free_block)
    {
        block_index = p->free_block;
        p->free_block
            = *((nux_u32_t *)nux_object_get_unchecked(env, p->free_block));
    }
    if (!block_index)
    {
        if (p->items_size >= p->items_capa)
        {
            nux_set_error(env, NUX_ERROR_OUT_OF_POOL_ITEM);
            return NU_NULL;
        }
        block_index = p->blocks_first + p->items_size++;
    }
    return nux_object_add(env, type, block_index);
}
void
nux_pool_remove (nux_env_t env, nux_id_t pool, nux_id_t id)
{
    nux_pool_t *p = nux_object_get(env, pool, NUX_OBJECT_POOL);
    NU_CHECK(p, return);
    nux_u32_t *free_item = nux_object_get_unchecked(env, id);
    *free_item           = p->free_block;
    p->free_block        = nux_object_get_block_unchecked(env, id);
}
