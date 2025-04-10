#include "internal.h"

static inline nux_u32_t
id_index (nux_env_t env, nux_id_t id)
{
    if (id < env->inst->objects_static_head) // in static table
    {
        return id;
    }
    else // in dynamic table (remove version to get index)
    {
        return NUX_ID_INDEX(id);
    }
}
static nux_id_t
object_add (nux_env_t env, nux_object_type_t type, nux_u32_t block)
{
    nux_u32_t index = 0;
    if (env->inst->objects_dynamic_free)
    {
        index                           = env->inst->objects_dynamic_free;
        env->inst->objects_dynamic_free = env->inst->objects[index].next;
    }
    else
    {
        if (env->inst->objects_dynamic_head <= env->inst->objects_static_head)
        {
            nux_set_error(env, NUX_ERROR_OUT_OF_DYNAMIC_OBJECTS);
            return NU_NULL;
        }
        index = env->inst->objects_dynamic_head;
        --env->inst->objects_dynamic_head;
    }
    env->inst->objects[index].key = NUX_KEY_MAKE(block, 1);
    return NUX_ID_MAKE(index, 1);
}
static void
object_remove (nux_env_t env, nux_id_t id)
{
    nux_u32_t index = id_index(env, id);
    if (index > env->inst->objects_dynamic_head)
    {
        env->inst->objects[index].next  = env->inst->objects_dynamic_free;
        env->inst->objects_dynamic_free = index;
    }
}

void
nux_object_init_table (nux_instance_t inst, nux_u32_t capa)
{
    inst->objects_capa         = capa;
    inst->objects_count        = 0;
    inst->objects_static_head  = 1; // reserve index 0 for NULL
    inst->objects_dynamic_free = NU_NULL;
    inst->objects_dynamic_head = capa - 1;
}
void *
nux_object_get (nux_env_t env, nux_id_t id, nux_object_type_t type)
{
    nux_u32_t index   = id_index(env, id);
    nux_u32_t version = NUX_ID_VERSION(id); // will be zero if in static table
    nux_object_entry_t *entry = env->inst->objects + index;
    NU_CHECK(NUX_KEY_VERSION(entry->key) == version
                 && NUX_KEY_TYPE(entry->key) == type,
             return NU_NULL);
    return env->inst->memory + index * NUX_BLOCK_SIZE;
}
void *
nux_object_get_unchecked (nux_env_t env, nux_id_t id)
{
    nux_u32_t           index = id_index(env, id);
    nux_object_entry_t *entry = env->inst->objects + index;
    return env->inst->memory + NUX_KEY_BLOCK(entry->key) * NUX_BLOCK_SIZE;
}
nux_u32_t
nux_object_get_block_unchecked (nux_env_t env, nux_id_t id)
{
    nux_u32_t index = id_index(env, id);
    return NUX_KEY_BLOCK(env->inst->objects[index].key);
}
nux_id_t
nux_object_add (nux_env_t env, nux_object_type_t type, nux_u32_t block)
{
    nux_id_t id = object_add(env, type, block);
    ++env->inst->objects_count;
    // TODO: constructor ?
    return id;
}
void
nux_object_remove (nux_env_t env, nux_id_t id)
{
    // TODO: destructor ?
    object_remove(env, id);
    --env->inst->objects_count;
}

nux_object_type_t
nux_object_type (nux_env_t env, nux_id_t id)
{
    nux_u32_t index = id_index(env, id);
    NU_CHECK(index < env->inst->objects_capa, return NU_NULL);
    nux_object_entry_t *entry = env->inst->objects + index;
    return NUX_KEY_TYPE(entry->key);
}
nux_status_t
nux_object_put (nux_env_t env, nux_id_t id, nux_u32_t static_index)
{
    if (static_index == 0 || static_index > env->inst->objects_static_head)
    {
        nux_set_error(env, NUX_ERROR_INVALID_OBJECT_STATIC_INDEX);
        return NUX_FAILURE;
    }
    if (NUX_KEY_TYPE(env->inst->objects[static_index].key) != NUX_OBJECT_FREE)
    {
        nux_set_error(env, NUX_ERROR_INVALID_OBJECT_STATIC_INDEX);
        return NUX_FAILURE;
    }
    nux_u32_t index                      = id_index(env, id);
    env->inst->objects[static_index].key = env->inst->objects[index].key;
    object_remove(env, id);
    return NUX_SUCCESS;
}
