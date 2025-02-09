#ifndef CORE_SHARED_H
#define CORE_SHARED_H

#include <nulib/nulib.h>

typedef struct vm vm_t;

void api_trace(vm_t *vm, const nu_char_t *text);

typedef enum
{
    GPU_VERTEX_POSITION = 1 << 0,
    GPU_VERTEX_UV       = 1 << 1,
    GPU_VERTEX_COLOR    = 1 << 2,
    GPU_VERTEX_INDICES  = 1 << 3,
} gpu_vertex_attribute_t;

typedef enum
{
    GPU_PRIMITIVE_TRIANGLES = 0,
    GPU_PRIMITIVE_LINES     = 1,
    GPU_PRIMITIVE_POINTS    = 2,
} gpu_primitive_t;

typedef enum
{
    GPU_TRANSFORM_MODEL      = 0,
    GPU_TRANSFORM_VIEW       = 1,
    GPU_TRANSFORM_PROJECTION = 2,
} gpu_transform_t;

nu_status_t api_allocgpool(vm_t *vm, nu_u32_t index, nu_u32_t size);
nu_status_t api_gpool(vm_t *vm, nu_u32_t index);
nu_status_t api_cleargpool(vm_t *vm, nu_u32_t index);

nu_status_t api_alloctex(vm_t *vm, nu_u32_t idx, nu_u32_t size);
nu_status_t api_writetex(vm_t       *vm,
                         nu_u32_t    idx,
                         nu_u32_t    x,
                         nu_u32_t    y,
                         nu_u32_t    w,
                         nu_u32_t    h,
                         const void *p);

nu_status_t api_allocmesh(vm_t                  *vm,
                          nu_u32_t               idx,
                          nu_u32_t               count,
                          gpu_primitive_t        primitive,
                          gpu_vertex_attribute_t attribs);
nu_status_t api_writemesh(vm_t                  *vm,
                          nu_u32_t               idx,
                          gpu_vertex_attribute_t attribs,
                          nu_u32_t               first,
                          nu_u32_t               count,
                          const void            *p);

nu_status_t api_allocmodel(vm_t *vm, nu_u32_t idx, nu_u32_t count);
nu_status_t api_writemodel(vm_t           *vm,
                           nu_u32_t        idx,
                           nu_u32_t        node,
                           nu_u32_t        mesh,
                           nu_u32_t        texture,
                           nu_u32_t        parent,
                           const nu_f32_t *transform);

void api_transform(vm_t *vm, gpu_transform_t transform, const nu_f32_t *m);
void api_cursor(vm_t *vm, nu_u32_t x, nu_u32_t y);
void api_fogcolor(vm_t *vm, nu_u32_t color);
void api_fogdensity(vm_t *vm, nu_f32_t density);
void api_clear(vm_t *vm, nu_u32_t color);
void api_draw(vm_t *vm, nu_u32_t index);
void api_text(vm_t *vm, const void *text);
void api_print(vm_t *vm, const void *text);
void api_blit(
    vm_t *vm, nu_u32_t index, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);

typedef enum
{
    GAMEPAD_BUTTON_A  = 1 << 0,
    GAMEPAD_BUTTON_X  = 1 << 1,
    GAMEPAD_BUTTON_Y  = 1 << 2,
    GAMEPAD_BUTTON_B  = 1 << 3,
    GAMEPAD_BUTTON_LB = 1 << 4,
    GAMEPAD_BUTTON_RB = 1 << 5,
} gamepad_button_t;

typedef enum
{
    GAMEPAD_AXIS_LEFTX  = 0,
    GAMEPAD_AXIS_LEFTY  = 1,
    GAMEPAD_AXIS_RIGHTX = 2,
    GAMEPAD_AXIS_RIGHTY = 3,
    GAMEPAD_AXIS_MAX    = 4
} gamepad_axis_t;

nu_u32_t api_button(vm_t *vm, nu_u32_t player);
nu_f32_t api_axis(vm_t *vm, nu_u32_t player, gamepad_axis_t axis);

#endif
