#include "internal.h"

nux_u32_t
nux_generate_cube (nux_env_t *env, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz)
{
    const nux_b3_t box = nux_b3(NUX_V3_ZEROES, nux_v3(sx / 2, sy / 2, sz / 2));

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

    nux_mesh_t *mesh = nux_arena_alloc(env->active_arena, sizeof(*mesh));
    NUX_CHECK(mesh, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_MESH, mesh);
    NUX_CHECK(id, return NUX_NULL);
    mesh->count = NUX_ARRAY_SIZE(positions);
    mesh->data  = nux_arena_alloc(env->active_arena,
                                 sizeof(nux_f32_t) * 5 * mesh->count);
    NUX_CHECK(mesh->data, return NUX_NULL);

    for (nux_u32_t i = 0; i < mesh->count; ++i)
    {
        mesh->data[i * 5 + 0] = positions[i].x;
        mesh->data[i * 5 + 1] = positions[i].y;
        mesh->data[i * 5 + 2] = positions[i].z;
        mesh->data[i * 5 + 3] = uvs[i].x;
        mesh->data[i * 5 + 4] = uvs[i].y;
    }

    NUX_CHECK(
        nux_graphics_push_vertices(env, mesh->count, mesh->data, &mesh->first),
        return NUX_NULL);

    return id;
}
