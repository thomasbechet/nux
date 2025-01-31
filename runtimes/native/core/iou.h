#ifndef IOU_H
#define IOU_H

#include "shared.h"

/////////////////
///    IOU    ///
/////////////////

#define IOU_MEM_SIZE                NU_MEM_1M
#define CART_CHUNK_MESH_HEADER_SIZE sizeof(nu_u32_t) * 4
#define CONTROLLER_MAX_PLAYER       4

typedef enum
{
    CART_CHUNK_RAW     = 0,
    CART_CHUNK_WASM    = 1,
    CART_CHUNK_TEXTURE = 2,
    CART_CHUNK_MESH    = 3,
    CART_CHUNK_MODEL   = 4,
} cart_chunk_type_t;

typedef struct
{
    cart_chunk_type_t type;
    nu_u32_t          length;
} cart_chunk_header_t;

typedef struct
{
    nu_u32_t version;
} cart_header_t;

typedef enum
{
    CONTROLLER_BUTTON_A  = 1 << 0,
    CONTROLLER_BUTTON_X  = 1 << 1,
    CONTROLLER_BUTTON_Y  = 1 << 2,
    CONTROLLER_BUTTON_B  = 1 << 3,
    CONTROLLER_BUTTON_LB = 1 << 4,
    CONTROLLER_BUTTON_RB = 1 << 5,
} controller_button_t;

typedef enum
{
    CONTROLLER_AXIS_LEFTX  = 0,
    CONTROLLER_AXIS_LEFTY  = 1,
    CONTROLLER_AXIS_RIGHTX = 2,
    CONTROLLER_AXIS_RIGHTY = 3,
    CONTROLLER_AXIS_MAX    = 4
} controller_axis_t;

typedef struct
{
    cart_header_t header;
    void         *heap;
    nu_u32_t      buttons[CONTROLLER_MAX_PLAYER];
    nu_f32_t      axis[CONTROLLER_MAX_PLAYER][CONTROLLER_AXIS_MAX];
} iou_t;

nu_status_t iou_init(vm_t *vm);
void        iou_update(vm_t *vm);
nu_status_t iou_load_cart(vm_t *vm, const nu_char_t *name);
nu_size_t   iou_read(vm_t *vm, void *p, nu_size_t n);
nu_status_t iou_read_u32(vm_t *vm, nu_u32_t *v);
nu_status_t iou_read_f32(vm_t *vm, nu_f32_t *v);
nu_status_t iou_read_m4(vm_t *vm, nu_m4_t *v);

void iou_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void iou_vlog(vm_t            *vm,
              nu_log_level_t   level,
              const nu_char_t *fmt,
              va_list          args);

nu_u32_t iou_button(vm_t *vm, nu_u32_t player);
nu_f32_t iou_axis(vm_t *vm, nu_u32_t player, controller_axis_t axis);

#endif
