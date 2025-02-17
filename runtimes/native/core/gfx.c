#include "gfx.h"

#include "vm.h"
#include "platform.h"

nu_status_t
gfx_init (vm_t *vm)
{
    vm->gfx.state.model       = nu_m4_identity();
    vm->gfx.state.view        = nu_m4_identity();
    vm->gfx.state.projection  = nu_m4_identity();
    vm->gfx.state.cursor      = NU_V2U_ZEROS;
    vm->gfx.state.color       = NU_COLOR_WHITE;
    vm->gfx.state.fog_color   = NU_COLOR_WHITE;
    vm->gfx.state.fog_density = 0;
    return NU_SUCCESS;
}
nu_status_t
gfx_free (vm_t *vm)
{
    for (nu_size_t i = 0; i < MAX_RESOURCE_COUNT; ++i)
    {
        switch (vm->res[i].type)
        {
            case RES_TEXTURE:
                os_gpu_free_texture(vm, i);
                break;
            case RES_MESH:
                os_gpu_free_mesh(vm, i);
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
    for (nu_size_t i = 0; i < MAX_RESOURCE_COUNT; ++i)
    {
        switch (vm->res[i].type)
        {
            case RES_TEXTURE: {
                os_gpu_free_texture(vm, i);
                os_gpu_init_texture(vm, i);
                os_gpu_update_texture(vm, i);
            }
            break;
            case RES_MESH: {
                os_gpu_free_mesh(vm, i);
                os_gpu_init_mesh(vm, i);
                os_gpu_update_mesh(vm, i);
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
    os_gpu_begin_frame(vm);
}
void
gfx_end_frame (vm_t *vm)
{
    os_gpu_end_frame(vm);
}

nu_u32_t
sys_add_texture (vm_t *vm, nu_u32_t size)
{
    if (size < GFX_MIN_TEXTURE_SIZE || size > GFX_MAX_TEXTURE_SIZE)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Invalid texture size %d (min %d max %d)",
               GFX_MIN_TEXTURE_SIZE,
               GFX_MAX_TEXTURE_SIZE,
               size);
        return ID_NULL;
    }
    nu_u32_t id = vm_add_res(vm, RES_TEXTURE);
    NU_CHECK(id, return ID_NULL);
    resource_t *res   = vm->res + ID_TO_INDEX(id);
    res->texture.size = size;
    res->texture.data = vm_malloc(vm, gfx_texture_memsize(size));
    NU_CHECK(res->texture.data != ADDR_INVALID, return ID_NULL);
    os_gpu_init_texture(vm, id);
    return id;
}
nu_status_t
sys_write_texture (vm_t       *vm,
                   nu_u32_t    id,
                   nu_u32_t    x,
                   nu_u32_t    y,
                   nu_u32_t    w,
                   nu_u32_t    h,
                   const void *p)
{
    NU_CHECK(id, return NU_FAILURE);
    resource_t *res = vm->res + ID_TO_INDEX(id);
    NU_CHECK(res->type == RES_TEXTURE, return NU_FAILURE);
    nu_byte_t *data = vm->mem + res->texture.data;
    NU_ASSERT(x + w <= res->texture.size);
    NU_ASSERT(y + h <= res->texture.size);
    for (nu_size_t i = 0; i < h; ++i)
    {
        nu_byte_t *row = data + res->texture.size * (y + i) + x;
        nu_byte_t *src = ((nu_byte_t *)p) + w * i;
        nu_memcpy(row, src, w);
    }
    os_gpu_update_texture(vm, id);
    return NU_SUCCESS;
}
nu_u32_t
sys_add_mesh (vm_t                  *vm,
              nu_u32_t               count,
              sys_primitive_t        primitive,
              sys_vertex_attribute_t attributes)
{
    nu_u32_t id = vm_add_res(vm, RES_MESH);
    NU_CHECK(id, return ID_NULL);
    resource_t *res      = vm->res + ID_TO_INDEX(id);
    res->mesh.count      = count;
    res->mesh.primitive  = primitive;
    res->mesh.attributes = attributes;
    res->mesh.data       = vm_malloc(vm, gfx_vertex_memsize(attributes, count));
    NU_CHECK(res->mesh.data != ADDR_INVALID, return ID_NULL);
    nu_memset(
        vm->mem + res->mesh.data, 0, gfx_vertex_memsize(attributes, count));
    os_gpu_init_mesh(vm, id);
    return id;
}
nu_status_t
sys_write_mesh (vm_t                  *vm,
                nu_u32_t               id,
                sys_vertex_attribute_t attributes,
                nu_u32_t               first,
                nu_u32_t               count,
                const void            *p)
{
    NU_CHECK(id, return NU_FAILURE);
    resource_t *res = vm->res + ID_TO_INDEX(id);
    NU_CHECK(res->type == RES_MESH, return NU_FAILURE);
    nu_f32_t       *ptr  = (nu_f32_t *)(vm->mem + res->mesh.data);
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
    os_gpu_update_mesh(vm, id);
    return NU_SUCCESS;
}

nu_u32_t
sys_add_model (vm_t *vm, nu_u32_t node_count)
{
    nu_u32_t id = vm_add_res(vm, RES_MODEL);
    NU_CHECK(id, return ID_NULL);
    resource_t *res     = vm->res + ID_TO_INDEX(id);
    nu_u32_t    memsize = node_count * sizeof(gfx_model_node_t);
    res->model.data     = vm_malloc(vm, memsize);
    NU_CHECK(res->model.data != ADDR_INVALID, return ID_NULL);
    res->model.node_count = node_count;
    os_gpu_init_model(vm, id);
    return id;
}
nu_status_t
sys_write_model (vm_t           *vm,
                 nu_u32_t        id,
                 nu_u32_t        node_index,
                 nu_u32_t        mesh,
                 nu_u32_t        texture,
                 nu_u32_t        parent,
                 const nu_f32_t *transform)
{
    NU_CHECK(id, return NU_FAILURE);
    resource_t *res = vm->res + ID_TO_INDEX(id);
    NU_CHECK(res->type == RES_MODEL, return NU_FAILURE);
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
    os_gpu_update_model(vm, id, node_index, &node);
    return NU_SUCCESS;
}

nu_u32_t
sys_add_spritesheet (vm_t    *vm,
                     nu_u32_t texture,
                     nu_u32_t row,
                     nu_u32_t col,
                     nu_u32_t fwidth,
                     nu_u32_t fheight)
{
    nu_u32_t id = vm_add_res(vm, RES_SPRITESHEET);
    NU_CHECK(id, return ID_NULL);
    resource_t *res          = vm->res + ID_TO_INDEX(id);
    res->spritesheet.texture = texture;
    res->spritesheet.row     = row;
    res->spritesheet.col     = col;
    res->spritesheet.fwidth  = fwidth;
    res->spritesheet.fheight = fheight;
    return id;
}

void
sys_transform (vm_t *vm, sys_transform_t transform, const nu_f32_t *m)
{
    switch (transform)
    {
        case SYS_TRANSFORM_MODEL:
            vm->gfx.state.model = nu_m4(m);
            break;
        case SYS_TRANSFORM_VIEW:
            vm->gfx.state.view = nu_m4(m);
            break;
        case SYS_TRANSFORM_PROJECTION:
            vm->gfx.state.projection = nu_m4(m);
            break;
    }
}
void
sys_cursor (vm_t *vm, nu_u32_t x, nu_u32_t y)
{
    vm->gfx.state.cursor = nu_v2u(x, y);
}
void
sys_fog_params (vm_t *vm, const nu_f32_t *params)
{
    vm->gfx.state.fog_density = nu_fabs(params[0]);
    vm->gfx.state.fog_near    = nu_fabs(params[1]);
    vm->gfx.state.fog_far     = NU_MAX(vm->gfx.state.fog_near, params[2]);
}
void
sys_fog_color (vm_t *vm, nu_u32_t color)
{
    vm->gfx.state.fog_color = nu_color_from_u32(color);
}
void
sys_clear (vm_t *vm, nu_u32_t color)
{
    os_gpu_clear(vm, color);
}
void
sys_color (vm_t *vm, nu_u32_t color)
{
    vm->gfx.state.color = nu_color_from_u32(color);
}
void
sys_draw (vm_t *vm, nu_u32_t id)
{
    NU_ASSERT(id);
    NU_CHECK(id && vm->res[ID_TO_INDEX(id)].type == RES_MODEL, return);
    os_gpu_draw_model(vm, id);
}
void
sys_draw_cube (vm_t *vm, const nu_f32_t *p, const nu_f32_t *s)
{
    os_gpu_draw_cube(vm, p, s);
}
void
sys_draw_lines (vm_t *vm, const nu_f32_t *p, nu_u32_t n)
{
    os_gpu_draw_lines(vm, p, n, NU_FALSE);
}
void
sys_draw_linestrip (vm_t *vm, const nu_f32_t *p, nu_u32_t n)
{
    os_gpu_draw_lines(vm, p, n, NU_TRUE);
}
void
sys_text (vm_t *vm, const void *text)
{
    os_gpu_draw_text(vm, text, nu_strlen(text));
}
void
sys_print (vm_t *vm, const void *text)
{
    sys_text(vm, text);
    vm->gfx.state.cursor.y += 9;
}
void
sys_blit (vm_t *vm, nu_u32_t id, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    NU_ASSERT(id);
    NU_CHECK(id && vm->res[ID_TO_INDEX(id)].type == RES_TEXTURE, return);
    os_gpu_draw_blit(vm, id, x, y, w, h);
}
void
sys_sprite (vm_t *vm, nu_u32_t spritesheet, nu_u32_t sprite)
{
    NU_ASSERT(spritesheet);
    NU_CHECK(spritesheet, return);
    resource_t *res = vm->res + ID_TO_INDEX(spritesheet);
    nu_u32_t    x   = (sprite % res->spritesheet.row) * res->spritesheet.fwidth;
    nu_u32_t    y = (sprite / res->spritesheet.row) * res->spritesheet.fheight;
    os_gpu_draw_blit(vm,
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
