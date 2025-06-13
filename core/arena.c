#include "internal.h"

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
    // Cleanup all objects
    nux_u32_t next = arena->last_object;
    while (next != object)
    {
        nux_u32_t     cleanup = next;
        nux_object_t *obj     = &env->inst->objects.data[cleanup];
        switch (obj->type)
        {
            case NUX_OBJECT_NULL:
                break;
            case NUX_OBJECT_ARENA:
                nux_arena_cleanup(env, obj->data);
                break;
            case NUX_OBJECT_LUA:
                break;
            case NUX_OBJECT_TEXTURE: {
                nux_texture_t *tex = obj->data;
                if (tex->slot)
                {
                    *nux_u32_vec_push(&env->inst->free_texture_slots)
                        = tex->slot;
                }
            }
            break;
            case NUX_OBJECT_RENDER_TARGET: {
                nux_render_target_t *rt = obj->data;
                if (rt->slot)
                {
                    *nux_u32_vec_push(&env->inst->free_framebuffer_slots)
                        = rt->slot;
                }
                if (rt->texture)
                {
                    *nux_u32_vec_push(&env->inst->free_framebuffer_slots)
                        = rt->slot;
                }
            }
            break;
            case NUX_OBJECT_MESH:
                break;
        }
        next                                            = obj->prev;
        *nux_u32_vec_push(&env->inst->objects_freelist) = cleanup;
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
            NUX_CHECKM(p, "Out of memory", return NUX_NULL);
            nux_memcpy(p, optr, osize);
            return p;
        }
    }
    else // malloc
    {
        NUX_ASSERT(nsize);
        NUX_ASSERT(!osize);
        void *p = arena_push(arena, nsize);
        NUX_CHECKM(p, "Out of memory", return NUX_NULL);
        nux_memset(p, 0, nsize);
        return p;
    }
}

void *
nux_arena_alloc (nux_arena_t *arena, nux_u32_t size)
{
    return arena_alloc(arena, NUX_NULL, 0, size);
}
void *
nux_alloc (nux_env_t *env, nux_u32_t size)
{
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, env->arena);
    return arena_alloc(arena, NUX_NULL, 0, size);
}
void *
nux_realloc (nux_env_t *env, void *p, nux_u32_t osize, nux_u32_t nsize)
{
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, env->arena);
    return arena_alloc(arena, p, osize, nsize);
}

nux_u32_t
nux_add_object (nux_env_t *env, nux_object_type_t type, void *data)
{
    nux_object_t *obj;
    nux_u32_t    *free_id = nux_u32_vec_pop(&env->inst->objects_freelist);
    nux_u32_t     id;
    if (free_id)
    {
        id  = *free_id;
        obj = &env->inst->objects.data[*free_id];
    }
    else
    {
        id  = env->inst->objects.size;
        obj = nux_object_vec_push(&env->inst->objects);
    }
    NUX_CHECKM(obj, "Out of objects", return NUX_NULL);
    obj->type          = type;
    obj->arena         = env->arena;
    obj->data          = data;
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, env->arena);
    obj->prev          = arena->last_object;
    arena->last_object = id;
    return id;
}
void *
nux_add_object_struct (nux_env_t        *env,
                       nux_object_type_t type,
                       nux_u32_t         ssize,
                       nux_u32_t        *id)
{
    void *data = nux_alloc(env, ssize);
    NUX_CHECK(data, return NUX_NULL);
    *id = nux_add_object(env, type, data);
    NUX_CHECK(*id, return NUX_NULL);
    return data;
}
nux_status_t
nux_delete_object (nux_env_t *env, nux_u32_t id)
{
    // Ensure the object is not the core arena or null object
    if (id < 2 || id <= env->inst->objects.size)
    {
        return NUX_FAILURE;
    }
    nux_object_t *obj = &env->inst->objects.data[id];
    if (obj->type == NUX_OBJECT_NULL)
    {
        return NUX_FAILURE;
    }
    switch (obj->type)
    {
        case NUX_OBJECT_NULL:
        case NUX_OBJECT_ARENA:
            nux_arena_cleanup(env, obj->data);
            break;
        case NUX_OBJECT_LUA:
        case NUX_OBJECT_TEXTURE:
        case NUX_OBJECT_RENDER_TARGET:
        case NUX_OBJECT_MESH:
            break;
    }
    obj->type = NUX_OBJECT_NULL;
    nux_u32_vec_pushv(&env->inst->objects_freelist, id);
    return NUX_SUCCESS;
}
void *
nux_get_object (nux_env_t *env, nux_object_type_t type, nux_u32_t id)
{
    if (!id || id >= env->inst->objects.size
        || env->inst->objects.data[id].type != type)
    {
        return NUX_NULL;
    }
    return env->inst->objects.data[id].data;
}
void
nux_arena_cleanup (nux_env_t *env, void *data)
{
    nux_arena_t *arena = data;
    delete_objects(env, arena, NUX_NULL);
}

nux_u32_t
nux_new_arena (nux_env_t *env, nux_u32_t capa)
{
    nux_frame_t frame = nux_begin_frame(env);

    nux_u32_t    id;
    nux_arena_t *arena
        = NUX_NEW_STRUCT(env, NUX_OBJECT_ARENA, nux_arena_t, &id);
    NUX_CHECKM(arena, "Failed to allocate arena object", return NUX_FAILURE);
    arena->capa        = capa;
    arena->size        = 0;
    arena->last_object = NUX_NULL;
    arena->data        = nux_alloc(env, capa);
    NUX_CHECKM(arena->data, "Failed to allocate arena memory", goto cleanup);
    return id;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_NULL;
}
void
nux_reset_arena (nux_env_t *env)
{
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, env->arena);
    NUX_CHECKM(arena, "Invalid arena id", return);
    // Delete all objects
    delete_objects(env, arena, NUX_NULL);
    // Reset memory
    arena->last_object = NUX_NULL;
    arena->size        = 0;
}
void
nux_set_arena (nux_env_t *env, nux_u32_t id)
{
    NUX_CHECKM(
        nux_get_object(env, NUX_OBJECT_ARENA, id), "Invalid arena id", return);
    env->arena = id;
}

nux_frame_t
nux_begin_frame (nux_env_t *env)
{
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, env->arena);
    NUX_ASSERT(arena);
    return (nux_frame_t) {
        .arena  = env->arena,
        .size   = arena->size,
        .object = arena->last_object,
    };
}
void
nux_reset_frame (nux_env_t *env, nux_frame_t frame)
{
    nux_arena_t *arena = nux_get_object(env, NUX_OBJECT_ARENA, frame.arena);
    NUX_ASSERT(arena);
    delete_objects(env, arena, frame.object);
    arena->size = frame.size;
}
