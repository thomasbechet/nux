#include "internal.h"

nux_rid_t
nux_mesh_new (nux_ctx_t *ctx, nux_rid_t arena, nux_u32_t capa)
{
    nux_rid_t   res;
    nux_mesh_t *mesh = nux_resource_new(ctx, arena, NUX_RESOURCE_MESH, &res);
    NUX_CHECK(mesh, return NUX_NULL);
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL;)
    mesh->count = capa;
    mesh->data  = nux_arena_malloc(
        a, sizeof(nux_f32_t) * NUX_VERTEX_SIZE * mesh->count);
    NUX_CHECK(mesh->data, return NUX_NULL);
    mesh->bounds = nux_b3(NUX_V3_ZEROS, NUX_V3_ZEROS);
    return res;
}
nux_rid_t
nux_mesh_new_cube (
    nux_ctx_t *ctx, nux_rid_t arena, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz)
{
    const nux_b3_t box = nux_b3(NUX_V3_ZEROS, nux_v3(sx, sy, sz));

    const nux_v3_t v0 = nux_v3(box.min.x, box.min.y, box.min.z);
    const nux_v3_t v1 = nux_v3(box.max.x, box.min.y, box.min.z);
    const nux_v3_t v2 = nux_v3(box.max.x, box.min.y, box.max.z);
    const nux_v3_t v3 = nux_v3(box.min.x, box.min.y, box.max.z);

    const nux_v3_t v4 = nux_v3(box.min.x, box.max.y, box.min.z);
    const nux_v3_t v5 = nux_v3(box.max.x, box.max.y, box.min.z);
    const nux_v3_t v6 = nux_v3(box.max.x, box.max.y, box.max.z);
    const nux_v3_t v7 = nux_v3(box.min.x, box.max.y, box.max.z);

    const nux_v3_t positions[]
        = { v0, v1, v2, v2, v3, v0, v4, v6, v5, v6, v4, v7,
            v0, v3, v7, v7, v4, v0, v1, v5, v6, v6, v2, v1,
            v0, v4, v5, v5, v1, v0, v3, v2, v6, v6, v7, v3 };

    const nux_v2_t uvs[] = {
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } }, { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 0, 0 } }, { { 0, 1 } }, { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 1, 0 } },
        { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } },
        { { 0, 1 } }, { { 1, 1 } }, { { 1, 1 } }, { { 1, 0 } }, { { 0, 0 } },
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } },
    };

    nux_rid_t id = nux_mesh_new(ctx, arena, NUX_ARRAY_SIZE(positions));
    NUX_CHECK(id, return NUX_NULL);
    nux_mesh_t *mesh = nux_resource_check(ctx, NUX_RESOURCE_MESH, id);
    mesh->bounds     = box;

    for (nux_u32_t i = 0; i < mesh->count; ++i)
    {
        mesh->data[i * 5 + 0] = positions[i].x;
        mesh->data[i * 5 + 1] = positions[i].y;
        mesh->data[i * 5 + 2] = positions[i].z;
        mesh->data[i * 5 + 3] = uvs[i].x;
        mesh->data[i * 5 + 4] = uvs[i].y;
    }

    NUX_CHECK(
        nux_graphics_push_vertices(ctx, mesh->count, mesh->data, &mesh->first),
        return NUX_NULL);

    return id;
}
void
nux_mesh_update_bounds (nux_ctx_t *ctx, nux_rid_t mesh)
{
    nux_mesh_t *m = nux_resource_check(ctx, NUX_RESOURCE_MESH, mesh);
    NUX_CHECK(m, return);
    if (m->count == 0)
    {
        return;
    }

    nux_v3_t min = nux_v3s(NUX_FLT_MAX);
    nux_v3_t max = nux_v3s(NUX_FLT_MIN);
    for (nux_u32_t i = 0; i < m->count; ++i)
    {
        nux_v3_t v = nux_v3(
            m->data[i * 5 + 0], m->data[i * 5 + 1], m->data[i * 5 + 2]);
        min = nux_v3_min(min, v);
        max = nux_v3_max(max, v);
    }

    m->bounds = nux_b3(min, max);
}
nux_v3_t
nux_mesh_bounds_min (nux_ctx_t *ctx, nux_rid_t mesh)
{
    nux_mesh_t *m = nux_resource_check(ctx, NUX_RESOURCE_MESH, mesh);
    NUX_CHECK(m, return NUX_V3_ZEROS);
    return m->bounds.min;
}
nux_v3_t
nux_mesh_bounds_max (nux_ctx_t *ctx, nux_rid_t mesh)
{
    nux_mesh_t *m = nux_resource_check(ctx, NUX_RESOURCE_MESH, mesh);
    NUX_CHECK(m, return NUX_V3_ZEROS);
    return m->bounds.max;
}
