#include "internal.h"

#define MEM2BLOCK(size) ((((size) - 1) / (NUX_BLOCK_SIZE)) + 1)

static nux_status_t
init_object_table (nux_instance_t inst, nux_u32_t object_capa)
{
    NU_CHECK(object_capa, return NUX_FAILURE);
    inst->objects_count     = 0;
    inst->objects_size      = 1; // reserve index 0 for NULL
    inst->objects_capa      = object_capa;
    inst->objects_free      = NU_NULL;
    inst->objects_slot_size = 0;
    return NUX_SUCCESS;
}
static nux_id_t
add_object (nux_env_t env, nux_object_type_t type, nux_u32_t block_index)
{
    nux_u32_t index = 0;
    if (env->inst->objects_free)
    {
        index                   = env->inst->objects_free;
        env->inst->objects_free = env->inst->objects[index].next;
    }
    else
    {
        if (env->inst->objects_size >= env->inst->objects_capa)
        {
            nux_set_error(env, NUX_ERROR_OUT_OF_OBJECTS);
            return NU_NULL;
        }
        index = env->inst->objects_size++;
    }
    env->inst->objects[index].key = NUX_KEY_MAKE(block_index, 1);
    ++env->inst->objects_count;
    return NUX_ID_MAKE(index, 1);
}
static void
remove_object_unchecked (nux_env_t env, nux_id_t id)
{
    nux_u32_t index                = NUX_ID_INDEX(id);
    env->inst->objects[index].next = env->inst->objects_free;
    env->inst->objects_free        = index;
    --env->inst->objects_count;
}
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
static nux_u32_t
get_object_block_index (nux_env_t env, nux_id_t id)
{
    return NUX_KEY_BLOCK(env->inst->objects[NUX_ID_INDEX(id)].key);
}

nux_id_t
nux_stack_new (nux_env_t env, nux_id_t allocator, nux_u32_t size)
{
    nux_u32_t block_count = MEM2BLOCK(size);
    nux_id_t  id
        = allocator_add(env, allocator, NUX_OBJECT_STACK, sizeof(nux_stack_t));
    NU_CHECK(id, return NU_NULL);
    nux_stack_t *stack = nux_object_get_unchecked(env, id);
    stack->blocks_capa = block_count;
    stack->blocks_size = 0;
    stack->blocks      = allocator_add(env, allocator, NUX_OBJECT_MEMORY, size);
    NU_CHECK(stack->blocks, return NU_NULL);
    stack->blocks_first = get_object_block_index(env, stack->blocks);
    return id;
}
nux_id_t
nux_stack_push (nux_env_t         env,
                nux_id_t          stack,
                nux_object_type_t type,
                nux_u32_t         size)
{
    nux_u32_t    block_count = MEM2BLOCK(size);
    nux_stack_t *s           = nux_object_get(env, stack, NUX_OBJECT_STACK);
    NU_CHECK(s, return NU_NULL);
    if (s->blocks_size + block_count >= s->blocks_capa)
    {
        nux_set_error(env, NUX_ERROR_OUT_OF_MEMORY);
        return NU_NULL;
    }
    nux_id_t id = add_object(env, type, s->blocks_first + s->blocks_size);
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
    pool->blocks_first = get_object_block_index(env, pool->blocks);
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
    return add_object(env, type, block_index);
}
void
nux_pool_remove (nux_env_t env, nux_id_t pool, nux_id_t id)
{
    nux_pool_t *p = nux_object_get(env, pool, NUX_OBJECT_POOL);
    NU_CHECK(p, return);
    nux_u32_t *free_item = nux_object_get_unchecked(env, id);
    *free_item           = p->free_block;
    p->free_block        = get_object_block_index(env, id);
}
void *
nux_object_get (nux_env_t env, nux_id_t id, nux_object_type_t type)
{
    nux_u32_t index   = NUX_ID_INDEX(id);
    nux_u32_t version = NUX_ID_VERSION(id);
    NU_CHECK(index < env->inst->objects_size, return NU_NULL);
    nux_object_entry_t *entry = env->inst->objects + index;
    NU_CHECK(NUX_KEY_VERSION(entry->key) == version
                 && NUX_KEY_TYPE(entry->key) == type,
             return NU_NULL);
    return env->inst->memory + index * NUX_BLOCK_SIZE;
}
void *
nux_object_get_unchecked (nux_env_t env, nux_id_t id)
{
    nux_u32_t           index = NUX_ID_INDEX(id);
    nux_object_entry_t *entry = env->inst->objects + index;
    return env->inst->memory + NUX_KEY_BLOCK(entry->key) * NUX_BLOCK_SIZE;
}
nux_object_type_t
nux_object_type (nux_env_t env, nux_id_t id)
{
    nux_u32_t index = NUX_ID_INDEX(id);
    NU_CHECK(index < env->inst->objects_size, return NU_NULL);
    nux_object_entry_t *entry = env->inst->objects + index;
    return NUX_KEY_TYPE(entry->key);
}
nux_id_t
nux_object_slot (nux_env_t env, nux_u32_t slot)
{
    NU_CHECK(slot > env->inst->objects_slot_size, return NU_NULL);
    nux_u32_t                 index = env->inst->objects_capa - slot - 1;
    const nux_object_entry_t *entry = &env->inst->objects[index];
    return NUX_ID_MAKE(index, NUX_KEY_VERSION(entry->key));
}
nux_status_t
nux_object_slot_set (nux_env_t env, nux_u32_t slot, nux_id_t id)
{
    return NUX_SUCCESS;
}
