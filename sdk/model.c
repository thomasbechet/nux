#include "sdk.h"

#include <runtime.h>
#include <core/vm.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>
#include <stb/stb_image.h>

static nu_status_t
compile_texture (const cgltf_texture *texture, nu_u32_t id, sdk_project_t *proj)
{
    nu_status_t status = NU_FAILURE;

    // Load image buffer
    cgltf_buffer_view *view = texture->image->buffer_view;
    nu_i32_t           w, h, n;
    nu_byte_t *img = stbi_load_from_memory(view->buffer->data + view->offset,
                                           view->size,
                                           &w,
                                           &h,
                                           &n,
                                           STBI_rgb_alpha);
    if (!img)
    {
        logger_log(NU_LOG_ERROR, "Failed to load image '%s'", texture->name);
        return NU_FAILURE;
    }

    logger_log(
        NU_LOG_DEBUG, "Loading texture '%s' w %d h %d", texture->name, w, h);

    // Find nearest texture size
    nu_u32_t target_size = nu_upper_power_of_two(NU_MAX(w, h));
    if (target_size > SYS_MAX_TEXTURE_SIZE)
    {
        target_size = SYS_MAX_TEXTURE_SIZE;
        logger_log(
            NU_LOG_WARNING, "Texture resized to %d", SYS_MAX_TEXTURE_SIZE);
    }

    // Resize image
    nu_byte_t *resized = native_malloc(gfx_texture_memsize(target_size));
    NU_ASSERT(resized);
    if (!image_resize(nu_v2u(w, h), img, target_size, resized))
    {
        logger_log(
            NU_LOG_ERROR, "Failed to resized glb image '%s'", texture->name);
        goto cleanup0;
    }

    // Write image
    status = cart_write_texture(proj, id, target_size, resized);
    NU_CHECK(status, goto cleanup0);

cleanup0:
    stbi_image_free(img);
    return NU_SUCCESS;
}
static nu_u32_t
buffer_index (cgltf_accessor *accessor, nu_size_t i)
{
    cgltf_buffer_view *view  = accessor->buffer_view;
    nu_byte_t         *data  = (nu_byte_t *)view->buffer->data;
    nu_u32_t           index = -1;
    switch (accessor->component_type)
    {
        case cgltf_component_type_r_8:
        case cgltf_component_type_r_8u: {
            index = ((nu_u8_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        case cgltf_component_type_r_16:
        case cgltf_component_type_r_16u: {
            index = ((nu_u16_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        case cgltf_component_type_r_32f:
        case cgltf_component_type_r_32u: {
            index = ((nu_u32_t *)(data + view->offset + accessor->offset))[i];
        }
        break;
        default:
            break;
    }
    NU_ASSERT(index != (nu_u32_t)-1);
    return index;
}
static nu_status_t
compile_primitive_mesh (const cgltf_primitive *primitive,
                        nu_u32_t               id,
                        sdk_project_t         *proj)
{
    // Access attributes
    const nu_v3_t *positions = NU_NULL;
    const nu_v2_t *uvs       = NU_NULL;
    for (nu_size_t a = 0; a < primitive->attributes_count; ++a)
    {
        cgltf_attribute   *attribute = primitive->attributes + a;
        cgltf_accessor    *accessor  = attribute->data;
        cgltf_buffer_view *view      = accessor->buffer_view;
        nu_byte_t         *data      = (nu_byte_t *)view->buffer->data;
        switch (attribute->type)
        {
            case cgltf_attribute_type_position:
                positions = (nu_v3_t *)(data + accessor->offset + view->offset);
                break;
            case cgltf_attribute_type_texcoord:
                uvs = (nu_v2_t *)(data + accessor->offset + view->offset);
                break;
            default:
                break;
        }
    }

    // Build vertex attributes
    nu_v3_t                base_color = NU_V3_ONES;
    sys_vertex_attribute_t attributes = 0;
    if (positions)
    {
        attributes |= SYS_VERTEX_POSITION;
    }
    if (uvs)
    {
        attributes |= SYS_VERTEX_UV;
    }
    if (primitive->material && primitive->material->has_pbr_metallic_roughness)
    {
        if (!primitive->material->pbr_metallic_roughness.base_color_texture
                 .texture)
        {
            cgltf_float *color
                = primitive->material->pbr_metallic_roughness.base_color_factor;
            base_color = nu_v3(color[0], color[1], color[2]);
            attributes |= SYS_VERTEX_COLOR;
        }
    }

    cgltf_accessor *accessor     = primitive->indices;
    nu_size_t       indice_count = accessor->count;

    // Write header
    cart_chunk_entry_t *entry = sdk_begin_entry(proj, id, RESOURCE_MESH);
    NU_CHECK(cart_write_u32(proj, indice_count), return NU_FAILURE);
    NU_CHECK(cart_write_u32(proj, SYS_PRIMITIVE_TRIANGLES), return NU_FAILURE);
    NU_CHECK(cart_write_u32(proj, attributes), return NU_FAILURE);

    // Write vertices
    if (attributes & SYS_VERTEX_POSITION)
    {
        for (nu_size_t i = 0; i < indice_count; ++i)
        {
            cart_write_v3(proj, positions[buffer_index(accessor, i)]);
        }
    }
    if (attributes & SYS_VERTEX_UV)
    {
        for (nu_size_t i = 0; i < indice_count; ++i)
        {
            cart_write_v2(proj, uvs[buffer_index(accessor, i)]);
        }
    }
    if (attributes & SYS_VERTEX_COLOR)
    {
        for (nu_size_t i = 0; i < indice_count; ++i)
        {
            cart_write_v3(proj, base_color);
        }
    }

    return NU_SUCCESS;
}

nu_status_t
sdk_model_load (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    return NU_SUCCESS;
}
nu_status_t
sdk_model_save (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    return NU_SUCCESS;
}

nu_status_t
sdk_model_compile (sdk_project_t *proj, sdk_project_asset_t *asset)
{
    typedef struct
    {
        void    *cgltf_ptr;
        nu_u32_t id;
    } resource_t;

#define MAX_RESOURCE 512
    cgltf_options options;
    cgltf_result  result;
    resource_t    resources[MAX_RESOURCE];
    nu_size_t     resources_count = 0;
    cgltf_data   *data            = NU_NULL;
    nu_status_t   status          = NU_SUCCESS;
    nu_memset(&options, 0, sizeof(options));
    nu_memset(resources, 0, sizeof(resources));

    // Parse file and load buffers
    result = cgltf_parse_file(&options, asset->source, &data);
    if (result != cgltf_result_success)
    {
        logger_log(NU_LOG_ERROR,
                   "Failed to load gltf file %s (code %d)",
                   asset->source,
                   result);
        return NU_FAILURE;
    }
    result = cgltf_load_buffers(&options, data, asset->source);
    if (result != cgltf_result_success)
    {
        logger_log(
            NU_LOG_ERROR, "Failed to load gltf buffers %s", asset->source);
        return NU_FAILURE;
    }

    // Load mesh primitives
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh *mesh = data->meshes + i;
        for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
        {
            nu_u32_t id = sdk_next_id(proj);
            NU_CHECK(compile_primitive_mesh(mesh->primitives + p, id, proj),
                     goto cleanup0);
            logger_log(NU_LOG_DEBUG,
                       "Loading mesh %u '%s' primitive %d",
                       id,
                       mesh->name,
                       p);
            resources[resources_count].id        = id;
            resources[resources_count].cgltf_ptr = mesh->primitives + p;
            ++resources_count;
        }
    }

    // Load base color textures
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        cgltf_material *material = data->materials + i;
        cgltf_texture  *texture  = NU_NULL;
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
            nu_u32_t id = sdk_next_id(proj);
            NU_CHECK(compile_texture(texture, id, proj), goto cleanup0);
            resources[resources_count].id        = id;
            resources[resources_count].cgltf_ptr = texture;
            ++resources_count;
        }
    }

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Compute required node
        nu_u32_t node_count = 0;
        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            if (node->mesh)
            {
                node_count += node->mesh->primitives_count;
            }
        }

        // Write model
        cart_chunk_entry_t *entry
            = sdk_begin_entry(proj, asset->id, RESOURCE_MODEL);
        NU_CHECK(cart_write_u32(proj, node_count), goto cleanup0);

        // Create model
        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];

            nu_m4_t transform = nu_m4_identity();
            if (node->has_scale)
            {
                transform = nu_m4_mul(
                    nu_m4_scale(
                        nu_v3(node->scale[0], node->scale[1], node->scale[2])),
                    transform);
            }
            if (node->has_rotation)
            {
                nu_q4_t q = nu_q4(node->rotation[0],
                                  node->rotation[1],
                                  node->rotation[2],
                                  node->rotation[3]);
                transform = nu_q4_mulm4(q, transform);
            }
            if (node->has_translation)
            {
                transform
                    = nu_m4_mul(nu_m4_translate(nu_v3(node->translation[0],
                                                      node->translation[1],
                                                      node->translation[2])),
                                transform);
            }

            if (node->mesh)
            {
                for (nu_size_t p = 0; p < node->mesh->primitives_count; ++p)
                {
                    cgltf_primitive *primitive = node->mesh->primitives + p;

                    // Find mesh
                    nu_u32_t mesh = 0;
                    for (nu_size_t i = 0; i < MAX_RESOURCE; ++i)
                    {
                        if (resources[i].cgltf_ptr == primitive)
                        {
                            mesh = resources[i].id;
                            break;
                        }
                    }
                    if (!mesh)
                    {
                        logger_log(NU_LOG_ERROR,
                                   "Mesh primitive not found for model %s",
                                   node->name);
                        status = NU_FAILURE;
                        goto cleanup0;
                    }

                    // Find texture
                    nu_u32_t texture = 0;
                    if (primitive->material
                        && primitive->material->has_pbr_metallic_roughness
                        && primitive->material->pbr_metallic_roughness
                               .base_color_texture.texture)
                    {
                        for (nu_size_t i = 0; i < MAX_RESOURCE; ++i)
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
                        logger_log(
                            NU_LOG_WARNING,
                            "Texture not found for model %s, using default",
                            node->name);
                    }

                    logger_log(NU_LOG_DEBUG,
                               "Loading node %s mesh %d texture %d",
                               node->name,
                               mesh,
                               texture);

                    // Write model node
                    NU_CHECK(cart_write_u32(proj, mesh), goto cleanup0);
                    NU_CHECK(cart_write_u32(proj, texture), goto cleanup0);
                    NU_CHECK(cart_write_u32(proj, -1), goto cleanup0);
                    NU_CHECK(cart_write_m4(proj, transform), goto cleanup0);
                }
            }
        }

        break; // TODO: support multiple scene
    }

cleanup0:
    cgltf_free(data);
    return status;
}
