#include "internal.h"

#ifdef NUX_BUILD_IMPORTER
#define CGLTF_IMPLEMENTATION
#define JSMN_HEADER
#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#include <externals/jsmn/jsmn.h>
#include <externals/cgltf/cgltf.h>
#define STBIR_DEBUG
#define STB_IMAGE_STATIC
#define STBI_NO_STDIO
#define STBI_ASSERT(x) NUX_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include <externals/stb/stb_image.h>
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
// #include <stb_image_resize.h>
// #include <stb_image_resize2.h>
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>
#endif

typedef struct
{
    void     *cgltf_ptr;
    nux_rid_t rid;
} resource_t;

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
static nux_mesh_t *
load_primitive_mesh (nux_arena_t *arena, const cgltf_primitive *primitive)
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
    nux_mesh_t *mesh = nux_mesh_new(arena, indice_count);
    NUX_CHECK(mesh, return NUX_NULL);

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

    NUX_CHECK(nux_graphics_push_vertices(mesh->count, mesh->data, &mesh->first),
              return NUX_NULL);

    // Optional : Generate bounds
    nux_mesh_update_bounds(mesh);

    return mesh;
}
static nux_texture_t *
load_texture (nux_arena_t *arena, const cgltf_texture *texture)
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
    NUX_ENSURE(img, return NUX_NULL, "failed to load image %s", texture->name);

    NUX_DEBUG("loading texture %s w %d h %d", texture->name, w, h);

    nux_texture_t *tex = nux_texture_new(arena, NUX_TEXTURE_IMAGE_RGBA, w, h);
    NUX_CHECK(tex, return NUX_NULL);
    nux_texture_write(tex, 0, 0, w, h, img);

    stbi_image_free(img);
    return tex;
}
static nux_status_t
load_node (nux_arena_t *arena,
           resource_t  *resources,
           nux_u32_t    resources_count,
           cgltf_scene *scene,
           cgltf_node  *node,
           nux_nid_t    parent)
{
    nux_nid_t e = nux_node_create(parent);
    NUX_CHECK(e, return NUX_FAILURE);

    nux_v3_t translation = NUX_V3_ZEROS;
    nux_q4_t rotation    = nux_q4_identity();
    nux_v3_t scale       = NUX_V3_ONES;
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
        translation = nux_v3(
            node->translation[0], node->translation[1], node->translation[2]);
    }

    // Set transform
    nux_transform_add(e);
    nux_transform_set_translation(e, translation);
    nux_transform_set_rotation(e, rotation);
    nux_transform_set_scale(e, scale);

    if (node->mesh)
    {
        for (nux_u32_t p = 0; p < node->mesh->primitives_count; ++p)
        {
            cgltf_primitive *primitive = node->mesh->primitives + p;

            // Find mesh
            nux_rid_t mesh = NUX_NULL;
            for (nux_u32_t i = 0; i < resources_count; ++i)
            {
                if (resources[i].cgltf_ptr == primitive)
                {
                    mesh = resources[i].rid;
                    break;
                }
            }
            NUX_ENSURE(mesh,
                       return NUX_FAILURE,
                       "mesh primitive not found for model %s",
                       node->name);

            // Find texture
            nux_rid_t texture = NUX_NULL;
            if (primitive->material
                && primitive->material->has_pbr_metallic_roughness
                && primitive->material->pbr_metallic_roughness
                       .base_color_texture.texture)
            {
                for (nux_u32_t i = 0; i < resources_count; ++i)
                {
                    if (resources[i].cgltf_ptr
                        == primitive->material->pbr_metallic_roughness
                               .base_color_texture.texture)
                    {
                        texture = resources[i].rid;
                        break;
                    }
                }
            }
            if (!texture)
            {
                NUX_WARNING("texture not found for model %s, using default",
                            node->name);
            }

            NUX_DEBUG("loading node %s mesh 0x%08X texture 0x%08X",
                      node->name,
                      mesh,
                      texture);

            // Write staticmesh
            nux_staticmesh_add(e);
            nux_staticmesh_set_mesh(e,
                                    nux_resource_get(NUX_RESOURCE_MESH, mesh));
            nux_staticmesh_set_texture(
                e, nux_resource_get(NUX_RESOURCE_TEXTURE, texture));
        }
    }
    for (nux_u32_t i = 0; i < node->children_count; ++i)
    {
        cgltf_node *child = node->children[i];
        load_node(arena, resources, resources_count, scene, child, e);
    }
    return NUX_SUCCESS;
}
nux_scene_t *
nux_scene_load_gltf (nux_arena_t *arena, const nux_c8_t *path)
{

    cgltf_options options;
    cgltf_result  result;
    resource_t    resources[512];
    nux_u32_t     resources_count = 0;
    cgltf_data   *data            = NUX_NULL;

    nux_memset(&options, 0, sizeof(options));
    nux_memset(resources, 0, sizeof(resources));

    nux_scene_t *scene = NUX_NULL;

    // Load file
    nux_u32_t buf_size;
    void     *buf = nux_file_load(nux_arena_frame(), path, &buf_size);
    NUX_CHECK(buf, goto error);

    // Parse file
    result = cgltf_parse(&options, buf, buf_size, &data);
    NUX_ENSURE(result == cgltf_result_success,
               goto error,
               "failed to parse gltf file %s (code %d)",
               path,
               result);

    // Load buffers
    result = cgltf_load_buffers(&options, data, path);
    NUX_ENSURE(result == cgltf_result_success,
               goto error,
               "failed to load gltf buffers %s",
               path);

    // Load mesh primitives
    for (nux_u32_t i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh *mesh = data->meshes + i;
        for (nux_u32_t p = 0; p < mesh->primitives_count; ++p)
        {
            nux_rid_t rid = nux_resource_rid(
                load_primitive_mesh(arena, mesh->primitives + p));
            NUX_CHECK(rid, goto error);
            NUX_DEBUG(
                "loading mesh %s primitive %d rid 0x%08X", mesh->name, p, rid);
            resources[resources_count].cgltf_ptr = mesh->primitives + p;
            resources[resources_count].rid       = rid;
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
            nux_rid_t rid = nux_resource_rid(load_texture(arena, texture));
            NUX_CHECK(rid, goto error);
            resources[resources_count].cgltf_ptr = texture;
            resources[resources_count].rid       = rid;
            ++resources_count;
        }
    }

    // Load scenes and nodes
    for (nux_u32_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *gltf_scene = data->scenes + s;

        // Create scene
        scene = nux_scene_new(arena);
        NUX_CHECK(scene, goto error);
        nux_scene_set_active(scene);

        for (nux_u32_t c = 0; c < gltf_scene->nodes_count; ++c)
        {
            cgltf_node *node = gltf_scene->nodes[c];
            if (!node->parent) // root only nodes
            {
                NUX_CHECK(load_node(arena,
                                    resources,
                                    NUX_ARRAY_SIZE(resources),
                                    gltf_scene,
                                    node,
                                    nux_node_root()),
                          goto error);
            }
        }

        // TODO: support multiple scene
        break;
    }

    cgltf_free(data);
    nux_scene_set_active(NUX_NULL);
    return scene;
error:
    cgltf_free(data);
    nux_scene_set_active(NUX_NULL);
    return NUX_NULL;
}
