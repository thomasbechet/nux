#ifndef SYSCALL_H
#define SYSCALL_H

#include <nulib/nulib.h>

typedef struct vm vm_t;

typedef enum
{
    // 16:9
    // SYS_SCREEN_WIDTH  = 1920,
    // SYS_SCREEN_HEIGHT = 1080,

    // 16:10, too high ?
    // SYS_SCREEN_WIDTH  = 640,
    // SYS_SCREEN_HEIGHT = 400,

    // 4:3
    // SYS_SCREEN_WIDTH  = 640,
    // SYS_SCREEN_HEIGHT = 480,

    // 16:10
    // SYS_SCREEN_WIDTH  = 512,
    // SYS_SCREEN_HEIGHT = 320,

    // 16:10
    // SYS_SCREEN_WIDTH  = 480,
    // SYS_SCREEN_HEIGHT = 300,

    // 4:3
    SYS_SCREEN_WIDTH  = 480,
    SYS_SCREEN_HEIGHT = 360,

    // 16:10
    // SYS_SCREEN_WIDTH  = 360,
    // SYS_SCREEN_HEIGHT = 225,

    // 4:3
    // SYS_SCREEN_WIDTH  = 320,
    // SYS_SCREEN_HEIGHT = 240

    SYS_MIN_TEXTURE_SIZE   = 32,
    SYS_MAX_TEXTURE_SIZE   = 256,
    SYS_MAX_RESOURCE_COUNT = 1024,
    SYS_MAX_PLAYER_COUNT   = 4,
} sys_constants_t;

typedef enum
{
    SYS_VERTEX_POSITION = 1 << 0,
    SYS_VERTEX_UV       = 1 << 1,
    SYS_VERTEX_COLOR    = 1 << 2,
    SYS_VERTEX_INDICES  = 1 << 3,
} sys_vertex_attribute_t;

typedef enum
{
    SYS_PRIMITIVE_TRIANGLES = 0,
    SYS_PRIMITIVE_LINES     = 1,
    SYS_PRIMITIVE_POINTS    = 2,
} sys_primitive_t;

typedef enum
{
    SYS_BUTTON_A = 1 << 0,
    SYS_BUTTON_X = 1 << 1,
    SYS_BUTTON_Y = 1 << 2,
    SYS_BUTTON_B = 1 << 3,

    SYS_BUTTON_UP    = 1 << 4,
    SYS_BUTTON_DOWN  = 1 << 5,
    SYS_BUTTON_LEFT  = 1 << 6,
    SYS_BUTTON_RIGHT = 1 << 7,

    SYS_BUTTON_LB = 1 << 8,
    SYS_BUTTON_RB = 1 << 9,

    SYS_BUTTON_COUNT = 10,
} sys_button_t;

typedef enum
{
    SYS_AXIS_LEFTX = 0,
    SYS_AXIS_LEFTY = 1,

    SYS_AXIS_RIGHTX = 2,
    SYS_AXIS_RIGHTY = 3,

    SYS_AXIS_RT = 4,
    SYS_AXIS_LT = 5,

    SYS_AXIS_COUNT = 6
} sys_axis_t;

typedef enum
{
    SYS_CONSOLE_MEMORY_CAPACITY = 0,
    SYS_CONSOLE_MEMORY_USAGE    = 1,
} sys_console_info_t;

typedef enum
{
    SYS_RENDER_VIEWPORT    = 0,
    SYS_RENDER_SCISSOR     = 1,
    SYS_RENDER_TRANSFORM   = 2,
    SYS_RENDER_VIEW        = 3,
    SYS_RENDER_PROJECTION  = 4,
    SYS_RENDER_CURSOR      = 5,
    SYS_RENDER_FOG_COLOR   = 6,
    SYS_RENDER_FOG_DENSITY = 7,
    SYS_RENDER_FOG_NEAR    = 8,
    SYS_RENDER_FOG_FAR     = 9,
    SYS_RENDER_COLOR       = 10
} sys_render_state_t;

typedef enum
{
    SYS_INSPECT_I32 = 0,
    SYS_INSPECT_F32 = 1,
} sys_inspect_type_t;

void     sys_trace(vm_t *vm, const nu_char_t *text);
void     sys_inspect_i32(vm_t *vm, const nu_char_t *name, nu_i32_t *p);
void     sys_inspect_f32(vm_t *vm, const nu_char_t *name, nu_f32_t *p);
nu_u32_t sys_console_info(vm_t *vm, sys_console_info_t info);
nu_f32_t sys_global_time(vm_t *vm);
nu_f32_t sys_delta_time(vm_t *vm);

nu_status_t sys_create_scope(vm_t *vm, nu_u32_t id, nu_u32_t size);
nu_status_t sys_rewind_scope(vm_t *vm, nu_u32_t id);
void        sys_set_active_scope(vm_t *vm, nu_u32_t id);

nu_status_t sys_create_texture(vm_t *vm, nu_u32_t id, nu_u32_t size);
nu_status_t sys_update_texture(vm_t       *vm,
                               nu_u32_t    id,
                               nu_u32_t    x,
                               nu_u32_t    y,
                               nu_u32_t    w,
                               nu_u32_t    h,
                               const void *p);

nu_status_t sys_create_mesh(vm_t                  *vm,
                            nu_u32_t               id,
                            nu_u32_t               count,
                            sys_primitive_t        primitive,
                            sys_vertex_attribute_t attribs);
nu_status_t sys_update_mesh(vm_t                  *vm,
                            nu_u32_t               id,
                            sys_vertex_attribute_t attribs,
                            nu_u32_t               first,
                            nu_u32_t               count,
                            const void            *p);

nu_status_t sys_create_model(vm_t *vm, nu_u32_t id, nu_u32_t count);
nu_status_t sys_update_model(vm_t           *vm,
                             nu_u32_t        id,
                             nu_u32_t        node,
                             nu_u32_t        mesh,
                             nu_u32_t        texture,
                             nu_u32_t        parent,
                             const nu_f32_t *transform);

nu_status_t sys_create_spritesheet(vm_t    *vm,
                                   nu_u32_t id,
                                   nu_u32_t texture,
                                   nu_u32_t row,
                                   nu_u32_t col,
                                   nu_u32_t fwidth,
                                   nu_u32_t fheight);

void sys_set_render_state(vm_t *vm, sys_render_state_t state, const void *p);
void sys_get_render_state(vm_t *vm, sys_render_state_t state, void *p);
void sys_set_scissor(vm_t *vm, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);
void sys_set_viewport(vm_t *vm, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);
void sys_set_view(vm_t *vm, const nu_f32_t *m);
void sys_set_projection(vm_t *vm, const nu_f32_t *m);
void sys_set_transform(vm_t *vm, const nu_f32_t *m);
void sys_set_cursor(vm_t *vm, nu_u32_t x, nu_u32_t y);
void sys_set_fog_near(vm_t *vm, nu_f32_t near);
void sys_set_fog_far(vm_t *vm, nu_f32_t far);
void sys_set_fog_density(vm_t *vm, nu_f32_t density);
void sys_set_fog_color(vm_t *vm, nu_u32_t color);
void sys_set_color(vm_t *vm, nu_u32_t color);

void sys_clear(vm_t *vm, nu_u32_t color);
void sys_draw_model(vm_t *vm, nu_u32_t id);
void sys_draw_volume(vm_t *vm);
void sys_draw_cube(vm_t *vm, const nu_f32_t *c, const nu_f32_t *s);
void sys_draw_lines(vm_t *vm, const nu_f32_t *p, nu_u32_t n);
void sys_draw_linestrip(vm_t *vm, const nu_f32_t *p, nu_u32_t n);
void sys_draw_text(vm_t *vm, const void *text);
void sys_print(vm_t *vm, const void *text);
void sys_blit(
    vm_t *vm, nu_u32_t id, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);
void sys_draw_sprite(vm_t *vm, nu_u32_t spritesheet, nu_u32_t sprite);

nu_u32_t sys_button(vm_t *vm, nu_u32_t player);
nu_f32_t sys_axis(vm_t *vm, nu_u32_t player, sys_axis_t axis);

#endif
