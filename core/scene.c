#include "internal.h"

nux_u32_t
nux_scene_new (nux_env_t *env)
{
    nux_scene_t *s = nux_arena_alloc(env->active_arena, sizeof(*s));
    NUX_CHECK(s, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(env, env->active_arena, NUX_OBJECT_SCENE, s);
    NUX_CHECK(id, return NUX_NULL);

    s->arena = env->active_arena;
    NUX_CHECK(nux_entity_pool_alloc(env->active_arena, 1024, &s->entities),
              return NUX_NULL);
    NUX_CHECK(nux_component_pool_alloc(env->active_arena, 1024, &s->components),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_entity_pool_add(&s->entities);
    nux_component_pool_add(&s->components);

    return id;
}
void
nux_scene_draw (nux_env_t *env, nux_u32_t scene, nux_u32_t camera)
{
    nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(s, return);

    // Propagate transforms
    for (nux_u32_t ei = 0; ei < s->entities.size; ++ei)
    {
        nux_entity_t *e = s->entities.data + ei;
        if (!e->id)
        {
            continue;
        }
        if (e->components[NUX_COMPONENT_TRANSFORM])
        {
            nux_transform_t *t
                = &s->components.data[e->components[NUX_COMPONENT_TRANSFORM]]
                       .transform;
            nux_transform_update_matrix(env, e->id);
        }
    }

    // Draw entities
    for (nux_u32_t ei = 0; ei < s->entities.size; ++ei)
    {
        nux_entity_t *e = s->entities.data + ei;
        if (!e->id)
        {
            continue;
        }
        if (e->components[NUX_COMPONENT_TRANSFORM]
            && e->components[NUX_COMPONENT_STATICMESH])
        {
            nux_staticmesh_t *sm
                = &s->components.data[e->components[NUX_COMPONENT_STATICMESH]]
                       .staticmesh;
            if (!sm->mesh)
            {
                continue;
            }
            nux_transform_t *t
                = &s->components.data[e->components[NUX_COMPONENT_TRANSFORM]]
                       .transform;
            nux_mesh_t *m = nux_object_get(env, NUX_OBJECT_MESH, sm->mesh);
            NUX_ASSERT(m);

            // Push transform
            nux_u32_t transform_idx;
            NUX_ASSERT(nux_graphics_push_transforms(
                env, 1, &t->global_matrix, &transform_idx));

            nux_gpu_command_t *cmd
                = nux_gpu_command_vec_push(&env->inst->gpu_commands);
            NUX_ASSERT(cmd);
            cmd->main.colormap        = NUX_NULL;
            cmd->main.texture         = NUX_NULL;
            cmd->main.vertices        = env->inst->vertices_buffer_slot;
            cmd->main.transforms      = env->inst->transforms_buffer_slot;
            cmd->main.vertex_first    = m->first;
            cmd->main.vertex_count    = m->count;
            cmd->main.transform_index = transform_idx;
        }
    }

    // Push constants
    nux_entity_t *ce = nux_object_get(env, NUX_OBJECT_ENTITY, camera);
    NUX_CHECK(ce, return);
    nux_transform_t *ct
        = nux_scene_get_component(env, camera, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(ct, return);
    nux_camera_t *cc
        = nux_scene_get_component(env, camera, NUX_COMPONENT_CAMERA);
    NUX_CHECK(cc, return);

    nux_v3_t eye    = nux_m4_mulv3(ct->global_matrix, nux_v3s(0));
    nux_v3_t center = nux_m4_mulv3(ct->global_matrix, nux_v3(0, 0, -1));
    nux_v3_t up     = nux_m4_mulv3(ct->global_matrix, NUX_V3_UP);

    nux_gpu_constants_buffer_t constants;
    constants.view = nux_lookat(eye, center, NUX_V3_UP);
    constants.proj
        = nux_perspective(nux_radian(cc->fov),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          0.1,
                          100);
    constants.screen_size = nux_v2u(env->inst->stats[NUX_STAT_SCREEN_WIDTH],
                                    env->inst->stats[NUX_STAT_SCREEN_HEIGHT]);
    constants.canvas_size = nux_v2u(NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    constants.time        = env->inst->time;
    nux_os_update_buffer(env->inst->userdata,
                         env->inst->constants_buffer_slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit pass
    nux_gpu_pass_t pass = {
        .type                  = NUX_GPU_PASS_MAIN,
        .pipeline              = env->inst->main_pipeline_slot,
        .main.constants_buffer = env->inst->constants_buffer_slot,
        .count                 = env->inst->gpu_commands.size,
    };
    nux_os_gpu_submit_pass(
        env->inst->userdata, &pass, env->inst->gpu_commands.data);
}
nux_u32_t
nux_entity_new (nux_env_t *env, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_entity_t *e = nux_entity_pool_add(&s->entities);
    NUX_CHECK(e, return NUX_NULL);
    nux_u32_t id = nux_object_create(env, s->arena, NUX_OBJECT_ENTITY, e);
    NUX_CHECK(id, return NUX_NULL);
    nux_memset(e, 0, sizeof(*e));
    e->scene = scene;
    e->id    = id;
    return id;
}
void
nux_scene_cleanup (nux_env_t *env, void *data)
{
    nux_scene_t *scene = data;
}
void *
nux_scene_add_component (nux_env_t           *env,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(env, entity, type);
    }
    nux_scene_t     *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
    nux_component_t *c = nux_component_pool_add(&s->components);
    NUX_CHECKM(c, "Out of scene items", return NUX_NULL);
    *comp_index = c - s->components.data;
    return c;
}
void
nux_scene_remove_component (nux_env_t           *env,
                            nux_u32_t            entity,
                            nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        nux_component_pool_remove(&s->components,
                                  &s->components.data[*comp_index]);
    }
    *comp_index = NUX_NULL;
}
void *
nux_scene_get_component (nux_env_t           *env,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(env, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(env, NUX_OBJECT_SCENE, e->scene);
        return &s->components.data[*comp_index];
    }
    return NUX_NULL;
}
