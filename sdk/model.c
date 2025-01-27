#include "sdk.h"

#include <native/core/vm.h>
#include <native/core/gpu.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

static nu_status_t
compile_mesh (const cgltf_mesh *mesh, nu_u32_t index, FILE *f)
{
    sdk_log(NU_LOG_INFO, "- Loading mesh '%s'", mesh->name);

    // Fill buffer
    for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
    {
        cgltf_primitive *primitive = mesh->primitives + p;

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
                    positions
                        = (nu_v3_t *)(data + accessor->offset + view->offset);
                    break;
                case cgltf_attribute_type_texcoord:
                    uvs = (nu_v2_t *)(data + accessor->offset + view->offset);
                    break;
                default:
                    break;
            }
        }

        // Build vertex attributes
        gpu_vertex_attribute_t attributes = 0;
        if (positions)
        {
            attributes |= GPU_VERTEX_POSTIION;
        }
        if (uvs)
        {
            attributes |= GPU_VERTEX_UV;
        }

        cgltf_accessor    *accessor     = primitive->indices;
        cgltf_buffer_view *view         = accessor->buffer_view;
        nu_byte_t         *data         = (nu_byte_t *)view->buffer->data;
        nu_size_t          indice_count = accessor->count;

        // Write header
        nu_size_t vertex_size = gpu_vertex_size(attributes);
        NU_CHECK(cart_write_chunk_header(f,
                                         CART_CHUNK_MESH,
                                         CART_CHUNK_MESH_HEADER_SIZE
                                             + vertex_size * indice_count
                                                   * sizeof(nu_f32_t)),
                 return NU_FAILURE);
        NU_CHECK(cart_write_u32(f, index), return NU_FAILURE);
        NU_CHECK(cart_write_u32(f, indice_count), return NU_FAILURE);
        NU_CHECK(cart_write_u32(f, GPU_PRIMITIVE_TRIANGLES), return NU_FAILURE);
        NU_CHECK(cart_write_u32(f, attributes), return NU_FAILURE);

        // Write vertices
        for (nu_size_t i = 0; i < indice_count; ++i)
        {
            nu_u32_t index = -1;
            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:
                case cgltf_component_type_r_8u: {
                    index = ((nu_u8_t *)(data + view->offset
                                         + accessor->offset))[i];
                }
                break;
                case cgltf_component_type_r_16:
                case cgltf_component_type_r_16u: {
                    index = ((nu_u16_t *)(data + view->offset
                                          + accessor->offset))[i];
                }
                break;
                case cgltf_component_type_r_32f:
                case cgltf_component_type_r_32u: {
                    index = ((nu_u32_t *)(data + view->offset
                                          + accessor->offset))[i];
                }
                break;
                default:
                    break;
            }
            NU_ASSERT(index != (nu_u32_t)-1);

            if (attributes & GPU_VERTEX_POSTIION)
            {
                cart_write_v3(f, positions[index]);
            }
            if (attributes & GPU_VERTEX_UV)
            {
                cart_write_v2(f, uvs[index]);
            }
        }
    }

    return NU_SUCCESS;
}

nu_status_t
sdk_model_load (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_parse_u32(jasset, "index", &asset->model.target_index),
             return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
sdk_model_save (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_write_u32(jasset, "index", asset->model.target_index),
             return NU_FAILURE);
    return NU_SUCCESS;
}

nu_status_t
sdk_model_compile (sdk_project_asset_t *asset, FILE *f)
{
    typedef struct
    {
        void    *cgltf_ptr;
        nu_u32_t index;
    } resource_t;

#define MAX_RESOURCE 128
    cgltf_options options;
    cgltf_result  result;
    resource_t    resources[MAX_RESOURCE];
    nu_size_t     resources_count = 0;
    cgltf_data   *data            = NU_NULL;
    nu_status_t   status          = NU_SUCCESS;
    nu_memset(&options, 0, sizeof(options));
    nu_memset(resources, 0, sizeof(resources));

    // Parse file and load buffers
    result = cgltf_parse_file(&options, asset->source_path, &data);
    if (result != cgltf_result_success)
    {
        sdk_log(
            NU_LOG_ERROR, "Failed to load gltf file %s", asset->source_path);
        return NU_FAILURE;
    }
    result = cgltf_load_buffers(&options, data, asset->source_path);
    if (result != cgltf_result_success)
    {
        sdk_log(
            NU_LOG_ERROR, "Failed to load gltf buffers %s", asset->source_path);
        return NU_FAILURE;
    }

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        NU_CHECK(compile_mesh(data->meshes + i, i, f), goto cleanup0);
        resources[resources_count].index     = i;
        resources[resources_count].cgltf_ptr = data->meshes + i;
        ++resources_count;
    }

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Create model
        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            sdk_log(NU_LOG_INFO, "- Loading node %s", node->name);

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
                // Find mesh
                nu_u32_t mesh = -1;
                for (nu_size_t i = 0; i < MAX_RESOURCE; ++i)
                {
                    if (resources[i].cgltf_ptr == node->mesh)
                    {
                        mesh = resources[i].index;
                        break;
                    }
                }
                if (mesh == (nu_u32_t)-1)
                {
                    sdk_log(NU_LOG_ERROR,
                            "Mesh not found for model %s",
                            node->name);
                    status = NU_FAILURE;
                    goto cleanup0;
                }

                // Write model chunk
                nu_u32_t parent = n ? asset->model.target_index : (nu_u32_t)-1;
                NU_CHECK(cart_write_chunk_header(
                             f, CART_CHUNK_MODEL, CART_CHUNK_MODEL_HEADER_SIZE),
                         goto cleanup0);
                NU_CHECK(cart_write_u32(f, asset->model.target_index + n),
                         goto cleanup0);
                NU_CHECK(cart_write_u32(f, mesh), goto cleanup0);
                NU_CHECK(cart_write_u32(f, -1), goto cleanup0);
                NU_CHECK(cart_write_u32(f, parent), goto cleanup0);
                NU_CHECK(cart_write_m4(f, transform), goto cleanup0);
            }
        }

        break; // TODO: support multiple scene
    }

cleanup0:
    cgltf_free(data);
    return status;
}
