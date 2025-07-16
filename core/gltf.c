#include "internal.h"

#ifdef NUX_BUILD_IMPORTER
#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
#define STBIR_DEBUG
#define STB_IMAGE_STATIC
#define STBI_NO_STDIO
#define STBI_ASSERT(x) NUX_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
// #include <stb_image_resize.h>
// #include <stb_image_resize2.h>
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>
#endif

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
load_primitive_mesh (nux_ctx_t *ctx, const cgltf_primitive *primitive)
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

    // Create mesh
    nux_u32_t id = nux_mesh_new(ctx, indice_count);
    NUX_CHECK(id, return NUX_NULL);
    nux_mesh_t *mesh = nux_id_get(ctx, NUX_TYPE_MESH, id);

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

    // Optional : Generate bounds
    nux_mesh_gen_bounds(ctx, id);

    return id;
}
static nux_u32_t
load_texture (nux_ctx_t *ctx, const cgltf_texture *texture)
{
    nux_status_t status = NUX_FAILURE;

    // Load image buffer
    cgltf_buffer_view *view = texture->image->buffer_view;
    nux_i32_t          w, h, n;
    nux_u8_t *img = stbi_load_from_memory(view->buffer->data + view->offset,
                                          view->size,
                                          &w,
                                          &h,
                                          &n,
                                          STBI_rgb_alpha);
    if (!img)
    {
        NUX_ERROR("Failed to load image '%s'", texture->name);
        return NUX_NULL;
    }

    NUX_DEBUG("Loading texture '%s' w %d h %d", texture->name, w, h);

    nux_u32_t id = nux_texture_new(ctx, NUX_TEXTURE_IMAGE_RGBA, w, h);
    NUX_CHECK(id, return NUX_NULL);
    nux_texture_write(ctx, id, 0, 0, w, h, img);

    stbi_image_free(img);
    return id;
}
nux_u32_t
nux_scene_load_gltf (nux_ctx_t *ctx, const nux_c8_t *path)
{
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

    nux_u32_t scene_id = nux_scene_new(ctx);
    NUX_CHECK(scene_id, goto cleanup0);

    // Load file
    nux_u32_t buf_size;
    nux_u32_t prev_arena = nux_arena_get_active(ctx);
    nux_arena_set_active(ctx, nux_arena_frame(ctx));
    void *buf = nux_io_load_file(ctx, path, &buf_size);
    nux_arena_set_active(ctx, prev_arena);
    NUX_CHECKM(buf, "Failed to read gltf file", goto cleanup0);

    // Parse file
    result = cgltf_parse(&options, buf, buf_size, &data);
    if (result != cgltf_result_success)
    {
        NUX_ERROR("Failed to parse gltf file %s (code %d)", path, result);
        goto cleanup0;
    }

    // Load buffers
    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success)
    {
        NUX_ERROR("Failed to load gltf buffers %s", path);
        goto cleanup0;
    }

    // Load mesh primitives
    for (nux_u32_t i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh *mesh = data->meshes + i;
        for (nux_u32_t p = 0; p < mesh->primitives_count; ++p)
        {
            nux_u32_t id = load_primitive_mesh(ctx, mesh->primitives + p);
            NUX_DEBUG("Loading mesh %u '%s' primitive %d", id, mesh->name, p);
            NUX_CHECK(id, goto cleanup0);
            resources[resources_count].cgltf_ptr = mesh->primitives + p;
            resources[resources_count].id        = id;
            ++resources_count;
        }
    }

    // Load base color textures
    for (nux_u32_t i = 0; i < data->materials_count; ++i)
    {
        cgltf_material *material = data->materials + i;
        cgltf_texture  *texture  = NUX_NULL;
        if (material->has_pbr_metallic_roughness)
        {
            texture
                = material->pbr_metallic_roughness.base_color_texture.texture;
        }
        else if (material->has_diffuse_transmission)
        {
            texture = material->diffuse_transmission
                          .diffuse_transmission_texture.texture;
        }
        else if (material->emissive_texture.texture)
        {
            texture = material->emissive_texture.texture;
        }
        if (texture)
        {
            nux_u32_t id = load_texture(ctx, texture);
            NUX_DEBUG("Loading texture %u '%s'", id, texture->name);
            NUX_CHECK(id, goto cleanup0);
            resources[resources_count].cgltf_ptr = texture;
            resources[resources_count].id        = id;
            ++resources_count;
        }
    }

    // Load scenes and nodes
    for (nux_u32_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Compute required node
        nux_u32_t node_count = 0;
        NUX_UNUSED1(node_count);
        for (nux_u32_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            if (node->mesh)
            {
                node_count += node->mesh->primitives_count;
            }
        }

        // Create nodes
        for (nux_u32_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];

            nux_u32_t node_id = nux_node_new(ctx, scene_id);
            NUX_CHECK(node_id, goto cleanup0);

            nux_v3_t translation = NUX_V3_ZEROES;
            nux_q4_t rotation    = nux_q4_identity();
            NUX_UNUSED1(rotation);
            nux_v3_t scale = NUX_V3_ONES;
            if (node->has_scale)
            {
                scale = nux_v3(node->scale[0], node->scale[1], node->scale[2]);
            }
            if (node->has_rotation)
            {
                rotation = nux_q4(node->rotation[0],
                                  node->rotation[1],
                                  node->rotation[2],
                                  node->rotation[3]);
            }
            if (node->has_translation)
            {
                translation = nux_v3(node->translation[0],
                                     node->translation[1],
                                     node->translation[2]);
            }

            // Set transform
            nux_transform_add(ctx, node_id);
            nux_transform_set_translation(ctx, node_id, translation);
            nux_transform_set_rotation(ctx, node_id, rotation);
            nux_transform_set_scale(ctx, node_id, scale);

            if (node->mesh)
            {
                for (nux_u32_t p = 0; p < node->mesh->primitives_count; ++p)
                {
                    cgltf_primitive *primitive = node->mesh->primitives + p;

                    // Find mesh
                    nux_u32_t mesh = NUX_NULL;
                    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(resources); ++i)
                    {
                        if (resources[i].cgltf_ptr == primitive)
                        {
                            mesh = resources[i].id;
                            break;
                        }
                    }
                    if (!mesh)
                    {
                        NUX_ERROR("Mesh primitive not found for model %s",
                                  node->name);
                        goto cleanup0;
                    }

                    // Find texture
                    nux_u32_t texture = NUX_NULL;
                    if (primitive->material
                        && primitive->material->has_pbr_metallic_roughness
                        && primitive->material->pbr_metallic_roughness
                               .base_color_texture.texture)
                    {
                        for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(resources);
                             ++i)
                        {
                            if (resources[i].cgltf_ptr
                                == primitive->material->pbr_metallic_roughness
                                       .base_color_texture.texture)
                            {
                                texture = resources[i].id;
                                break;
                            }
                        }
                    }
                    if (!texture)
                    {
                        NUX_WARNING(
                            "Texture not found for model %s, using default",
                            node->name);
                    }

                    NUX_DEBUG("Loading node %s mesh %d texture %d",
                              node->name,
                              mesh,
                              texture);

                    // Write staticmesh
                    nux_staticmesh_add(ctx, node_id);
                    nux_staticmesh_set_mesh(ctx, node_id, mesh);
                    nux_staticmesh_set_texture(ctx, node_id, texture);
                }
            }
        }

        break; // TODO: support multiple scene
    }

cleanup0:
    cgltf_free(data);
    return scene_id;
}
