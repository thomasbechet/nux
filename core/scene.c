#include "internal.h"

nux_u32_t
nux_scene_new (nux_ctx_t *ctx)
{
    nux_scene_t *s = nux_arena_alloc(ctx->active_arena, sizeof(*s));
    NUX_CHECK(s, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(ctx, ctx->active_arena, NUX_OBJECT_SCENE, s);
    NUX_CHECK(id, return NUX_NULL);

    s->arena = ctx->active_arena;
    NUX_CHECK(nux_entity_pool_alloc(ctx->active_arena, 1024, &s->entities),
              return NUX_NULL);
    NUX_CHECK(nux_component_pool_alloc(ctx->active_arena, 1024, &s->components),
              return NUX_NULL);

    // Reserve index 0 to null
    nux_entity_pool_add(&s->entities);
    nux_component_pool_add(&s->components);

    return id;
}
void
nux_scene_draw (nux_ctx_t *ctx, nux_u32_t scene, nux_u32_t camera)
{
    nux_scene_t *s = nux_object_get(ctx, NUX_OBJECT_SCENE, scene);
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
            nux_transform_update_matrix(ctx, e->id);
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
            nux_mesh_t *m = nux_object_get(ctx, NUX_OBJECT_MESH, sm->mesh);
            NUX_ASSERT(m);

            // Push transform
            nux_u32_t transform_idx;
            NUX_ASSERT(nux_graphics_push_transforms(
                ctx, 1, &t->global_matrix, &transform_idx));

            nux_gpu_command_t *cmd
                = nux_gpu_command_vec_push(&ctx->gpu_commands);
            NUX_ASSERT(cmd);
            cmd->main.colormap        = NUX_NULL;
            cmd->main.texture         = NUX_NULL;
            cmd->main.vertices        = ctx->vertices_buffer_slot;
            cmd->main.transforms      = ctx->transforms_buffer_slot;
            cmd->main.vertex_first    = m->first;
            cmd->main.vertex_count    = m->count;
            cmd->main.transform_index = transform_idx;
        }
    }

    // Push constants
    nux_entity_t *ce = nux_object_get(ctx, NUX_OBJECT_ENTITY, camera);
    NUX_CHECK(ce, return);
    nux_transform_t *ct
        = nux_scene_get_component(ctx, camera, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(ct, return);
    nux_camera_t *cc
        = nux_scene_get_component(ctx, camera, NUX_COMPONENT_CAMERA);
    NUX_CHECK(cc, return);

    nux_v3_t eye    = nux_m4_mulv3(ct->global_matrix, NUX_V3_ZEROES, 1);
    nux_v3_t center = nux_m4_mulv3(ct->global_matrix, NUX_V3_FORWARD, 1);
    nux_v3_t up     = nux_m4_mulv3(ct->global_matrix, NUX_V3_UP, 1);

    nux_gpu_constants_buffer_t constants;
    constants.view = nux_lookat(eye, center, NUX_V3_UP);
    constants.proj
        = nux_perspective(nux_radian(cc->fov),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          0.1,
                          100);
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
nux_entity_new (nux_ctx_t *ctx, nux_u32_t scene)
{
    nux_scene_t *s = nux_object_get(ctx, NUX_OBJECT_SCENE, scene);
    NUX_CHECK(s, return NUX_NULL);
    nux_entity_t *e = nux_entity_pool_add(&s->entities);
    NUX_CHECK(e, return NUX_NULL);
    nux_u32_t id = nux_object_create(ctx, s->arena, NUX_OBJECT_ENTITY, e);
    NUX_CHECK(id, return NUX_NULL);
    nux_memset(e, 0, sizeof(*e));
    e->scene = scene;
    e->id    = id;
    return id;
}
void
nux_scene_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_scene_t *scene = data;
}
void *
nux_scene_add_component (nux_ctx_t           *ctx,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(ctx, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_remove_component(ctx, entity, type);
    }
    nux_scene_t     *s = nux_object_get(ctx, NUX_OBJECT_SCENE, e->scene);
    nux_component_t *c = nux_component_pool_add(&s->components);
    NUX_CHECKM(c, "Out of scene items", return NUX_NULL);
    *comp_index = c - s->components.data;
    return c;
}
void
nux_scene_remove_component (nux_ctx_t           *ctx,
                            nux_u32_t            entity,
                            nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(ctx, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    NUX_ASSERT(comp_index);
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(ctx, NUX_OBJECT_SCENE, e->scene);
        nux_component_pool_remove(&s->components,
                                  &s->components.data[*comp_index]);
    }
    *comp_index = NUX_NULL;
}
void *
nux_scene_get_component (nux_ctx_t           *ctx,
                         nux_u32_t            entity,
                         nux_component_type_t type)
{
    nux_entity_t *e = nux_object_get(ctx, NUX_OBJECT_ENTITY, entity);
    NUX_CHECK(e, return NUX_NULL);
    NUX_ASSERT(type <= NUX_COMPONENT_MAX);
    nux_u32_t *comp_index = e->components + type;
    if (*comp_index)
    {
        nux_scene_t *s = nux_object_get(ctx, NUX_OBJECT_SCENE, e->scene);
        return &s->components.data[*comp_index];
    }
    return NUX_NULL;
}
nux_u32_t
nux_scene_load (nux_ctx_t *ctx, const nux_c8_t *url)
{
    return NUX_NULL;
}
static nux_status_t
parse_component (nux_ctx_t *ctx, nux_u32_t e)
{
    const nux_u32_t c_index = -2;
    return NUX_SUCCESS;
}
static nux_v3_t
parse_v3 (lua_State *L)
{
    lua_geti(L, -1, 1);
    float x = luaL_checknumber(L, -1);
    lua_geti(L, -2, 2);
    float y = luaL_checknumber(L, -1);
    lua_geti(L, -3, 3);
    float    z = luaL_checknumber(L, -1);
    nux_v3_t v = nux_v3(x, y, z);
    lua_pop(L, 3);
    return v;
}
static nux_u32_t
try_parse_u32 (lua_State      *L,
               nux_i32_t       idx,
               const nux_c8_t *k,
               nux_u32_t       default_value)
{
    nux_u32_t ret = default_value;
    lua_getfield(L, idx, k);
    if (!lua_isnil(L, -1))
    {
        ret = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);
    return ret;
}
static nux_f32_t
try_parse_f32 (lua_State      *L,
               nux_i32_t       idx,
               const nux_c8_t *k,
               nux_f32_t       default_value)
{
    nux_f32_t ret = default_value;
    lua_getfield(L, idx, k);
    if (!lua_isnil(L, -1))
    {
        ret = luaL_checknumber(L, -1);
    }
    lua_pop(L, 1);
    return ret;
}
static nux_v3_t
try_parse_v3 (lua_State      *L,
              nux_i32_t       idx,
              const nux_c8_t *k,
              nux_v3_t        default_value)
{
    nux_v3_t ret = default_value;
    lua_getfield(L, idx, k);
    if (!lua_isnil(L, -1))
    {
        ret = parse_v3(L);
    }
    lua_pop(L, 1);
    return ret;
}
static nux_status_t
nux_transform_parse (nux_ctx_t *ctx,
                     nux_u32_t  e,
                     lua_State *L,
                     nux_u32_t  t_index)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_ASSERT(t);
    nux_v3_t translation
        = try_parse_v3(L, t_index, "translation", NUX_V3_ZEROES);
    nux_v3_t scale       = try_parse_v3(L, t_index, "scale", NUX_V3_ONES);
    t->local_translation = translation;
    t->local_scale       = scale;
    t->dirty             = NUX_TRUE;
    return NUX_SUCCESS;
}
static nux_status_t
nux_camera_parse (nux_ctx_t *ctx, nux_u32_t e, lua_State *L, nux_u32_t t_index)
{
    nux_camera_t *c = nux_scene_get_component(ctx, e, NUX_COMPONENT_CAMERA);
    NUX_ASSERT(c);
    c->fov = try_parse_f32(L, t_index, "fov", 60);
    return NUX_SUCCESS;
}
static nux_status_t
nux_staticmesh_parse (nux_ctx_t *ctx,
                      nux_u32_t  e,
                      lua_State *L,
                      nux_u32_t  t_index)
{
    nux_staticmesh_t *sm
        = nux_scene_get_component(ctx, e, NUX_COMPONENT_STATICMESH);
    NUX_ASSERT(sm);
    sm->mesh = try_parse_u32(L, t_index, "mesh", NUX_NULL);
    return NUX_SUCCESS;
}
nux_u32_t
nux_scene_parse (nux_ctx_t *ctx, lua_State *L)
{
    // TODO: handle failure case, deallocate resources

    nux_u32_t s = nux_scene_new(ctx);
    NUX_CHECK(s, return NUX_NULL);

    const nux_u32_t t_index = 1;
    NUX_CHECK(lua_istable(L, t_index), lua_error(L));

    lua_pushnil(L); // key iter
    while (lua_next(L, t_index) != 0)
    {
        lua_pushvalue(L, -2);
        // -1 => key (string or number or ...)
        // -2 => value

        if (lua_isnumber(L, -1)) // Non tagged entity
        {
            nux_i32_t i = (nux_i32_t)lua_tonumber(L, -1);
        }
        else if (lua_isstring(L, -1)) // tagged entity
        {
            const nux_c8_t *tag
                = lua_tostring(L, -1); // No side effect as key is a string
        }

        // Check is table
        NUX_CHECKM(
            lua_istable(L, -2), "Entity value is not a table", return NUX_NULL);

        // Create entity
        nux_u32_t e = nux_entity_new(ctx, s);
        NUX_CHECK(e, return NUX_NULL);

        // Iterate components
        lua_pushnil(L);
        while (lua_next(L, -3))
        {
            lua_pushvalue(L, -2);
            // -1 => key (string or number or ...)
            // -2 => value

            if (lua_isstring(L, -1))
            {
                const nux_c8_t *key
                    = lua_tostring(L, -1); // No side effect as key is a string

                // Check is table
                NUX_CHECKM(lua_istable(L, -2),
                           "Component value is not a table",
                           return NUX_NULL);

                if (NUX_MATCH(key, "transform"))
                {
                    nux_transform_add(ctx, e);
                    nux_transform_parse(ctx, e, L, -2);
                }
                else if (NUX_MATCH(key, "staticmesh"))
                {
                    nux_staticmesh_add(ctx, e);
                    nux_staticmesh_parse(ctx, e, L, -2);
                }
                else if (NUX_MATCH(key, "camera"))
                {
                    nux_camera_add(ctx, e);
                    nux_camera_parse(ctx, e, L, -2);
                }
            }
            lua_pop(L, 2);
        }
        lua_pop(L, 2); // key / value
    }
    lua_pop(L, 1); // key iter

    return s;
}
