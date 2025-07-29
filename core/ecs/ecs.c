#include "internal.h"

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
static nux_u32_t
ecs_bitset_find_unset (const nux_ecs_bitset_t *bitset)
{
    for (nux_u32_t i = 0; i < bitset->size; ++i)
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
            return i * ECS_ENTITY_PER_MASK + index;
        }
    }
    return bitset->size * ECS_ENTITY_PER_MASK;
}
static nux_u32_t
ecs_bitset_capacity (const nux_ecs_bitset_t *bitset)
{
    return bitset->capa * ECS_ENTITY_PER_MASK;
}

nux_status_t
nux_ecs_init (nux_ctx_t *ctx)
{
    NUX_CHECK(
        nux_ecs_component_vec_alloc(ctx, ECS_COMPONENT_MAX, &ctx->components),
        return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_u32_t
nux_ecs_register_component (nux_ctx_t      *ctx,
                            const nux_c8_t *name,
                            nux_u32_t       size)
{
    nux_ecs_component_t *comp = nux_ecs_component_vec_push(&ctx->components);
    NUX_ENSURE(comp, return NUX_NULL, "max ecs component count reached");
    nux_strncpy(comp->name, name, ECS_COMPONENT_NAME_LEN);
    comp->size = size;
    return ID_MAKE(ctx->components.size - 1);
}

nux_res_t
nux_ecs_new_iter (nux_ctx_t *ctx,
                  nux_u32_t  include_count,
                  nux_u32_t  exclude_count)
{
}
void
nux_ecs_includes (nux_ctx_t *ctx, nux_u32_t iter, nux_u32_t c)
{
}
void
nux_ecs_excludes (nux_ctx_t *ctx, nux_u32_t iter, nux_u32_t c)
{
}
nux_u32_t
nux_ecs_begin (nux_ctx_t *ctx, nux_res_t ecs, nux_u32_t iter)
{
}
nux_u32_t
nux_ecs_next (nux_ctx_t *ctx, nux_res_t ecs, nux_u32_t iter)
{
}
