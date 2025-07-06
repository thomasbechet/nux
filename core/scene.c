#include "internal.h"

#define DEFAULT_NODE_COUNT 4096

nux_u32_t
nux_scene_new (nux_ctx_t *ctx)
{
    nux_scene_t *s = nux_arena_alloc(ctx->active_arena, sizeof(*s));
    NUX_CHECK(s, return NUX_NULL);
    nux_u32_t id = nux_object_create(ctx, ctx->active_arena, NUX_TYPE_SCENE, s);
    NUX_CHECK(id, return NUX_NULL);

    s->arena = ctx->active_arena;
    NUX_CHECK(
        nux_node_pool_alloc(ctx->active_arena, DEFAULT_NODE_COUNT, &s->nodes),
        return NUX_NULL);
    NUX_CHECK(nux_component_pool_alloc(
                  ctx->active_arena, DEFAULT_NODE_COUNT, &s->components),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_node_pool_add(&s->nodes);
    nux_component_pool_add(&s->components);

    return id;
}
void
nux_scene_draw (nux_ctx_t *ctx, nux_u32_t scene, nux_u32_t camera)
{
    nux_scene_t *s = nux_object_get(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return);

    // Propagate transforms
    for (nux_u32_t ni = 0; ni < s->nodes.size; ++ni)
    {
        nux_node_t *n = s->nodes.data + ni;
        if (!n->id)
        {
            continue;
        }
        if (n->components[NUX_COMPONENT_TRANSFORM])
        {
            nux_transform_t *t
                = &s->components.data[n->components[NUX_COMPONENT_TRANSFORM]]
                       .transform;
            nux_transform_update_matrix(ctx, n->id);
        }
    }

    // Draw nodes
    for (nux_u32_t ni = 0; ni < s->nodes.size; ++ni)
    {
        nux_node_t *n = s->nodes.data + ni;
        if (!n->id)
        {
            continue;
        }
        if (n->components[NUX_COMPONENT_TRANSFORM]
            && n->components[NUX_COMPONENT_STATICMESH])
        {
            nux_staticmesh_t *sm
                = &s->components.data[n->components[NUX_COMPONENT_STATICMESH]]
                       .staticmesh;
            if (!sm->mesh)
            {
                continue;
            }
            nux_transform_t *t
                = &s->components.data[n->components[NUX_COMPONENT_TRANSFORM]]
                       .transform;
            nux_mesh_t *m = nux_object_get(ctx, NUX_TYPE_MESH, sm->mesh);
            NUX_ASSERT(m);
            nux_texture_t *tex = NUX_NULL;
            if (sm->texture)
            {
                tex = nux_object_get(ctx, NUX_TYPE_TEXTURE, sm->texture);
            }

            // Push transform
            nux_u32_t transform_idx;
            NUX_CHECK(nux_graphics_push_transforms(
                          ctx, 1, &t->global_matrix, &transform_idx),
                      continue);

            nux_gpu_command_t *cmd
                = nux_gpu_command_vec_push(&ctx->gpu_commands);
            NUX_ASSERT(cmd);
            if (tex)
            {
                cmd->colormap = NUX_NULL;
                cmd->texture  = tex->slot;
            }
            else
            {
                cmd->colormap = NUX_NULL;
                cmd->texture  = NUX_NULL;
            }
            cmd->vertices        = ctx->vertices_buffer_slot;
            cmd->transforms      = ctx->transforms_buffer_slot;
            cmd->vertex_first    = m->first;
            cmd->vertex_count    = m->count;
            cmd->transform_index = transform_idx;
        }
    }

    // Push constants
    nux_node_t *ce = nux_object_get(ctx, NUX_TYPE_NODE, camera);
    NUX_CHECK(ce, return);
    nux_transform_t *ct
        = nux_scene_get_component(ctx, camera, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(ct, return);
    nux_camera_t *cc
        = nux_scene_get_component(ctx, camera, NUX_COMPONENT_CAMERA);
    NUX_CHECK(cc, return);

    nux_v3_t eye    = nux_m4_mulv3(ct->global_matrix, NUX_V3_ZEROES, 1);
    nux_v3_t center = nux_m4_mulv3(ct->global_matrix, NUX_V3_FORWARD, 1);
    nux_v3_t up     = nux_m4_mulv3(ct->global_matrix, NUX_V3_UP, 0);

    nux_gpu_constants_buffer_t constants;
    constants.view = nux_lookat(eye, center, up);
    constants.proj
        = nux_perspective(nux_radian(cc->fov),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          cc->near,
                          cc->far);
    constants.screen_size = nux_v2u(ctx->stats[NUX_STAT_SCREEN_WIDTH],
                                    ctx->stats[NUX_STAT_SCREEN_HEIGHT]);
    constants.canvas_size = nux_v2u(NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    constants.time        = ctx->time;
    nux_os_update_buffer(ctx->userdata,
                         ctx->constants_buffer_slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit pass
    nux_gpu_pass_t pass = {
        .type                  = NUX_GPU_PASS_MAIN,
        .pipeline              = ctx->main_pipeline_slot,
        .main.constants_buffer = ctx->constants_buffer_slot,
        .count                 = ctx->gpu_commands.size,
    };
    nux_os_gpu_submit_pass(ctx->userdata, &pass, ctx->gpu_commands.data);
}
nux_u32_t
nux_scene_get_node (nux_ctx_t *ctx, nux_u32_t scene, nux_u32_t index)
{
    nux_scene_t *s = nux_object_get(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    NUX_CHECK(index < s->nodes.size, return NUX_NULL);
    return s->nodes.data[index].id;
}
nux_u32_t
nux_node_new (nux_ctx_t *ctx, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_node_t *n = nux_node_pool_add(&s->nodes);
    NUX_CHECK(n, return NUX_NULL);
    nux_u32_t id = nux_object_create(ctx, s->arena, NUX_TYPE_NODE, n);
    NUX_CHECK(id, return NUX_NULL);
    nux_memset(n, 0, sizeof(*n));
    n->scene  = scene;
    n->parent = NUX_NULL;
    n->id     = id;
    return id;
}
void
nux_node_set_parent (nux_ctx_t *ctx, nux_u32_t node, nux_u32_t parent)
{
    NUX_CHECKM(node != parent, "Setting node parent to itself", return);
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return);
    n->parent = parent;

    // Update child components
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    if (t)
    {
        t->dirty = NUX_TRUE;
    }
}
nux_u32_t
nux_node_get_parent (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    return n->parent;
}
nux_u32_t
nux_node_get_scene (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    return n->scene;
}

void
nux_component_register (nux_ctx_t *ctx, nux_u32_t type)
{
    nux_component_type_t *component_type
        = ctx->component_types + ctx->component_types_count;
    component_type->type = type;
    ++ctx->component_types_count;
}

void
nux_scene_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_scene_t *scene = data;
}
void *
nux_scene_add_component (nux_ctx_t *ctx, nux_u32_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(ctx, node, comp_type);
    }
    nux_scene_t     *s = nux_object_get(ctx, NUX_TYPE_SCENE, n->scene);
    nux_component_t *c = nux_component_pool_add(&s->components);
    NUX_CHECKM(c, "Out of scene items", return NUX_NULL);
    *comp_index = c - s->components.data;
    return c;
}
void
nux_scene_remove_component (nux_ctx_t *ctx, nux_u32_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(ctx, NUX_TYPE_SCENE, n->scene);
        nux_component_pool_remove(&s->components,
                                  &s->components.data[*comp_index]);
    }
    *comp_index = NUX_NULL;
}
void *
nux_scene_get_component (nux_ctx_t *ctx, nux_u32_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_object_get(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(ctx, NUX_TYPE_SCENE, n->scene);
        return &s->components.data[*comp_index];
    }
    return NUX_NULL;
}
