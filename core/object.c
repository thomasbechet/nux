#include "internal.h"

#define BUILD_ID(index, version) ((nux_u32_t)((version)) << 24 | (index))
#define ID_VERSION(id)           ((id) >> 24)
#define ID_INDEX(id)             ((id) & 0xFFFFFF)

static void *
arena_push (nux_arena_t *arena, nux_u32_t size)
{
    if (arena->size + size > arena->capa)
    {
        return NUX_NULL; // Out of memory
    }
    void *p = (nux_u8_t *)arena->data + arena->size;
    arena->size += size;
    return p;
}
static void
delete_objects (nux_env_t *env, nux_arena_t *arena, nux_u32_t object)
{
    nux_u32_t next = arena->last_object;
    while (next != object)
    {
        nux_object_delete(env, next);
        next = env->inst->objects.data[ID_INDEX(next)].prev;
    }
}
static void *
arena_alloc (nux_arena_t *arena, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    if (optr) // realloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(osize);
        if (nsize <= osize) // shrink
        {
            return optr; // nothing to do
        }
        else // grow
        {
            void *p = arena_push(arena, nsize);
            NUX_CHECK(p, return NUX_NULL);
            nux_memcpy(p, optr, osize);
            return p;
        }
    }
    else // malloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(!osize);
        void *p = arena_push(arena, nsize);
        NUX_CHECK(p, return NUX_NULL);
        nux_memset(p, 0, nsize);
        return p;
    }
}

void
nux_object_register (nux_instance_t      *inst,
                     const nux_c8_t      *name,
                     nux_object_cleanup_t cleanup)
{
    nux_object_type_t *type = inst->object_types + inst->object_types_count;
    nux_strncpy(type->name, name, NUX_ARRAY_SIZE(type->name));
    type->cleanup = cleanup;
    ++inst->object_types_count;
}
nux_u32_t
nux_object_create (nux_env_t   *env,
                   nux_arena_t *arena,
                   nux_u32_t    type_index,
                   void        *data)
{
    nux_object_t *obj = nux_object_pool_add(&env->inst->objects);
    NUX_CHECKM(obj, "Out of objects", return NUX_NULL);
    obj->type_index = type_index;
    obj->data       = data;
    obj->version += 1;
    nux_u32_t index = obj - env->inst->objects.data;
    nux_u32_t id    = BUILD_ID(index, obj->version);
    obj->prev       = arena->last_object;
    if (arena->last_object)
    {
        nux_object_t *prev_obj = &env->inst->objects.data[ID_INDEX(obj->prev)];
        prev_obj->next         = id;
    }
    obj->next = NUX_NULL;
    if (arena->first_object == NUX_NULL)
    {
        arena->first_object = id;
    }
    arena->last_object = id;
    NUX_ASSERT(obj->next != id);
    NUX_ASSERT(obj->prev != id);
    return id;
}
void
nux_object_delete (nux_env_t *env, nux_u32_t id)
{
    nux_object_t      *obj  = &env->inst->objects.data[ID_INDEX(id)];
    nux_object_type_t *type = env->inst->object_types + obj->type_index;
    if (type->cleanup)
    {
        type->cleanup(env, obj->data);
    }
    if (obj->prev)
    {
        nux_object_t *prev_obj = &env->inst->objects.data[ID_INDEX(obj->prev)];
        prev_obj->next         = obj->next;
    }
    if (obj->next)
    {
        nux_object_t *next_obj = &env->inst->objects.data[ID_INDEX(obj->next)];
        next_obj->prev         = obj->prev;
    }
    nux_object_pool_remove(&env->inst->objects, obj);
}
void
nux_object_update (nux_env_t *env, nux_u32_t id, void *data)
{
    nux_object_t *obj = env->inst->objects.data + ID_INDEX(id);
    obj->data         = data;
}
void *
nux_object_get (nux_env_t *env, nux_u32_t type_index, nux_u32_t id)
{
    nux_u32_t index   = ID_INDEX(id);
    nux_u8_t  version = ID_VERSION(id);
    if (index >= env->inst->objects.size
        || env->inst->objects.data[index].type_index != type_index
        || env->inst->objects.data[index].version != version)
    {
        nux_object_type_t *got
            = &env->inst
                   ->object_types[env->inst->objects.data[index].type_index];
        nux_object_type_t *expect = &env->inst->object_types[type_index];
        NUX_ERROR("Invalid object type (expect \"%s\", got \"%s\")",
                  expect->name,
                  got->name);
        return NUX_NULL;
    }
    return env->inst->objects.data[index].data;
}

void *
nux_arena_alloc (nux_arena_t *arena, nux_u32_t size)
{
    return arena_alloc(arena, NUX_NULL, 0, size);
}
void
nux_arena_reset_to (nux_env_t *env, nux_arena_t *arena, nux_u32_t object)
{
    delete_objects(env, arena, object);
}

nux_u32_t
nux_arena_new (nux_env_t *env)
{
    nux_arena_t *arena = nux_arena_pool_add(&env->inst->arenas);
    NUX_CHECKM(arena, "Failed to allocate arena", return NUX_NULL);
    arena->id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_ARENA, arena);
    NUX_CHECK(arena->id, goto cleanup0);
    const nux_u32_t default_capa = 1 << 20;
    arena->capa                  = default_capa;
    arena->size                  = 0;
    arena->first_object          = NUX_NULL;
    arena->last_object           = NUX_NULL;
    arena->data = nux_arena_alloc(env->active_arena, arena->capa);
    NUX_CHECK(arena->data, goto cleanup1);
    return arena->id;

cleanup1:
    nux_object_delete(env, arena->id);
cleanup0:
    nux_arena_pool_remove(&env->inst->arenas, arena);
    return NUX_NULL;
}
void
nux_arena_reset (nux_env_t *env, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(env, NUX_OBJECT_ARENA, id);
    NUX_CHECKM(arena, "Invalid arena id", return);
    // Delete all objects
    delete_objects(env, arena, NUX_NULL);
    // Reset memory
    arena->last_object = NUX_NULL;
    arena->size        = 0;
}
nux_status_t
nux_arena_use (nux_env_t *env, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(env, NUX_OBJECT_ARENA, id);
    NUX_CHECKM(arena, "Invalid arena id", return NUX_FAILURE);
    env->active_arena = arena;
    return NUX_SUCCESS;
}
nux_u32_t
nux_arena_active (nux_env_t *env)
{
    return env->active_arena->id;
}
void
nux_arena_dump (nux_env_t *env, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(env, NUX_OBJECT_ARENA, id);
    NUX_CHECK(arena, return);
    NUX_INFO("ARENA : %d", id);
    nux_u32_t next = arena->first_object;
    while (next)
    {
        nux_object_t      *obj  = &env->inst->objects.data[ID_INDEX(next)];
        nux_object_type_t *type = &env->inst->object_types[obj->type_index];
        NUX_INFO("- %s : %d", type->name, next);
        next = obj->next;
    }
}
