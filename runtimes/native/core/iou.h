#ifndef IOU_H
#define IOU_H

#include "shared.h"

/////////////////
///    IOU    ///
/////////////////

#define IOU_MEM_SIZE                NU_MEM_1M
#define CART_CHUNK_MESH_HEADER_SIZE sizeof(nu_u32_t) * 4

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

typedef struct
{
    cart_header_t header;
    void         *heap;
} iou_t;

nu_status_t iou_init(vm_t *vm);
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

#endif
