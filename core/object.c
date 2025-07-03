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
delete_objects (nux_ctx_t *ctx, nux_arena_t *arena, nux_u32_t object)
{
    nux_u32_t next = arena->last_object;
    while (next != object)
    {
        nux_object_delete(ctx, next);
        next = ctx->objects.data[ID_INDEX(next)].prev;
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

nux_u32_t
nux_object_create (nux_ctx_t   *ctx,
                   nux_arena_t *arena,
                   nux_u32_t    type,
                   void        *data)
{
    nux_object_t *obj = nux_object_pool_add(&ctx->objects);
    NUX_CHECKM(obj, "Out of objects", return NUX_NULL);
    obj->type = type;
    obj->data = data;
    obj->version += 1;
    nux_u32_t index = obj - ctx->objects.data;
    nux_u32_t id    = BUILD_ID(index, obj->version);
    obj->prev       = arena->last_object;
    if (arena->last_object)
    {
        nux_object_t *prev_obj = &ctx->objects.data[ID_INDEX(obj->prev)];
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
nux_object_delete (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_object_t *obj  = &ctx->objects.data[ID_INDEX(id)];
    nux_type_t   *type = ctx->types + obj->type;
    if (type->cleanup)
    {
        type->cleanup(ctx, obj->data);
    }
    if (obj->prev)
    {
        nux_object_t *prev_obj = &ctx->objects.data[ID_INDEX(obj->prev)];
        prev_obj->next         = obj->next;
    }
    if (obj->next)
    {
        nux_object_t *next_obj = &ctx->objects.data[ID_INDEX(obj->next)];
        next_obj->prev         = obj->prev;
    }
    nux_object_pool_remove(&ctx->objects, obj);
}
void
nux_object_update (nux_ctx_t *ctx, nux_u32_t id, void *data)
{
    nux_object_t *obj = ctx->objects.data + ID_INDEX(id);
    obj->data         = data;
}
void *
nux_object_get (nux_ctx_t *ctx, nux_u32_t type_index, nux_u32_t id)
{
    nux_u32_t index   = ID_INDEX(id);
    nux_u8_t  version = ID_VERSION(id);
    if (index >= ctx->objects.size
        || ctx->objects.data[index].type != type_index
        || ctx->objects.data[index].version != version)
    {
        nux_type_t *got    = &ctx->types[ctx->objects.data[index].type];
        nux_type_t *expect = &ctx->types[type_index];
        NUX_ERROR("Invalid object type (expect \"%s\", got \"%s\")",
                  expect->name,
                  got->name);
        return NUX_NULL;
    }
    return ctx->objects.data[index].data;
}

void *
nux_arena_alloc (nux_arena_t *arena, nux_u32_t size)
{
    return arena_alloc(arena, NUX_NULL, 0, size);
}
void
nux_arena_reset_to (nux_ctx_t *ctx, nux_arena_t *arena, nux_u32_t object)
{
    delete_objects(ctx, arena, object);
}

nux_u32_t
nux_arena_new (nux_ctx_t *ctx)
{
    nux_arena_t *arena = nux_arena_pool_add(&ctx->arenas);
    NUX_CHECKM(arena, "Failed to allocate arena", return NUX_NULL);
    arena->id
        = nux_object_create(ctx, ctx->active_arena, NUX_TYPE_ARENA, arena);
    NUX_CHECK(arena->id, goto cleanup0);
    const nux_u32_t default_capa = 1 << 20;
    arena->capa                  = default_capa;
    arena->size                  = 0;
    arena->first_object          = NUX_NULL;
    arena->last_object           = NUX_NULL;
    arena->data = nux_arena_alloc(ctx->active_arena, arena->capa);
    NUX_CHECK(arena->data, goto cleanup1);
    return arena->id;

cleanup1:
    nux_object_delete(ctx, arena->id);
cleanup0:
    nux_arena_pool_remove(&ctx->arenas, arena);
    return NUX_NULL;
}
void
nux_arena_reset (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(ctx, NUX_TYPE_ARENA, id);
    NUX_CHECKM(arena, "Invalid arena id", return);
    // Delete all objects
    delete_objects(ctx, arena, NUX_NULL);
    // Reset memory
    arena->last_object = NUX_NULL;
    arena->size        = 0;
}
nux_status_t
nux_arena_use (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(ctx, NUX_TYPE_ARENA, id);
    NUX_CHECKM(arena, "Invalid arena id", return NUX_FAILURE);
    ctx->active_arena = arena;
    return NUX_SUCCESS;
}
nux_u32_t
nux_arena_active (nux_ctx_t *ctx)
{
    return ctx->active_arena->id;
}
void
nux_arena_dump (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_arena_t *arena = nux_object_get(ctx, NUX_TYPE_ARENA, id);
    NUX_CHECK(arena, return);
    NUX_INFO("ARENA : %d", id);
    nux_u32_t next = arena->first_object;
    while (next)
    {
        nux_object_t *obj  = &ctx->objects.data[ID_INDEX(next)];
        nux_type_t   *type = &ctx->types[obj->type];
        NUX_INFO("- %s : %d", type->name, next);
        next = obj->next;
    }
}
