#include "internal.h"

#define DEFAULT_NODE_COUNT      4096
#define TRANSFORMS_DEFAULT_SIZE 4096
#define BATCH_DEFAULT_SIZE      512

static nux_status_t
push_transforms (nux_ctx_t      *ctx,
                 nux_scene_t    *scene,
                 nux_u32_t       mcount,
                 const nux_m4_t *data,
                 nux_u32_t      *index)
{
    NUX_ENSURE(scene->transforms_buffer_head + mcount < TRANSFORMS_DEFAULT_SIZE,
               return NUX_FAILURE,
               "out of transforms");
    NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                    scene->transforms_buffer.slot,
                                    scene->transforms_buffer_head * NUX_M4_SIZE
                                        * sizeof(nux_f32_t),
                                    mcount * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               return NUX_FAILURE,
               "failed to update transform buffer");
    *index = scene->transforms_buffer_head;
    scene->transforms_buffer_head += mcount;
    return NUX_SUCCESS;
}

nux_id_t
nux_scene_new (nux_ctx_t *ctx)
{
    nux_id_t     id;
    nux_scene_t *s
        = nux_arena_alloc_resource(ctx, NUX_TYPE_SCENE, sizeof(*s), &id);
    NUX_CHECK(s, return NUX_NULL);

    s->arena = ctx->active_arena;
    NUX_CHECK(nux_node_pool_alloc(ctx, DEFAULT_NODE_COUNT, &s->nodes),
              return NUX_NULL);
    NUX_CHECK(nux_component_pool_alloc(ctx, DEFAULT_NODE_COUNT, &s->components),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_node_pool_add(&s->nodes);
    nux_component_pool_add(&s->components);

    // Allocate gpu commands buffer
    NUX_CHECK(nux_gpu_command_vec_alloc(ctx, 4096, &s->commands),
              return NUX_NULL);
    NUX_CHECK(nux_gpu_command_vec_alloc(ctx, 4096, &s->commands_lines),
              return NUX_NULL);

    // Allocate constants buffer
    s->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    s->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(ctx, &s->constants_buffer), return NUX_NULL);

    // Allocate batches buffer
    s->batches_buffer_head = 0;
    s->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    s->batches_buffer.size = sizeof(nux_gpu_scene_batch_t) * BATCH_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &s->batches_buffer), return NUX_NULL);

    // Allocate transforms buffer
    s->transforms_buffer_head = 0;
    s->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    s->transforms_buffer.size = NUX_M4_SIZE * TRANSFORMS_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(ctx, &s->transforms_buffer), return NUX_NULL);

    return id;
}
void
nux_scene_render (nux_ctx_t *ctx, nux_id_t scene, nux_id_t camera)
{
    nux_scene_t *s = nux_id_check(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return);

    // Propagate transforms
    // nux_u32_t batch_count = 0;
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
            if (n->components[NUX_COMPONENT_STATICMESH])
            {
                // ++batch_count;
            }
        }
    }

    // Bind framebuffer, pipeline and constants
    nux_gpu_bind_framebuffer(&s->commands, 0);
    nux_gpu_clear(&s->commands, 0x4f9bd9);

    // Begin opaque pass
    nux_gpu_bind_pipeline(&s->commands, ctx->uber_pipeline_opaque.slot);
    nux_gpu_bind_buffer(
        &s->commands, NUX_GPU_DESC_UBER_CONSTANTS, s->constants_buffer.slot);
    nux_gpu_bind_buffer(
        &s->commands, NUX_GPU_DESC_UBER_BATCHES, s->batches_buffer.slot);
    nux_gpu_bind_buffer(
        &s->commands, NUX_GPU_DESC_UBER_TRANSFORMS, s->transforms_buffer.slot);
    nux_gpu_bind_buffer(
        &s->commands, NUX_GPU_DESC_UBER_VERTICES, ctx->vertices_buffer.slot);

    // Begin lines pass
    nux_gpu_bind_pipeline(&s->commands_lines, ctx->uber_pipeline_line.slot);

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
            nux_mesh_t *m = nux_id_check(ctx, NUX_TYPE_MESH, sm->mesh);
            NUX_ASSERT(m);
            nux_texture_t *tex = NUX_NULL;
            if (sm->texture)
            {
                tex = nux_id_check(ctx, NUX_TYPE_TEXTURE, sm->texture);
            }

            // Push transform
            nux_u32_t transform_index;
            NUX_CHECK(
                push_transforms(ctx, s, 1, &t->global_matrix, &transform_index),
                continue);

            // Create batch
            nux_u32_t batch_index = s->batches_buffer_head;
            ++s->batches_buffer_head;
            nux_gpu_scene_batch_t batch;
            batch.first_transform = transform_index;
            batch.first_vertex    = m->first;
            batch.has_texture     = tex ? 1 : 0;
            NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                            s->batches_buffer.slot,
                                            batch_index * sizeof(batch),
                                            sizeof(batch),
                                            &batch),
                       continue,
                       "failed to update batches buffer");

            // Create commands
            if (tex)
            {
                nux_gpu_bind_texture(
                    &s->commands, NUX_GPU_DESC_UBER_TEXTURE0, tex->gpu.slot);
            }
            nux_gpu_push_u32(
                &s->commands, NUX_GPU_DESC_UBER_BATCH_INDEX, batch_index);
            nux_gpu_draw(&s->commands, m->count);

            // Create line batch
            if (m->bounds_first)
            {
                batch_index = s->batches_buffer_head;
                ++s->batches_buffer_head;
                batch.first_transform = transform_index;
                batch.first_vertex    = m->bounds_first;
                batch.has_texture     = 0;
                NUX_ENSURE(nux_os_buffer_update(ctx->userdata,
                                                s->batches_buffer.slot,
                                                batch_index * sizeof(batch),
                                                sizeof(batch),
                                                &batch),
                           continue,
                           "failed to update batches buffer");
                nux_gpu_push_u32(&s->commands_lines,
                                 NUX_GPU_DESC_UBER_BATCH_INDEX,
                                 batch_index);
                nux_gpu_draw(&s->commands_lines, 12 * 2);
            }
        }
    }

    // Update constants
    nux_node_t *ce = nux_id_check(ctx, NUX_TYPE_NODE, camera);
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
    constants.time        = ctx->time;
    nux_os_buffer_update(ctx->userdata,
                         s->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit commands
    nux_os_gpu_submit(ctx->userdata, s->commands.data, s->commands.size);
    nux_os_gpu_submit(
        ctx->userdata, s->commands_lines.data, s->commands_lines.size);

    // Reset frame data
    s->transforms_buffer_head = 0;
    s->batches_buffer_head    = 0;
    nux_gpu_command_vec_clear(&s->commands);
    nux_gpu_command_vec_clear(&s->commands_lines);
}
nux_id_t
nux_scene_get_node (nux_ctx_t *ctx, nux_id_t scene, nux_u32_t index)
{
    nux_scene_t *s = nux_id_check(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    NUX_CHECK(index < s->nodes.size, return NUX_NULL);
    return s->nodes.data[index].id;
}
nux_id_t
nux_node_new (nux_ctx_t *ctx, nux_id_t scene)
{
    nux_scene_t *s = nux_id_check(ctx, NUX_TYPE_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_node_t *n = nux_node_pool_add(&s->nodes);
    NUX_CHECK(n, return NUX_NULL);
    nux_id_t id = nux_id_create(ctx, NUX_TYPE_NODE, n);
    NUX_CHECK(id, return NUX_NULL);
    nux_memset(n, 0, sizeof(*n));
    n->scene  = scene;
    n->parent = NUX_NULL;
    n->id     = id;
    return id;
}
void
nux_node_set_parent (nux_ctx_t *ctx, nux_id_t node, nux_id_t parent)
{
    NUX_ENSURE(node != parent, return, "setting node parent to itself");
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
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
nux_id_t
nux_node_get_parent (nux_ctx_t *ctx, nux_id_t node)
{
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    return n->parent;
}
nux_id_t
nux_node_get_scene (nux_ctx_t *ctx, nux_id_t node)
{
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
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
nux_scene_add_component (nux_ctx_t *ctx, nux_id_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(ctx, node, comp_type);
    }
    nux_scene_t     *s = nux_id_check(ctx, NUX_TYPE_SCENE, n->scene);
    nux_component_t *c = nux_component_pool_add(&s->components);
    NUX_ENSURE(c, return NUX_NULL, "out of scene items");
    *comp_index = c - s->components.data;
    return c;
}
void
nux_scene_remove_component (nux_ctx_t *ctx, nux_id_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_id_check(ctx, NUX_TYPE_SCENE, n->scene);
        nux_component_pool_remove(&s->components,
                                  &s->components.data[*comp_index]);
    }
    *comp_index = NUX_NULL;
}
void *
nux_scene_get_component (nux_ctx_t *ctx, nux_id_t node, nux_u32_t comp_type)
{
    nux_node_t *n = nux_id_check(ctx, NUX_TYPE_NODE, node);
    NUX_CHECK(n, return NUX_NULL);
    NUX_ASSERT(comp_type < ctx->component_types_count);
    nux_u32_t *comp_index = n->components + comp_type;
    if (*comp_index)
    {
        nux_scene_t *s = nux_id_check(ctx, NUX_TYPE_SCENE, n->scene);
        return &s->components.data[*comp_index];
    }
    return NUX_NULL;
}
