#include "platform.h"

nu_status_t
gfx_init (vm_t *vm, const vm_config_t *config)
{
    NU_ASSERT(sizeof(gfx_command_t) == GFX_COMMAND_SIZE);

    // vm->gfx.state.model       = nu_m4_identity();
    // vm->gfx.state.view        = nu_m4_identity();
    // vm->gfx.state.projection  = nu_m4_identity();
    // vm->gfx.state.cursor      = NU_V2U_ZEROS;
    // vm->gfx.state.color       = NU_COLOR_WHITE;
    // vm->gfx.state.fog_color   = NU_COLOR_WHITE;
    // vm->gfx.state.fog_density = 0;

    vm->gfx.cmds_size = config->gpu_command_size;
    NU_ASSERT(vm->gfx.cmds_size);
    vm->gfx.cmds_addr
        = vm_malloc(vm, config->gpu_command_size * GFX_COMMAND_SIZE);
    NU_CHECK(vm->gfx.cmds_addr != ADDR_INVALID, return NU_FAILURE);
    vm->gfx.cmds_count = 0;

    return NU_SUCCESS;
}
nu_status_t
gfx_free (vm_t *vm)
{
    for (nu_size_t i = 0; i < SYS_MAX_RESOURCE_COUNT; ++i)
    {
        switch (vm->res[i].type)
        {
            case RESOURCE_TEXTURE:
                os_gfx_free_texture(vm, i);
                break;
            case RESOURCE_MESH:
                os_gfx_free_mesh(vm, i);
                break;
            default:
                break;
        }
    }
    return NU_SUCCESS;
}
void
gpu_reload_state (vm_t *vm)
{
    for (nu_size_t i = 0; i < SYS_MAX_RESOURCE_COUNT; ++i)
    {
        switch (vm->res[i].type)
        {
            case RESOURCE_TEXTURE: {
                os_gfx_free_texture(vm, i);
                os_gfx_init_texture(vm, i);
                os_gfx_update_texture(vm, i);
            }
            break;
            case RESOURCE_MESH: {
                os_gfx_free_mesh(vm, i);
                os_gfx_init_mesh(vm, i);
                os_gfx_update_mesh(vm, i);
            }
            break;
            default:
                break;
        }
    }
}
void
gfx_begin_frame (vm_t *vm)
{
    vm->gfx.cmds_count = 0;
}
void
gfx_end_frame (vm_t *vm)
{
}

nu_status_t
sys_create_camera (vm_t *vm, nu_u32_t id)
{
    resource_t *res = vm_set_res(vm, id, RESOURCE_CAMERA);
    NU_CHECK(res, return NU_FAILURE);
    res->camera.data = vm_malloc(vm, sizeof(gfx_camera_t));
    NU_ASSERT(res->camera.data != ADDR_INVALID);
    return NU_SUCCESS;
}
void
sys_set_camera_view (vm_t *vm, nu_u32_t id, const nu_f32_t *m)
{
    resource_t *res = vm_get_res(vm, id, RESOURCE_CAMERA);
    NU_ASSERT(res);
    gfx_camera_t *cam = (gfx_camera_t *)(vm->mem + res->camera.data);
    cam->view         = nu_m4(m);
}
void
sys_set_camera_projection (vm_t *vm, nu_u32_t id, const nu_f32_t *m)
{
    resource_t *res = vm_get_res(vm, id, RESOURCE_CAMERA);
    NU_ASSERT(res);
    gfx_camera_t *cam = vm_ptr(vm, res->camera.data);
    cam->projection   = nu_m4(m);
}
void
sys_set_camera_lookat (vm_t           *vm,
                       nu_u32_t        id,
                       const nu_f32_t *eye,
                       const nu_f32_t *center,
                       const nu_f32_t *up)
{
    nu_m4_t view = nu_lookat(nu_v3(eye[0], eye[1], eye[2]),
                             nu_v3(center[0], center[1], center[2]),
                             nu_v3(up[0], up[1], up[2]));
    sys_set_camera_view(vm, id, view.data);
}
void
sys_set_camera_perspective (
    vm_t *vm, nu_u32_t id, nu_f32_t fov, nu_f32_t near, nu_f32_t far)
{
    nu_m4_t projection = nu_perspective(
        fov, (nu_f32_t)SYS_SCREEN_WIDTH / SYS_SCREEN_HEIGHT, near, far);
    sys_set_camera_projection(vm, id, projection.data);
}

nu_status_t
sys_create_texture (vm_t *vm, nu_u32_t id, nu_u32_t size)
{
    if (size < SYS_MIN_TEXTURE_SIZE || size > SYS_MAX_TEXTURE_SIZE)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Invalid texture size %d (min %d max %d)",
               size,
               SYS_MIN_TEXTURE_SIZE,
               SYS_MAX_TEXTURE_SIZE);
        return ID_NULL;
    }
    resource_t *res = vm_set_res(vm, id, RESOURCE_TEXTURE);
    NU_CHECK(res, return NU_FAILURE);
    res->texture.size = size;
    res->texture.data = vm_malloc(vm, gfx_texture_memsize(size));
    NU_CHECK(res->texture.data != ADDR_INVALID, return ID_NULL);
    os_gfx_init_texture(vm, id);
    return id;
}
nu_status_t
sys_update_texture (vm_t       *vm,
                    nu_u32_t    id,
                    nu_u32_t    x,
                    nu_u32_t    y,
                    nu_u32_t    w,
                    nu_u32_t    h,
                    const void *p)
{
    resource_t *res = vm_get_res(vm, id, RESOURCE_TEXTURE);
    NU_CHECK(res, return NU_FAILURE);
    nu_byte_t *data = vm_ptr(vm, res->texture.data);
    NU_ASSERT(x + w <= res->texture.size);
    NU_ASSERT(y + h <= res->texture.size);
    for (nu_size_t i = 0; i < h; ++i)
    {
        nu_byte_t *row = data + res->texture.size * (y + i) + x;
        nu_byte_t *src = ((nu_byte_t *)p) + w * i;
        nu_memcpy(row, src, w);
    }
    os_gfx_update_texture(vm, id);
    return NU_SUCCESS;
}
nu_status_t
sys_create_mesh (vm_t                  *vm,
                 nu_u32_t               id,
                 nu_u32_t               count,
                 sys_primitive_t        primitive,
                 sys_vertex_attribute_t attributes)
{
    resource_t *res = vm_set_res(vm, id, RESOURCE_MESH);
    NU_CHECK(res, return NU_FAILURE);
    res->mesh.count      = count;
    res->mesh.primitive  = primitive;
    res->mesh.attributes = attributes;
    res->mesh.data       = vm_malloc(vm, gfx_vertex_memsize(attributes, count));
    NU_CHECK(res->mesh.data != ADDR_INVALID, return ID_NULL);
    nu_memset(
        vm_ptr(vm, res->mesh.data), 0, gfx_vertex_memsize(attributes, count));
    os_gfx_init_mesh(vm, id);
    return NU_SUCCESS;
}
nu_status_t
sys_update_mesh (vm_t                  *vm,
                 nu_u32_t               id,
                 sys_vertex_attribute_t attributes,
                 nu_u32_t               first,
                 nu_u32_t               count,
                 const void            *p)
{
    resource_t     *res  = vm_get_res(vm, id, RESOURCE_MESH);
    nu_f32_t       *ptr  = vm_ptr(vm, res->mesh.data);
    const nu_f32_t *data = p;
    if (attributes & SYS_VERTEX_POSITION
        && res->mesh.attributes & SYS_VERTEX_POSITION)
    {
        nu_u32_t src_offset
            = gfx_vertex_offset(attributes, SYS_VERTEX_POSITION, count);
        nu_u32_t dst_offset = gfx_vertex_offset(
            res->mesh.attributes, SYS_VERTEX_POSITION, res->mesh.count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 3 + 0]
                = data[src_offset + i * 3 + 0];
            ptr[dst_offset + (first + i) * 3 + 1]
                = data[src_offset + i * 3 + 1];
            ptr[dst_offset + (first + i) * 3 + 2]
                = data[src_offset + i * 3 + 2];
        }
    }
    if (attributes & SYS_VERTEX_UV && res->mesh.attributes & SYS_VERTEX_UV)
    {
        nu_u32_t src_offset
            = gfx_vertex_offset(attributes, SYS_VERTEX_UV, count);
        nu_u32_t dst_offset = gfx_vertex_offset(
            res->mesh.attributes, SYS_VERTEX_UV, res->mesh.count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 2 + 0]
                = data[src_offset + i * 2 + 0];
            ptr[dst_offset + (first + i) * 2 + 1]
                = data[src_offset + i * 2 + 1];
        }
    }
    if (attributes & SYS_VERTEX_COLOR
        && res->mesh.attributes & SYS_VERTEX_COLOR)
    {
        nu_u32_t src_offset
            = gfx_vertex_offset(attributes, SYS_VERTEX_COLOR, count);
        nu_u32_t dst_offset = gfx_vertex_offset(
            res->mesh.attributes, SYS_VERTEX_COLOR, res->mesh.count);
        for (nu_size_t i = 0; i < count; ++i)
        {
            ptr[dst_offset + (first + i) * 3 + 0]
                = data[src_offset + i * 3 + 0];
            ptr[dst_offset + (first + i) * 3 + 1]
                = data[src_offset + i * 3 + 1];
            ptr[dst_offset + (first + i) * 3 + 2]
                = data[src_offset + i * 3 + 2];
        }
    }
    os_gfx_update_mesh(vm, id);
    return NU_SUCCESS;
}

nu_status_t
sys_create_model (vm_t *vm, nu_u32_t id, nu_u32_t node_count)
{
    resource_t *res = vm_set_res(vm, id, RESOURCE_MODEL);
    NU_CHECK(res, return NU_FAILURE);
    nu_u32_t memsize = node_count * sizeof(gfx_model_node_t);
    res->model.data  = vm_malloc(vm, memsize);
    NU_CHECK(res->model.data != ADDR_INVALID, return ID_NULL);
    res->model.node_count = node_count;
    os_gfx_init_model(vm, id);
    return NU_SUCCESS;
}
nu_status_t
sys_update_model (vm_t           *vm,
                  nu_u32_t        id,
                  nu_u32_t        node_index,
                  nu_u32_t        mesh,
                  nu_u32_t        texture,
                  nu_u32_t        parent,
                  const nu_f32_t *transform)
{
    resource_t *res = vm_get_res(vm, id, RESOURCE_MODEL);
    if (node_index >= res->model.node_count)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid model node index %d", node_index);
        return NU_FAILURE;
    }
    // TODO: save model node in memory
    gfx_model_node_t node = { .texture         = texture,
                              .mesh            = mesh,
                              .parent          = parent,
                              .local_to_parent = nu_m4(transform) };
    os_gfx_update_model(vm, id, node_index, &node);
    return NU_SUCCESS;
}

nu_status_t
sys_create_spritesheet (vm_t    *vm,
                        nu_u32_t id,
                        nu_u32_t texture,
                        nu_u32_t row,
                        nu_u32_t col,
                        nu_u32_t fwidth,
                        nu_u32_t fheight)
{
    resource_t *res = vm_set_res(vm, id, RESOURCE_SPRITESHEET);
    NU_CHECK(res, return NU_FAILURE);
    res->spritesheet.texture = texture;
    res->spritesheet.row     = row;
    res->spritesheet.col     = col;
    res->spritesheet.fwidth  = fwidth;
    res->spritesheet.fheight = fheight;
    return NU_SUCCESS;
}

static gfx_command_t *
push_command (vm_t *vm)
{
    if (vm->gfx.cmds_count >= vm->gfx.cmds_size)
    {
        vm_log(vm, NU_LOG_ERROR, "out of graphics command buffer memory");
        return NU_NULL;
    }
    gfx_command_t *cmds = vm_ptr(vm, vm->gfx.cmds_addr);
    gfx_command_t *cmd  = cmds + vm->gfx.cmds_count;
    ++vm->gfx.cmds_count;
    return cmd;
}

// void
// sys_set_render_state (vm_t *vm, sys_render_state_t state, const void *p)
// {
//     switch (state)
//     {
//         case SYS_RENDER_SCISSOR:
//             vm->gfx.state.scissor = *(const nu_v4u_t *)p;
//             break;
//         case SYS_RENDER_VIEWPORT:
//             vm->gfx.state.viewport = *(const nu_v4u_t *)p;
//             break;
//         case SYS_RENDER_TRANSFORM:
//             vm->gfx.state.model = *(const nu_m4_t *)p;
//             break;
//         case SYS_RENDER_VIEW:
//             vm->gfx.state.view = *(const nu_m4_t *)p;
//             break;
//         case SYS_RENDER_PROJECTION:
//             vm->gfx.state.projection = *(const nu_m4_t *)p;
//             break;
//         case SYS_RENDER_CURSOR:
//             vm->gfx.state.cursor = *(const nu_v2u_t *)p;
//             break;
//         case SYS_RENDER_FOG_COLOR:
//             vm->gfx.state.fog_color = nu_color_from_u32(*(const nu_u32_t
//             *)p); break;
//         case SYS_RENDER_FOG_DENSITY:
//             vm->gfx.state.fog_density = nu_fabs(*(const nu_f32_t *)p);
//             break;
//         case SYS_RENDER_FOG_NEAR:
//             vm->gfx.state.fog_near = nu_fabs(*(const nu_f32_t *)p);
//             break;
//         case SYS_RENDER_FOG_FAR:
//             vm->gfx.state.fog_far
//                 = NU_MAX(vm->gfx.state.fog_near, *(const nu_f32_t *)p);
//             break;
//         case SYS_RENDER_COLOR:
//             vm->gfx.state.color = nu_color_from_u32(*(const nu_u32_t *)p);
//             break;
//     }
// }
void
sys_push_scissor (vm_t *vm, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    gfx_command_t *cmd     = push_command(vm);
    cmd->type              = GFX_CMD_PUSH_SCISSOR;
    cmd->push_scissor.rect = nu_v4u(x, y, w, h);
}
void
sys_push_viewport (vm_t *vm, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    gfx_command_t *cmd      = push_command(vm);
    cmd->type               = GFX_CMD_PUSH_VIEWPORT;
    cmd->push_viewport.rect = nu_v4u(x, y, w, h);
}
void
sys_push_camera (vm_t *vm, nu_u32_t id)
{
    gfx_command_t *cmd      = push_command(vm);
    cmd->type               = GFX_CMD_PUSH_CAMERA;
    cmd->push_camera.camera = id;
}
void
sys_push_translation (vm_t *vm, nu_f32_t x, nu_f32_t y, nu_f32_t z)
{
    gfx_command_t *cmd    = push_command(vm);
    cmd->type             = GFX_CMD_PUSH_TRANSLATION;
    cmd                   = push_command(vm);
    cmd->push_translation = nu_v3(x, y, z);
}
void
sys_push_cursor (vm_t *vm, nu_u32_t x, nu_u32_t y)
{
    gfx_command_t *cmd        = push_command(vm);
    cmd->type                 = GFX_CMD_PUSH_CURSOR;
    cmd->push_cursor.position = nu_v2u(x, y);
}
void
sys_push_color (vm_t *vm, nu_u32_t color)
{
    gfx_command_t *cmd    = push_command(vm);
    cmd->type             = GFX_CMD_PUSH_COLOR;
    cmd->push_color.color = nu_color_from_u32(color);
}

void
sys_clear (vm_t *vm, nu_u32_t color)
{
    gfx_command_t *cmd = push_command(vm);
    cmd->type          = GFX_CMD_CLEAR;
}
void
sys_draw_model (vm_t *vm, nu_u32_t id)
{
    NU_CHECK(vm_get_res(vm, id, RESOURCE_MODEL), return);
    gfx_command_t *cmd    = push_command(vm);
    cmd->type             = GFX_CMD_DRAW_MODEL;
    cmd->draw_model.model = id;
}
void
sys_draw_volume (vm_t *vm)
{
    const nu_f32_t r    = 100;
    const nu_f32_t r2   = r * 2;
    const nu_f32_t c[3] = { -r, -r, -r };
    const nu_f32_t s[3] = { r2, r2, r2 };
    // os_gfx_draw_volume(vm, c, s);
}
void
sys_draw_cube (vm_t *vm, const nu_f32_t *p, const nu_f32_t *s)
{
}
void
sys_draw_lines (vm_t *vm, const nu_f32_t *p, nu_u32_t n)
{
}
void
sys_draw_linestrip (vm_t *vm, const nu_f32_t *p, nu_u32_t n)
{
}
void
sys_draw_text (vm_t *vm, const void *text)
{
    // os_gfx_draw_text(vm, text, nu_strnlen(text, 4096));
}
void
sys_print (vm_t *vm, const void *text)
{
    sys_draw_text(vm, text);
    // vm->gfx.state.cursor.y += 9;
}
void
sys_blit (vm_t *vm, nu_u32_t id, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    NU_CHECK(vm_get_res(vm, id, RESOURCE_TEXTURE), return);
    gfx_command_t *cmd = push_command(vm);
    NU_ASSERT(cmd);
    cmd->type         = GFX_CMD_BLIT;
    cmd->blit.texture = id;
    cmd->blit.x       = x;
    cmd->blit.y       = y;
    cmd->blit.w       = w;
    cmd->blit.h       = h;
}
void
sys_draw_sprite (vm_t *vm, nu_u32_t spritesheet, nu_u32_t sprite)
{
    resource_t *res = vm_get_res(vm, spritesheet, RESOURCE_SPRITESHEET);
    NU_CHECK(res, return);
    nu_u32_t x = (sprite % res->spritesheet.row) * res->spritesheet.fwidth;
    nu_u32_t y = (sprite / res->spritesheet.row) * res->spritesheet.fheight;

    sys_blit(vm,
             res->spritesheet.texture,
             x,
             y,
             res->spritesheet.fwidth,
             res->spritesheet.fheight);
}

nu_u32_t
gfx_texture_memsize (nu_u32_t size)
{
    return size * size * 4;
}
nu_u32_t
gfx_vertex_memsize (sys_vertex_attribute_t attributes, nu_u32_t count)
{
    nu_u32_t size = 0;
    if (attributes & SYS_VERTEX_POSITION)
    {
        size += NU_V3_SIZE * count;
    }
    if (attributes & SYS_VERTEX_UV)
    {
        size += NU_V2_SIZE * count;
    }
    if (attributes & SYS_VERTEX_COLOR)
    {
        size += NU_V3_SIZE * count;
    }
    return size * sizeof(nu_f32_t);
}
nu_u32_t
gfx_vertex_offset (sys_vertex_attribute_t attributes,
                   sys_vertex_attribute_t attribute,
                   nu_u32_t               count)
{
    NU_ASSERT(attribute & attributes);
    nu_u32_t offset = 0;
    if (attributes & SYS_VERTEX_POSITION)
    {
        if (attribute == SYS_VERTEX_POSITION)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    if (attributes & SYS_VERTEX_UV)
    {
        if (attribute == SYS_VERTEX_UV)
        {
            return offset;
        }
        offset += NU_V2_SIZE * count;
    }
    if (attributes & SYS_VERTEX_COLOR)
    {
        if (attribute == SYS_VERTEX_COLOR)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    return offset;
}
