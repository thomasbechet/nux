#include "sdk.h"

#include <native/vm.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

// static void
// emplace_index (nu_f32_t      *vertices,
//                const nu_v3_t *positions,
//                const nu_v2_t *uvs,
//                nu_u16_t       index)
// {
//     const nu_v3_t *pos                       = positions + index;
//     const nu_v2_t *uv                        = uvs ? uvs + index : NU_NULL;
//     vertices[VM_VERTEX_SIZE_F32 * index + 0] = pos->x;
//     vertices[VM_VERTEX_SIZE_F32 * index + 1] = pos->y;
//     vertices[VM_VERTEX_SIZE_F32 * index + 2] = pos->z;
//     vertices[VM_VERTEX_SIZE_F32 * index + VM_VERTEX_UV_OFFSET + 0] = uv->x;
//     vertices[VM_VERTEX_SIZE_F32 * index + VM_VERTEX_UV_OFFSET + 1] = uv->y;
// }
#define EMPLACE_VERTEX(index_type)                                \
    index_type *indices                                           \
        = (index_type *)(data + view->offset + accessor->offset); \
    for (nu_size_t i = 0; i < indice_count; ++i)                  \
    {                                                             \
        emplace_index(vertices, positions, uvs, indices[i]);      \
    }
// static nu_f32_t *
// load_mesh (const cgltf_mesh *mesh, nu_size_t *size)
// {
//     printf("loading mesh: %s", mesh->name);
//
//     // compute vertex count
//     *size = 0;
//     for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
//     {
//         cgltf_primitive *primitive = mesh->primitives + p;
//         cgltf_accessor  *accessor  = primitive->indices;
//         *size += accessor->count;
//     }
//     nu_f32_t *vertices = malloc(VM_VERTEX_SIZE * (*size));
//     NU_ASSERT(vertices);
//
//     // fill buffer
//     for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
//     {
//         cgltf_primitive *primitive = mesh->primitives + p;
//
//         // Access attributes
//         const nu_v3_t *positions = NU_NULL;
//         const nu_v2_t *uvs       = NU_NULL;
//         for (nu_size_t a = 0; a < primitive->attributes_count; ++a)
//         {
//             cgltf_attribute   *attribute = primitive->attributes + a;
//             cgltf_accessor    *accessor  = attribute->data;
//             cgltf_buffer_view *view      = accessor->buffer_view;
//             nu_byte_t         *data      = (nu_byte_t *)view->buffer->data;
//             switch (attribute->type)
//             {
//                 case cgltf_attribute_type_position:
//                     positions
//                         = (nu_v3_t *)(data + accessor->offset +
//                         view->offset);
//                     break;
//                 case cgltf_attribute_type_texcoord:
//                     uvs = (nu_v2_t *)(data + accessor->offset +
//                     view->offset); break;
//                 default:
//                     break;
//             }
//         }
//
//         // Build vertex buffer
//         {
//             cgltf_accessor    *accessor     = primitive->indices;
//             cgltf_buffer_view *view         = accessor->buffer_view;
//             nu_byte_t         *data         = (nu_byte_t
//             *)view->buffer->data; nu_size_t          indice_count =
//             accessor->count;
//
//             nu_f32_t *vertices = malloc(VM_VERTEX_SIZE * indice_count);
//             *size              = indice_count;
//             NU_ASSERT(vertices);
//
//             // Push vertices
//             switch (accessor->component_type)
//             {
//                 case cgltf_component_type_r_8:
//                 case cgltf_component_type_r_8u: {
//                     EMPLACE_VERTEX(nu_u8_t)
//                 }
//                 break;
//                 case cgltf_component_type_r_16:
//                 case cgltf_component_type_r_16u: {
//                     EMPLACE_VERTEX(nu_u16_t)
//                 }
//                 break;
//                 case cgltf_component_type_r_32f:
//                 case cgltf_component_type_r_32u: {
//                     EMPLACE_VERTEX(nu_u32_t)
//                 }
//                 break;
//                 default:
//                     break;
//             }
//         }
//     }
//
//     return vertices;
// }
nu_f32_t *
project_load_gltf (const nu_byte_t *path, nu_size_t *size)
{
    (void)size;
    cgltf_options options;
    nu_memset(&options, 0, sizeof(options));
    cgltf_data  *data = NU_NULL;
    cgltf_result result;

    // Parse file and load buffers
    result = cgltf_parse_file(&options, (char *)path, &data);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }
    result = cgltf_load_buffers(&options, data, (char *)path);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        // error = nu__load_mesh(loader, data->meshes + i);
    }

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Create model
        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            sdk_log(NU_LOG_INFO, "Loading node %s", node->name);

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

            // if (node->mesh)
            // {
            //     // Find mesh
            //     {
            //         for (nu_size_t i = 0; i < loader->resources.size; ++i)
            //         {
            //             if (loader->resources.data[i].ptr == node->mesh)
            //             {
            //                 mesh = loader->resources.data[i].handle;
            //                 break;
            //             }
            //         }
            //         if (!mesh)
            //         {
            //             return NU_NULL;
            //         }
            //     }
            //
            //     // Set node
            //     nu_model_set(model, n, mesh, material, transform);
            // }
        }

        break; // TODO: support multiple scene
    }

    cgltf_free(data);
    return NU_NULL;
}
