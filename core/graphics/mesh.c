#include "internal.h"

static nux_vertex_layout_t
vertex_layout (nux_vertex_attribute_t attributes)
{
    nux_vertex_layout_t layout;
    layout.stride   = 0;
    layout.position = 0;
    layout.texcoord = 0;
    layout.color    = 0;
    if (attributes & NUX_VERTEX_POSITION)
    {
        layout.position = layout.stride;
        layout.stride += 3;
    }
    if (attributes & NUX_VERTEX_TEXCOORD)
    {
        layout.texcoord = layout.stride;
        layout.stride += 2;
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        layout.color = layout.stride;
        layout.stride += 3;
    }
    return layout;
}
static void
push_quad (nux_mesh_t *m, const nux_v3_t *positions, const nux_v2_t *uvs)
{
    switch (m->primitive)
    {
        case NUX_VERTEX_POINTS: {
            nux_mesh_push_vertices(m, 4, positions, uvs, NUX_NULL);
        }
        break;
        case NUX_VERTEX_LINES: {
            const nux_u32_t indices[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
            for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(indices); ++i)
            {
                nux_mesh_push_vertices(
                    m, 1, positions + indices[i], uvs + indices[i], NUX_NULL);
            }
        }
        break;
        // case NU_PRIMITIVE_LINES_STRIP: {
        //     nux_mesh_push_vertices(m, 4, positions, uvs, NUX_NULL);
        //     nux_mesh_push_vertices(m, 1, positions, uvs, NUX_NULL);
        // }
        // break;
        case NUX_VERTEX_TRIANGLES: {
            const nux_u32_t indices[] = { 0, 1, 2, 2, 3, 0 };
            for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(indices); ++i)
            {
                nux_mesh_push_vertices(
                    m, 1, positions + indices[i], uvs + indices[i], NUX_NULL);
            }
        }
        break;
    }
}
// static void
// nu__append_triangle (nu__mesh_t    *m,
//                      const nu_v3_t *positions,
//                      const nu_v2_t *uvs)
// {
//     switch (m->primitive)
//     {
//         case NU_PRIMITIVE_POINTS:
//             nu__append_vertices(m, 3, positions, uvs);
//             break;
//         case NU_PRIMITIVE_LINES: {
//             const nu_size_t indices[] = { 0, 1, 1, 2, 2, 0 };
//             for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
//             {
//                 nu__append_vertices(
//                     m, 1, positions + indices[i], uvs + indices[i]);
//             }
//         }
//         break;
//         case NU_PRIMITIVE_LINES_STRIP:
//             nu__append_vertices(m, 3, positions, uvs);
//             break;
//         case NU_PRIMITIVE_TRIANGLES:
//             nu__append_vertices(m, 3, positions, uvs);
//             break;
//     }
// }

nux_status_t
nux_mesh_upload (nux_mesh_t *mesh)
{
    if (mesh->dirty)
    {
        nux_u32_t count = nux_mesh_size(mesh);
        if (count > mesh->gpu.capacity)
        {
            mesh->gpu.capacity = count;
            NUX_CHECK(nux_graphics_push_vertices(count * mesh->layout.stride,
                                                 mesh->vertices.data,
                                                 &mesh->gpu.offset),
                      return NUX_FAILURE);
        }
        else
        {
            NUX_CHECK(nux_graphics_update_vertices(mesh->gpu.offset,
                                                   count * mesh->layout.stride,
                                                   mesh->vertices.data),
                      return NUX_FAILURE);
        }
        mesh->dirty = NUX_FALSE;
    }
    return NUX_SUCCESS;
}
void
nux_mesh_push_vertices (nux_mesh_t     *mesh,
                        nux_u32_t       count,
                        const nux_v3_t *positions,
                        const nux_v2_t *uvs,
                        const nux_v3_t *colors)
{
    nux_u32_t vertex_count = nux_mesh_size(mesh);
    nux_f32_vec_resize(&mesh->vertices,
                       mesh->vertices.size + count * mesh->layout.stride);
    nux_f32_t *data = mesh->vertices.data + vertex_count * mesh->layout.stride;
    for (nux_u32_t i = 0; i < count; ++i)
    {
        nux_u32_t offset = i * mesh->layout.stride;
        if (positions)
        {
            data[offset + mesh->layout.position + 0] = positions[i].x;
            data[offset + mesh->layout.position + 1] = positions[i].y;
            data[offset + mesh->layout.position + 2] = positions[i].z;
        }
        if (uvs)
        {
            data[offset + mesh->layout.texcoord + 0] = uvs[i].x;
            data[offset + mesh->layout.texcoord + 1] = uvs[i].y;
        }
        if (colors)
        {
            data[offset + mesh->layout.color + 0] = colors[i].x;
            data[offset + mesh->layout.color + 1] = colors[i].y;
            data[offset + mesh->layout.color + 2] = colors[i].z;
        }
    }
    mesh->dirty = NUX_TRUE;
}

nux_mesh_t *
nux_mesh_new (nux_arena_t           *arena,
              nux_u32_t              capa,
              nux_vertex_attribute_t attributes,
              nux_vertex_primitive_t primitive)
{
    nux_mesh_t *mesh = nux_resource_new(arena, NUX_RESOURCE_MESH);
    NUX_CHECK(mesh, return NUX_NULL);
    mesh->attributes = attributes;
    mesh->primitive  = primitive;
    mesh->layout     = vertex_layout(mesh->attributes);
    NUX_CHECK(nux_f32_vec_init_capa(nux_arena_allocator(arena),
                                    mesh->layout.stride * capa,
                                    &mesh->vertices),
              return NUX_NULL);
    mesh->bounds       = nux_b3(NUX_V3_ZEROS, NUX_V3_ZEROS);
    mesh->dirty        = NUX_TRUE;
    mesh->gpu.offset   = 0;
    mesh->gpu.capacity = 0;
    return mesh;
}
nux_mesh_t *
nux_mesh_new_cube (nux_arena_t *arena, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz)
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

    nux_mesh_t *mesh = nux_mesh_new(arena,
                                    NUX_ARRAY_SIZE(positions),
                                    NUX_VERTEX_POSITION | NUX_VERTEX_TEXCOORD,
                                    NUX_VERTEX_TRIANGLES);
    NUX_CHECK(mesh, return NUX_NULL);
    mesh->bounds = box;

    nux_mesh_push_vertices(
        mesh, NUX_ARRAY_SIZE(positions), positions, uvs, NUX_NULL);

    return mesh;
}
nux_mesh_t *
nux_mesh_new_plane (nux_arena_t *arena, nux_f32_t sx, nux_f32_t sy)
{
    const nux_v3_t positions[4] = {
        { 0, 0, sy },
        { sx, 0, sy },
        { sx, 0, 0 },
        { 0, 0, 0 },
    };
    const nux_v2_t uvs[4] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } };

    nux_mesh_t *mesh = nux_mesh_new(arena,
                                    NUX_ARRAY_SIZE(positions),
                                    NUX_VERTEX_POSITION | NUX_VERTEX_TEXCOORD,
                                    NUX_VERTEX_TRIANGLES);
    NUX_CHECK(mesh, return NUX_NULL);

    push_quad(mesh, positions, uvs);

    return mesh;
}
void
nux_mesh_update_bounds (nux_mesh_t *mesh)
{
    nux_u32_t count = nux_mesh_size(mesh);
    if (mesh == 0)
    {
        return;
    }

    nux_v3_t min = nux_v3s(NUX_FLT_MAX);
    nux_v3_t max = nux_v3s(NUX_FLT_MIN);
    for (nux_u32_t i = 0; i < count; ++i)
    {
        nux_u32_t offset = i * mesh->layout.stride;
        nux_v3_t  v
            = nux_v3(mesh->vertices.data[offset + mesh->layout.position + 0],
                     mesh->vertices.data[offset + mesh->layout.position + 1],
                     mesh->vertices.data[offset + mesh->layout.position + 2]);
        min = nux_v3_min(min, v);
        max = nux_v3_max(max, v);
    }

    mesh->bounds = nux_b3(min, max);
}
nux_v3_t
nux_mesh_bounds_min (nux_mesh_t *mesh)
{
    return mesh->bounds.min;
}
nux_v3_t
nux_mesh_bounds_max (nux_mesh_t *mesh)
{
    return mesh->bounds.max;
}
void
nux_mesh_set_origin (nux_mesh_t *mesh, nux_v3_t origin)
{
    nux_mesh_transform(mesh, nux_m4_translate(nux_v3_muls(origin, -1)));
}
void
nux_mesh_transform (nux_mesh_t *mesh, nux_m4_t transform)
{
    nux_u32_t count = nux_mesh_size(mesh);
    for (nux_u32_t i = 0; i < count; ++i)
    {
        nux_u32_t offset = i * mesh->layout.stride;
        nux_v3_t  v
            = nux_v3(mesh->vertices.data[offset + mesh->layout.position + 0],
                     mesh->vertices.data[offset + mesh->layout.position + 1],
                     mesh->vertices.data[offset + mesh->layout.position + 2]);
        v = nux_m4_mulv3(transform, v, 1);
        mesh->vertices.data[offset + mesh->layout.position + 0] = v.x;
        mesh->vertices.data[offset + mesh->layout.position + 1] = v.y;
        mesh->vertices.data[offset + mesh->layout.position + 2] = v.z;
    }
    mesh->dirty = NUX_TRUE;
    nux_mesh_update_bounds(mesh);
}
nux_u32_t
nux_mesh_size (nux_mesh_t *mesh)
{
    NUX_ASSERT(mesh->layout.stride);
    return mesh->vertices.size / mesh->layout.stride;
}
