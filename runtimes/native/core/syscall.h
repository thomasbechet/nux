#ifndef SYSCALL_H
#define SYSCALL_H

#include <nulib/nulib.h>

typedef struct vm vm_t;

void sys_trace(vm_t *vm, const nu_char_t *text);

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
    SYS_TRANSFORM_MODEL      = 0,
    SYS_TRANSFORM_VIEW       = 1,
    SYS_TRANSFORM_PROJECTION = 2,
} sys_transform_t;

nu_status_t sys_allocgpool(vm_t *vm, nu_u32_t index, nu_u32_t size);
nu_status_t sys_gpool(vm_t *vm, nu_u32_t index);
nu_status_t sys_cleargpool(vm_t *vm, nu_u32_t index);

nu_status_t sys_alloctex(vm_t *vm, nu_u32_t idx, nu_u32_t size);
nu_status_t sys_writetex(vm_t       *vm,
                         nu_u32_t    idx,
                         nu_u32_t    x,
                         nu_u32_t    y,
                         nu_u32_t    w,
                         nu_u32_t    h,
                         const void *p);

nu_status_t sys_allocmesh(vm_t                  *vm,
                          nu_u32_t               idx,
                          nu_u32_t               count,
                          sys_primitive_t        primitive,
                          sys_vertex_attribute_t attribs);
nu_status_t sys_writemesh(vm_t                  *vm,
                          nu_u32_t               idx,
                          sys_vertex_attribute_t attribs,
                          nu_u32_t               first,
                          nu_u32_t               count,
                          const void            *p);

nu_status_t sys_allocmodel(vm_t *vm, nu_u32_t idx, nu_u32_t count);
nu_status_t sys_writemodel(vm_t           *vm,
                           nu_u32_t        idx,
                           nu_u32_t        node,
                           nu_u32_t        mesh,
                           nu_u32_t        texture,
                           nu_u32_t        parent,
                           const nu_f32_t *transform);

void sys_transform(vm_t *vm, sys_transform_t transform, const nu_f32_t *m);
void sys_cursor(vm_t *vm, nu_u32_t x, nu_u32_t y);
void sys_fogparams(vm_t *vm, const nu_f32_t *params);
void sys_fogcolor(vm_t *vm, nu_u32_t color);
void sys_clear(vm_t *vm, nu_u32_t color);
void sys_color(vm_t *vm, nu_u32_t color);
void sys_draw(vm_t *vm, nu_u32_t index);
void sys_drawc(vm_t *vm, const nu_f32_t *c, const nu_f32_t *s);
void sys_drawl(vm_t *vm, const nu_f32_t *p, nu_u32_t n);
void sys_drawls(vm_t *vm, const nu_f32_t *p, nu_u32_t n);
void sys_text(vm_t *vm, const void *text);
void sys_print(vm_t *vm, const void *text);
void sys_blit(
    vm_t *vm, nu_u32_t index, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);

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
} sys_button_t;

typedef enum
{
    SYS_AXIS_LEFTX = 0,
    SYS_AXIS_LEFTY = 1,

    SYS_AXIS_RIGHTX = 2,
    SYS_AXIS_RIGHTY = 3,

    SYS_AXIS_RT = 4,
    SYS_AXIS_LT = 5,

    SYS_AXIS_ENUM_MAX = 6
} sys_axis_t;

nu_u32_t sys_button(vm_t *vm, nu_u32_t player);
nu_f32_t sys_axis(vm_t *vm, nu_u32_t player, sys_axis_t axis);

#endif
