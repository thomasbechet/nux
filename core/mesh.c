#include "internal.h"

#ifdef NUX_BUILD_IMPORTER
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

nux_u32_t
nux_mesh_new (nux_ctx_t *ctx, nux_u32_t capa)
{
    nux_mesh_t *mesh = nux_arena_alloc(ctx->active_arena, sizeof(*mesh));
    NUX_CHECK(mesh, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(ctx, ctx->active_arena, NUX_OBJECT_MESH, mesh);
    NUX_CHECK(id, return NUX_NULL);
    mesh->count = capa;
    mesh->data  = nux_arena_alloc(ctx->active_arena,
                                 sizeof(nux_f32_t) * 5 * mesh->count);
    NUX_CHECK(mesh->data, return NUX_NULL);
    return id;
}
nux_u32_t
nux_mesh_gen_cube (nux_ctx_t *ctx, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz)
{
    const nux_b3_t box = nux_b3(NUX_V3_ZEROES, nux_v3(sx, sy, sz));

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

    nux_u32_t id = nux_mesh_new(ctx, NUX_ARRAY_SIZE(positions));
    NUX_CHECK(id, return NUX_NULL);
    nux_mesh_t *mesh = nux_object_get(ctx, NUX_OBJECT_MESH, id);

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
static nux_u32_t
buffer_index (cgltf_accessor *accessor, nux_u32_t i)
{
    cgltf_buffer_view *view  = accessor->buffer_view;
    nux_u8_t          *data  = (nux_u8_t *)view->buffer->data;
    nux_u32_t          index = -1;
    switch (accessor->component_type)
    {
        case cgltf_component_type_r_8:
        case cgltf_component_type_r_8u: {
            index = ((nux_u8_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        case cgltf_component_type_r_16:
        case cgltf_component_type_r_16u: {
            index = ((nux_u16_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        case cgltf_component_type_r_32f:
        case cgltf_component_type_r_32u: {
            index = ((nux_u32_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        default:
            break;
    }
    NUX_ASSERT(index != (nux_u32_t)-1);
    return index;
}
static nux_u32_t
compile_primitive_mesh (nux_ctx_t *ctx, const cgltf_primitive *primitive)
{
    // Access attributes
    const nux_v3_t *positions = NUX_NULL;
    const nux_v2_t *uvs       = NUX_NULL;
    for (nux_u32_t a = 0; a < primitive->attributes_count; ++a)
    {
        cgltf_attribute   *attribute = primitive->attributes + a;
        cgltf_accessor    *accessor  = attribute->data;
        cgltf_buffer_view *view      = accessor->buffer_view;
        nux_u8_t          *data      = (nux_u8_t *)view->buffer->data;
        switch (attribute->type)
        {
            case cgltf_attribute_type_position:
                positions
                    = (nux_v3_t *)(data + accessor->offset + view->offset);
                break;
            case cgltf_attribute_type_texcoord:
                uvs = (nux_v2_t *)(data + accessor->offset + view->offset);
                break;
            default:
                break;
        }
    }

    // Build vertex attributes
    nux_v3_t base_color = NUX_V3_ONES;
    NUX_UNUSED1(base_color);
    nux_vertex_attribute_t attributes = 0;
    if (positions)
    {
        attributes |= NUX_VERTEX_POSITION;
    }
    if (uvs)
    {
        attributes |= NUX_VERTEX_UV;
    }
    if (primitive->material && primitive->material->has_pbr_metallic_roughness)
    {
        if (!primitive->material->pbr_metallic_roughness.base_color_texture
                 .texture)
        {
            cgltf_float *color
                = primitive->material->pbr_metallic_roughness.base_color_factor;
            base_color = nux_v3(color[0], color[1], color[2]);
            attributes |= NUX_VERTEX_COLOR;
        }
    }

    cgltf_accessor *accessor     = primitive->indices;
    nux_u32_t       indice_count = accessor->count;

    // Create mesh object
    nux_u32_t id = nux_mesh_new(ctx, indice_count);
    NUX_CHECK(id, return NUX_NULL);
    nux_mesh_t *mesh = nux_object_get(ctx, NUX_OBJECT_MESH, id);

    // Write vertices
    if (attributes & NUX_VERTEX_POSITION)
    {
        for (nux_u32_t i = 0; i < indice_count; ++i)
        {
            nux_v3_t position     = positions[buffer_index(accessor, i)];
            mesh->data[5 * i + 0] = position.x;
            mesh->data[5 * i + 1] = position.y;
            mesh->data[5 * i + 2] = position.z;
        }
    }
    if (attributes & NUX_VERTEX_UV)
    {
        for (nux_u32_t i = 0; i < indice_count; ++i)
        {
            nux_v2_t uv           = uvs[buffer_index(accessor, i)];
            mesh->data[5 * i + 3] = uv.x;
            mesh->data[5 * i + 4] = uv.y;
        }
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        for (nux_u32_t i = 0; i < indice_count; ++i)
        {
        }
    }

    NUX_CHECK(
        nux_graphics_push_vertices(ctx, mesh->count, mesh->data, &mesh->first),
        return NUX_NULL);

    return id;
}
nux_u32_t
nux_mesh_load (nux_ctx_t *ctx, const nux_c8_t *url)
{
    NUX_INFO("Loading mesh from %s", url);
#ifdef NUX_BUILD_IMPORTER
    typedef struct
    {
        void     *cgltf_ptr;
        nux_u32_t id;
    } resource_t;

    cgltf_options options;
    cgltf_result  result;
    resource_t    resources[512];
    nux_u32_t     resources_count = 0;
    cgltf_data   *data            = NUX_NULL;
    nux_memset(&options, 0, sizeof(options));
    nux_memset(resources, 0, sizeof(resources));

    // Parse file and load buffers
    result = cgltf_parse_file(&options, url, &data);
    if (result != cgltf_result_success)
    {
        NUX_ERROR("Failed to load gltf file %s (code %d)", url, result);
        return NUX_NULL;
    }
    result = cgltf_load_buffers(&options, data, url);
    if (result != cgltf_result_success)
    {
        NUX_ERROR("Failed to load gltf buffers %s", url);
        return NUX_NULL;
    }

    // Load mesh primitives
    for (nux_u32_t i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh *mesh = data->meshes + i;
        for (nux_u32_t p = 0; p < mesh->primitives_count; ++p)
        {
            nux_u32_t id = compile_primitive_mesh(ctx, mesh->primitives + p);
            NUX_INFO("Loading mesh %u '%s' primitive %d", id, mesh->name, p);
            NUX_CHECK(id, goto cleanup0);
            resources[resources_count].cgltf_ptr = mesh->primitives + p;
            resources[resources_count].id        = id;
            ++resources_count;
            // return id;
            if (resources_count == 3)
            {
                return id;
            }
        }
    }

    // Load base color textures
    // for (nux_u32_t i = 0; i < data->materials_count; ++i)
    // {
    //     cgltf_material *material = data->materials + i;
    //     cgltf_texture  *texture  = NUX_NULL;
    //     if (material->has_pbr_metallic_roughness)
    //     {
    //         texture
    //             =
    //             material->pbr_metallic_roughness.base_color_texture.texture;
    //     }
    //     else if (material->has_diffuse_transmission)
    //     {
    //         texture = material->diffuse_transmission
    //                       .diffuse_transmission_texture.texture;
    //     }
    //     else if (material->emissive_texture.texture)
    //     {
    //         texture = material->emissive_texture.texture;
    //     }
    //     if (texture)
    //     {
    //         nux_u32_t id = sdk_next_id(proj);
    //         NUX_CHECK(compile_texture(texture, id, proj), goto cleanup0);
    //         resources[resources_count].id        = id;
    //         resources[resources_count].cgltf_ptr = texture;
    //         ++resources_count;
    //     }
    // }

    // // Load scenes and nodes
    // for (nux_u32_t s = 0; s < data->scenes_count; ++s)
    // {
    //     cgltf_scene *scene = data->scenes + s;
    //
    //     // Compute required node
    //     nux_u32_t node_count = 0;
    //     for (nux_u32_t n = 0; n < scene->nodes_count; ++n)
    //     {
    //         cgltf_node *node = scene->nodes[n];
    //         if (node->mesh)
    //         {
    //             node_count
    //                 += node->mesh->primitives_count * 2; // transform + model
    //         }
    //     }
    //
    //     // Create model
    //     for (nux_u32_t n = 0; n < scene->nodes_count; ++n)
    //     {
    //         cgltf_node *node = scene->nodes[n];
    //
    //         nux_v3_t translation = NUX_V3_ZEROES;
    //         nux_q4_t rotation    = nux_q4_identity();
    //         nux_v3_t scale       = NUX_V3_ONES;
    //         if (node->has_scale)
    //         {
    //             scale = nux_v3(node->scale[0], node->scale[1],
    //             node->scale[2]);
    //         }
    //         if (node->has_rotation)
    //         {
    //             rotation = nux_q4(node->rotation[0],
    //                               node->rotation[1],
    //                               node->rotation[2],
    //                               node->rotation[3]);
    //         }
    //         if (node->has_translation)
    //         {
    //             translation = nux_v3(node->translation[0],
    //                                  node->translation[1],
    //                                  node->translation[2]);
    //         }
    //
    //         if (node->mesh)
    //         {
    //             for (nux_u32_t p = 0; p < node->mesh->primitives_count; ++p)
    //             {
    //                 cgltf_primitive *primitive = node->mesh->primitives + p;
    //
    //                 // Find mesh
    //                 nux_u32_t mesh = NUX_NULL;
    //                 for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(resources); ++i)
    //                 {
    //                     if (resources[i].cgltf_ptr == primitive)
    //                     {
    //                         mesh = resources[i].id;
    //                         break;
    //                     }
    //                 }
    //                 if (!mesh)
    //                 {
    //                     NUX_ERROR("Mesh primitive not found for model %s",
    //                               node->name);
    //                     status = NUX_FAILURE;
    //                     goto cleanup0;
    //                 }
    //
    //                 // Find texture
    //                 texture = NU_NULL;
    //                 if (primitive->material
    //                     && primitive->material->has_pbr_metallic_roughness
    //                     && primitive->material->pbr_metallic_roughness
    //                            .base_color_texture.texture)
    //                 {
    //                     for (nu_size_t i = 0; i < MAX_RESOURCE; ++i)
    //                     {
    //                         if (resources[i].cgltf_ptr
    //                             ==
    //                             primitive->material->pbr_metallic_roughness
    //                                    .base_color_texture.texture)
    //                         {
    //                             texture = resources[i].id;
    //                             break;
    //                         }
    //                     }
    //                 }
    //                 if (!texture)
    //                 {
    //                     sdk_log(NU_LOG_WARNING,
    //                             "Texture not found for model %s, using
    //                             default", node->name);
    //                 }
    //
    //                 sdk_log(NU_LOG_DEBUG,
    //                         "Loading node %s mesh %d texture %d",
    //                         node->name,
    //                         mesh,
    //                         texture);
    //
    //                 // Write transform node
    //                 NU_CHECK(cart_write_u32(proj, NUX_OBJECT_NODE),
    //                          goto cleanup0);
    //                 NU_CHECK(cart_write_u32(proj, NU_NULL), goto cleanup0);
    //                 NU_CHECK(cart_write_v3(proj, translation), goto
    //                 cleanup0); NU_CHECK(cart_write_q4(proj, rotation), goto
    //                 cleanup0); NU_CHECK(cart_write_v3(proj, scale), goto
    //                 cleanup0);
    //
    //                 // Write node
    //                 NU_CHECK(cart_write_u32(proj, NUX_OBJECT_MODEL),
    //                          goto cleanup0);
    //                 NU_CHECK(cart_write_u32(proj, mesh), goto cleanup0);
    //                 NU_CHECK(cart_write_u32(proj, texture), goto cleanup0);
    //             }
    //         }
    //     }
    //
    //     break; // TODO: support multiple scene
    // }

cleanup0:
    cgltf_free(data);
    return NUX_NULL;
#endif
    return NUX_NULL;
}
