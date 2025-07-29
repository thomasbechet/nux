#include "internal.h"

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
    // NUX_CHECK(nux_ecs_iter_vec_alloc(ctx, ECS_ITER_MAX, &ctx->iters),
    //           return NUX_FAILURE);
}
