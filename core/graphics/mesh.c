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
static nux_status_t
update_data (nux_mesh_t *mesh)
{
    return nux_graphics_update_vertices(mesh->vertex_offset,
                                        mesh->vertex_count
                                            * mesh->vertex_layout.stride,
                                        mesh->data);
}
static void
append_quad (nux_mesh_t *m, const nux_v3_t *positions, const nux_v2_t *uvs)
{
    switch (m->vertex_attributes)
    {
        case NUX_PRIMITIVE_POINTS: {
            nu__append_vertices(m, 4, positions, uvs);
        }
        break;
        case NU_PRIMITIVE_LINES: {
            const nu_size_t indices[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
            {
                nu__append_vertices(
                    m, 1, positions + indices[i], uvs + indices[i]);
            }
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
            nu__append_vertices(m, 4, positions, uvs);
            nu__append_vertices(m, 1, positions, uvs);
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            const nu_size_t indices[] = { 0, 1, 2, 2, 3, 0 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
            {
                nu__append_vertices(
                    m, 1, positions + indices[i], uvs + indices[i]);
            }
        }
        break;
    }
}

nux_mesh_t *
nux_mesh_new (nux_arena_t           *arena,
              nux_u32_t              capa,
              nux_vertex_attribute_t attributes)
{
    nux_mesh_t *mesh = nux_resource_new(arena, NUX_RESOURCE_MESH);
    NUX_CHECK(mesh, return NUX_NULL);
    mesh->vertex_attributes = attributes;
    mesh->vertex_layout     = vertex_layout(mesh->vertex_attributes);
    mesh->vertex_count      = capa;
    mesh->data              = nux_arena_malloc(arena,
                                  sizeof(nux_f32_t) * mesh->vertex_layout.stride
                                      * mesh->vertex_count);
    NUX_CHECK(mesh->data, return NUX_NULL);
    mesh->bounds = nux_b3(NUX_V3_ZEROS, NUX_V3_ZEROS);
    NUX_CHECK(nux_graphics_push_vertices(mesh->vertex_count
                                             * mesh->vertex_layout.stride,
                                         mesh->data,
                                         &mesh->vertex_offset),
              return NUX_NULL);
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
                                    NUX_VERTEX_POSITION | NUX_VERTEX_TEXCOORD);
    NUX_CHECK(mesh, return NUX_NULL);
    mesh->bounds = box;

    for (nux_u32_t i = 0; i < mesh->vertex_count; ++i)
    {
        nux_u32_t offset = i * mesh->vertex_layout.stride;
        mesh->data[offset + mesh->vertex_layout.position + 0] = positions[i].x;
        mesh->data[offset + mesh->vertex_layout.position + 1] = positions[i].y;
        mesh->data[offset + mesh->vertex_layout.position + 2] = positions[i].z;
        mesh->data[offset + mesh->vertex_layout.texcoord + 0] = uvs[i].x;
        mesh->data[offset + mesh->vertex_layout.texcoord + 1] = uvs[i].y;
    }

    NUX_CHECK(update_data(mesh), return NUX_NULL);

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
                                    NUX_VERTEX_POSITION | NUX_VERTEX_TEXCOORD);
    NUX_CHECK(mesh, return NUX_NULL);

    for (nux_u32_t i = 0; i < mesh->vertex_count; ++i)
    {
        nux_u32_t offset = i * mesh->vertex_layout.stride;
        mesh->data[offset + mesh->vertex_layout.position + 0] = positions[i].x;
        mesh->data[offset + mesh->vertex_layout.position + 1] = positions[i].y;
        mesh->data[offset + mesh->vertex_layout.position + 2] = positions[i].z;
        mesh->data[offset + mesh->vertex_layout.texcoord + 0] = uvs[i].x;
        mesh->data[offset + mesh->vertex_layout.texcoord + 1] = uvs[i].y;
    }

    NUX_CHECK(update_data(mesh), return NUX_NULL);

    return mesh;
}
void
nux_mesh_update_bounds (nux_mesh_t *mesh)
{
    if (mesh->vertex_count == 0)
    {
        return;
    }

    nux_v3_t min = nux_v3s(NUX_FLT_MAX);
    nux_v3_t max = nux_v3s(NUX_FLT_MIN);
    for (nux_u32_t i = 0; i < mesh->vertex_count; ++i)
    {
        nux_u32_t offset = i * mesh->vertex_layout.stride;
        nux_v3_t  v
            = nux_v3(mesh->data[offset + mesh->vertex_layout.position + 0],
                     mesh->data[offset + mesh->vertex_layout.position + 1],
                     mesh->data[offset + mesh->vertex_layout.position + 2]);
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
    for (nux_u32_t i = 0; i < mesh->vertex_count; ++i)
    {
        nux_u32_t offset = i * mesh->vertex_layout.stride;
        mesh->data[offset + mesh->vertex_layout.position + 0] -= origin.x;
        mesh->data[offset + mesh->vertex_layout.position + 1] -= origin.y;
        mesh->data[offset + mesh->vertex_layout.position + 2] -= origin.z;
    }
    update_data(mesh);
    nux_mesh_update_bounds(mesh);
}
