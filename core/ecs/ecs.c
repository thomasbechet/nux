#include "internal.h"

#include "lua_code.c.inc"

NUX_VEC_IMPL(nux_ecs_bitset, nux_ecs_mask_t);
NUX_VEC_IMPL(nux_ecs_chunk_vec, void *);
NUX_VEC_IMPL(nux_ecs_container_vec, nux_ecs_container_t);

#define ID_INDEX(id)   (id - 1)
#define ID_MAKE(index) (index + 1)

static void
ecs_mask_set (nux_ecs_mask_t *mask, nux_u32_t n)
{
    *mask = *mask | (1 << n);
}
static void
ecs_mask_unset (nux_ecs_mask_t *mask, nux_u32_t n)
{
    *mask = *mask & ~(1 << n);
}
static nux_b32_t
ecs_mask_isset (nux_ecs_mask_t mask, nux_u32_t n)
{
    return (mask >> n) & 1;
}
static nux_u32_t
ecs_mask_count (nux_ecs_mask_t mask)
{
    nux_u32_t c;
    for (c = 0; mask; mask >>= 1)
    {
        c += mask & 1;
    }
    return c;
}
static nux_b32_t
ecs_bitset_isset (const nux_ecs_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / ECS_ENTITY_PER_MASK;
    nux_u32_t offset = index % ECS_ENTITY_PER_MASK;
    return (bitset->size > mask && ecs_mask_isset(bitset->data[mask], offset));
}
static nux_status_t
ecs_bitset_set (nux_ecs_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / ECS_ENTITY_PER_MASK;
    nux_u32_t offset = index % ECS_ENTITY_PER_MASK;
    if (bitset->size <= mask)
    {
        if (mask >= bitset->capa)
        {
            return NUX_FAILURE; // max mask count reached
        }
        bitset->size = mask + 1;
        // masks are already initialized to 0
    }
    ecs_mask_set(&bitset->data[mask], offset);
    return NUX_SUCCESS;
}
static void
ecs_bitset_unset (nux_ecs_bitset_t *bitset, nux_u32_t index)
{
    nux_u32_t mask   = index / ECS_ENTITY_PER_MASK;
    nux_u32_t offset = index % ECS_ENTITY_PER_MASK;
    if (bitset->size > mask)
    {
        ecs_mask_unset(&bitset->data[mask], offset);
    }
}
static nux_ecs_mask_t
ecs_bitset_mask (const nux_ecs_bitset_t *bitset, nux_u32_t mask_index)
{
    if (mask_index >= bitset->size)
    {
        return 0;
    }
    return bitset->data[mask_index];
}
static nux_u32_t
ecs_bitset_count (const nux_ecs_bitset_t *bitset)
{
    nux_u32_t n = 0;
    for (nux_u32_t i = 0; i < bitset->size; ++i)
    {
        n += ecs_mask_count(bitset->data[i]);
    }
    return n;
}
static nux_b32_t
ecs_bitset_find_unset (const nux_ecs_bitset_t *bitset, nux_u32_t *found)
{
    for (nux_u32_t i = 0; i < bitset->capa; ++i)
    {
        nux_ecs_mask_t mask = bitset->data[i];
        if (bitset->data[i] != 0xFFFFFFFF)
        {
            nux_u32_t index = 0;
            while (mask & 0x1)
            {
                mask >>= 1;
                ++index;
            }
            *found = i * ECS_ENTITY_PER_MASK + index;
            return NUX_TRUE;
        }
    }
    return NUX_FALSE;
}
static nux_u32_t
ecs_bitset_capacity (const nux_ecs_bitset_t *bitset)
{
    return bitset->capa * ECS_ENTITY_PER_MASK;
}
static nux_ecs_t *
ecs_active (nux_ctx_t *ctx)
{
    NUX_ENSURE(ctx->ecs->active_ecs,
               return NUX_NULL,
               "using ecs api with no active ecs instance");
    return ctx->ecs->active_ecs;
}

nux_status_t
nux_ecs_init (nux_ctx_t *ctx)
{
    ctx->ecs = nux_arena_alloc(ctx, ctx->core_arena_rid, sizeof(*ctx->ecs));
    NUX_CHECK(ctx->ecs, return NUX_FAILURE);

    nux_ecs_module_t *module = ctx->ecs;

    // Register types
    nux_resource_type_t *type;
    type          = nux_resource_register(ctx, NUX_RESOURCE_ECS, "ecs");
    type->cleanup = nux_ecs_cleanup;
    type = nux_resource_register(ctx, NUX_RESOURCE_ECS_ITER, "ecs_iter");

    // Initialize values
    nux_memset(module->components, 0, sizeof(module->components));
    module->components_max = 0;
    ctx->ecs->active_ecs   = NUX_NULL;

    // Register components
    nux_ecs_register_component(
        ctx, NUX_COMPONENT_TRANSFORM, "transform", sizeof(nux_transform_t));

    // Register lua api
    nux_lua_open_ecs(ctx);
    nux_lua_dostring(ctx, lua_data_code);

    return NUX_SUCCESS;
}
void
nux_ecs_free (nux_ctx_t *ctx)
{
}
void
nux_ecs_register_component (nux_ctx_t      *ctx,
                            nux_u32_t       index,
                            const nux_c8_t *name,
                            nux_u32_t       size)
{
    nux_ecs_module_t *module = ctx->ecs;
    NUX_ASSERT(index != 0);
    NUX_ASSERT(index < NUX_COMPONENT_MAX);
    NUX_ASSERT(module->components[index].size == 0);
    nux_ecs_component_t *comp = &module->components[index];
    nux_strncpy(comp->name, name, ECS_COMPONENT_NAME_LEN);
    comp->size             = size;
    module->components_max = NUX_MAX(module->components_max, index + 1);
}

nux_rid_t
nux_ecs_new_iter (nux_ctx_t *ctx,
                  nux_rid_t  arena,
                  nux_u32_t  include_count,
                  nux_u32_t  exclude_count)
{
    nux_rid_t       rid;
    nux_ecs_iter_t *it = nux_resource_new(
        ctx, arena, NUX_RESOURCE_ECS_ITER, sizeof(*it), &rid);
    NUX_CHECK(rid, return NUX_NULL);
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    if (include_count)
    {
        NUX_CHECK(nux_u32_vec_alloc(ctx, a, include_count, &it->includes),
                  return NUX_NULL);
    }
    if (exclude_count)
    {
        NUX_CHECK(nux_u32_vec_alloc(ctx, a, exclude_count, &it->excludes),
                  return NUX_NULL);
    }
    return rid;
}
void
nux_ecs_includes (nux_ctx_t *ctx, nux_rid_t iter, nux_u32_t c)
{
    nux_ecs_iter_t *it = nux_resource_check(ctx, NUX_RESOURCE_ECS_ITER, iter);
    NUX_CHECK(it, return);
    nux_u32_vec_pushv(&it->includes, c);
}
void
nux_ecs_excludes (nux_ctx_t *ctx, nux_rid_t iter, nux_u32_t c)
{
    nux_ecs_iter_t *it = nux_resource_check(ctx, NUX_RESOURCE_ECS_ITER, iter);
    NUX_CHECK(it, return);
    nux_u32_vec_pushv(&it->excludes, c);
}
static nux_u32_t
ecs_iter_next (const nux_ecs_t *ins, nux_ecs_iter_t *it)
{
    // find next non empty mask
    while (!it->mask)
    {
        ++it->mask_index;
        const nux_u32_t max_mask_count = ins->bitset.size;
        if (it->mask_index >= max_mask_count)
        {
            // reach last mask
            return NUX_NULL;
        }

        // initialize mask with existing entities
        it->mask = ecs_bitset_mask(&ins->bitset, it->mask_index);
        // filter on required components
        for (nux_u32_t i = 0; i < it->includes.size; ++i)
        {
            nux_ecs_container_t *container
                = ins->containers.data + it->includes.data[i];
            it->mask &= ecs_bitset_mask(&container->bitset, it->mask_index);
        }
        // must be processed after includes
        for (nux_u32_t i = 0; i < it->excludes.size; ++i)
        {
            nux_ecs_container_t *container
                = ins->containers.data + it->excludes.data[i];
            it->mask &= ~ecs_bitset_mask(&container->bitset, it->mask_index);
        }

        it->mask_offset = 0;
    }

    // consume mask (at this step, the mask has at least one bit set)
    while (!(it->mask & 1))
    {
        it->mask >>= 1;
        ++it->mask_offset;
    }
    it->mask >>= 1;
    nux_u32_t offset = it->mask_offset++;
    return ID_MAKE(it->mask_index * ECS_ENTITY_PER_MASK + offset);
}
nux_u32_t
nux_ecs_next (nux_ctx_t *ctx, nux_rid_t iter, nux_ent_t e)
{
    nux_ecs_iter_t *it = nux_resource_check(ctx, NUX_RESOURCE_ECS_ITER, iter);
    NUX_CHECK(it, return NUX_NULL);
    if (!e) // initialize iterator
    {
        it->ecs         = nux_ecs_get_active(ctx);
        it->mask        = 0;
        it->mask_offset = 0;
        it->mask_index  = (nux_u32_t)-1; // trick for first iteration
    }
    nux_ecs_t *ins = nux_resource_check(ctx, NUX_RESOURCE_ECS, it->ecs);
    NUX_CHECK(ins, return NUX_NULL);
    return ecs_iter_next(ins, it);
}

nux_rid_t
nux_ecs_new (nux_ctx_t *ctx, nux_rid_t arena, nux_u32_t capa)
{
    nux_ecs_module_t *module = ctx->ecs;
    nux_rid_t         res;
    nux_arena_t      *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    nux_ecs_t *ins
        = nux_resource_new(ctx, arena, NUX_RESOURCE_ECS, sizeof(*ins), &res);
    NUX_CHECK(ins, return NUX_NULL);
    ins->arena = a;
    ins->self  = res;
    NUX_CHECK(nux_ecs_container_vec_alloc(
                  ctx, a, module->components_max, &ins->containers),
              return NUX_NULL);
    NUX_CHECK(nux_ecs_bitset_alloc(
                  ctx, a, (capa / ECS_ENTITY_PER_MASK) + 1, &ins->bitset),
              return NUX_NULL);
    return res;
}
nux_rid_t
nux_ecs_get_active (nux_ctx_t *ctx)
{
    return ctx->ecs->active_ecs ? ctx->ecs->active_ecs->self : NUX_NULL;
}
void
nux_ecs_set_active (nux_ctx_t *ctx, nux_rid_t ecs)
{
    nux_ecs_module_t *module = ctx->ecs;
    if (ecs)
    {
        nux_ecs_t *e = nux_resource_check(ctx, NUX_RESOURCE_ECS, ecs);
        NUX_CHECK(e, return);
        module->active_ecs = e;
    }
    else
    {
        module->active_ecs = NUX_NULL;
    }
}
nux_u32_t
nux_ecs_create (nux_ctx_t *ctx)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return NUX_NULL);
    nux_u32_t index;
    NUX_ENSURE(ecs_bitset_find_unset(&ins->bitset, &index),
               return NUX_NULL,
               "out of entities");
    ecs_bitset_set(&ins->bitset, index);
    return ID_MAKE(index);
}
void
nux_ecs_create_at (nux_ctx_t *ctx, nux_u32_t e)
{
    NUX_CHECK(e, return);
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return);
    if (nux_ecs_valid(ctx, e))
    {
        return;
    }
    ecs_bitset_set(&ins->bitset, ID_INDEX(e));
}
void
nux_ecs_delete (nux_ctx_t *ctx, nux_u32_t e)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return);
    nux_u32_t index = ID_INDEX(e);

    // remove from components
    for (nux_u32_t i = 0; i < ins->containers.size; ++i)
    {
        nux_ecs_remove(ctx, e, ID_MAKE(i));
    }

    // mask entity as invalid
    ecs_bitset_unset(&ins->bitset, index);
}
nux_b32_t
nux_ecs_valid (nux_ctx_t *ctx, nux_u32_t e)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return NUX_FALSE);
    nux_u32_t index = ID_INDEX(e);
    return ecs_bitset_isset(&ins->bitset, index);
}
nux_u32_t
nux_ecs_count (nux_ctx_t *ctx)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return 0);
    return ecs_bitset_count(&ins->bitset);
}
nux_u32_t
nux_ecs_capacity (nux_ctx_t *ctx)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return 0);
    return ins->bitset.capa * ECS_ENTITY_PER_MASK;
}
void
nux_ecs_clear (nux_ctx_t *ctx)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return);
    for (nux_u32_t i = 0; i < ins->containers.size; ++i)
    {
        nux_ecs_container_t *container = ins->containers.data + i;
        nux_ecs_bitset_clear(&container->bitset);
    }
    nux_ecs_bitset_clear(&ins->bitset);
}
void *
nux_ecs_add (nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c)
{
    nux_ecs_module_t *module = ctx->ecs;
    nux_ecs_t        *ins    = ecs_active(ctx);
    NUX_ENSURE(ins,
               return NUX_NULL,
               "trying to add component %d on entity %e but no ecs is active",
               c,
               e);
    nux_u32_t index = ID_INDEX(e);

    const nux_ecs_component_t *component = module->components + c;

    NUX_ENSURE(
        c < ins->containers.capa, return NUX_NULL, "invalid ecs component id");

    // initialize pool if component missing
    if (c >= ins->containers.size)
    {
        nux_u32_t prev_size  = ins->containers.size;
        ins->containers.size = c + 1;
        for (nux_u32_t i = prev_size; i < ins->containers.size; ++i)
        {
            nux_ecs_container_t *container = ins->containers.data + i;
            container->component_size      = module->components[i].size;
            NUX_CHECK(
                nux_ecs_chunk_vec_alloc(
                    ctx, ins->arena, ins->bitset.capa, &container->chunks),
                return NUX_NULL);
            NUX_CHECK(
                nux_ecs_bitset_alloc(
                    ctx, ins->arena, ins->bitset.capa, &container->bitset),
                return NUX_NULL);
            for (nux_u32_t i = 0; i < ins->bitset.capa; ++i)
            {
                container->chunks.data[i] = NUX_NULL;
                container->bitset.data[i] = 0;
            }
        }
    }

    nux_ecs_container_t *container = ins->containers.data + c;

    // check existing component
    if (!ecs_bitset_isset(&container->bitset, index))
    {
        // update bitset
        ecs_bitset_set(&container->bitset, index);

        // check if chunk exists
        nux_u32_t mask = index / ECS_ENTITY_PER_MASK;
        if (!container->chunks.data[mask])
        {
            // allocate new chunk
            container->chunks.data[mask] = nux_arena_alloc_raw(
                ctx,
                ins->arena,
                container->component_size * ECS_ENTITY_PER_MASK);
            // expect zero memory by default
            nux_memset(container->chunks.data[mask],
                       0,
                       container->component_size * ECS_ENTITY_PER_MASK);
        }
    }

    return nux_ecs_get(ctx, e, c);
}
void
nux_ecs_remove (nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c)
{
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return);
    nux_u32_t            index     = ID_INDEX(e);
    nux_ecs_container_t *container = ins->containers.data + c;
    ecs_bitset_unset(&container->bitset, index);
}
nux_b32_t
nux_ecs_has (nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c)
{
    NUX_ASSERT(e);
    NUX_ASSERT(c);
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return NUX_FALSE);
    nux_u32_t            index     = ID_INDEX(e);
    nux_ecs_container_t *container = ins->containers.data + c;
    return ecs_bitset_isset(&container->bitset, index);
}
void *
nux_ecs_get (nux_ctx_t *ctx, nux_u32_t e, nux_u32_t c)
{
    NUX_ASSERT(e);
    NUX_ASSERT(c);
    if (!nux_ecs_has(ctx, e, c))
    {
        return NUX_NULL;
    }
    nux_ecs_t *ins = ecs_active(ctx);
    NUX_CHECK(ins, return NUX_FALSE);
    nux_u32_t            index     = ID_INDEX(e);
    nux_u32_t            mask      = index / ECS_ENTITY_PER_MASK;
    nux_u32_t            offset    = index % ECS_ENTITY_PER_MASK;
    nux_ecs_container_t *container = ins->containers.data + c;
    return (void *)((nux_intptr_t)container->chunks.data[mask]
                    + container->component_size * offset);
}

void
nux_ecs_cleanup (nux_ctx_t *ctx, nux_rid_t res)
{
    nux_ecs_module_t *module = ctx->ecs;
    nux_ecs_t        *ecs    = nux_resource_check(ctx, NUX_RESOURCE_ECS, res);
    if (module->active_ecs == ecs)
    {
        module->active_ecs = NUX_NULL;
    }
}
