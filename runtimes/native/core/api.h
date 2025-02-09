#ifndef CORE_SHARED_H
#define CORE_SHARED_H

#include <nulib/nulib.h>

typedef struct vm vm_t;

void api_trace(vm_t *vm, const nu_char_t *text);

typedef enum
{
    API_VERTEX_POSITION = 1 << 0,
    API_VERTEX_UV       = 1 << 1,
    API_VERTEX_COLOR    = 1 << 2,
    API_VERTEX_INDICES  = 1 << 3,
} api_vertex_attribute_t;

typedef enum
{
    API_PRIMITIVE_TRIANGLES = 0,
    API_PRIMITIVE_LINES     = 1,
    API_PRIMITIVE_POINTS    = 2,
} api_primitive_t;

typedef enum
{
    API_TRANSFORM_MODEL      = 0,
    API_TRANSFORM_VIEW       = 1,
    API_TRANSFORM_PROJECTION = 2,
} api_transform_t;

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
                          api_primitive_t        primitive,
                          api_vertex_attribute_t attribs);
nu_status_t api_writemesh(vm_t                  *vm,
                          nu_u32_t               idx,
                          api_vertex_attribute_t attribs,
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

void api_transform(vm_t *vm, api_transform_t transform, const nu_f32_t *m);
void api_cursor(vm_t *vm, nu_u32_t x, nu_u32_t y);
void api_fogcolor(vm_t *vm, nu_u32_t color);
void api_fogdensity(vm_t *vm, nu_f32_t density);
void api_fogrange(vm_t *vm, nu_f32_t near, nu_f32_t far);
void api_clear(vm_t *vm, nu_u32_t color);
void api_draw(vm_t *vm, nu_u32_t index);
void api_text(vm_t *vm, const void *text);
void api_print(vm_t *vm, const void *text);
void api_blit(
    vm_t *vm, nu_u32_t index, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h);

typedef enum
{
    API_BUTTON_A  = 1 << 0,
    API_BUTTON_X  = 1 << 1,
    API_BUTTON_Y  = 1 << 2,
    API_BUTTON_B  = 1 << 3,
    API_BUTTON_LB = 1 << 4,
    API_BUTTON_RB = 1 << 5,
} api_button_t;

typedef enum
{
    API_AXIS_LEFTX  = 0,
    API_AXIS_LEFTY  = 1,
    API_AXIS_RIGHTX = 2,
    API_AXIS_RIGHTY = 3,
    API_AXIS_MAX    = 4
} api_axis_t;

nu_u32_t api_button(vm_t *vm, nu_u32_t player);
nu_f32_t api_axis(vm_t *vm, nu_u32_t player, api_axis_t axis);

#endif
