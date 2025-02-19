#ifndef VM_H
#define VM_H

#include "gfx.h"
#include "wasm.h"
#include "gamepad.h"

#define VM_VERSION_MAJOR(num)          ((num >> 16) & 0xFF)
#define VM_VERSION_MINOR(num)          ((num >> 8) & 0xFF)
#define VM_VERSION_PATCH(num)          (num & 0xFF)
#define VM_VERSION_MAKE(maj, min, pat) (maj << 16 | min << 8 | pat)
#define VM_VERSION                     VM_VERSION_MAKE(0, 0, 1)

#define MAX_RESOURCE_COUNT 1024

#define ID_NULL      (0)
#define ADDR_INVALID (nu_u32_t)(0xFFFFFFFF)

typedef enum
{
    RES_FREE = 0,
    RES_NULL,
    RES_POOL,
    RES_TEXTURE,
    RES_MESH,
    RES_MODEL,
    RES_SPRITESHEET,
} resource_type_t;

typedef struct
{
    resource_type_t type;
    nu_u32_t        next;
    union
    {
        struct
        {
            nu_u32_t data;
        } pool;
        struct
        {
            nu_u32_t size;
            nu_u32_t data;
        } texture;
        struct
        {
            nu_u32_t               count;
            sys_primitive_t        primitive;
            sys_vertex_attribute_t attributes;
            nu_u32_t               data;
        } mesh;
        struct
        {
            nu_u32_t node_count;
            nu_u32_t data;
        } model;
        struct
        {
            nu_u32_t texture;
            nu_u16_t row;
            nu_u16_t col;
            nu_u16_t fwidth;
            nu_u16_t fheight;
        } spritesheet;
    };
} resource_t;

typedef struct
{
    nu_u32_t memsize;
    nu_u32_t tps;
} vm_config_t;

struct vm
{
    wasm_t    wasm;
    gfx_t     gfx;
    gamepad_t gamepad;
    nu_bool_t running;

    nu_f32_t time;
    nu_u32_t tps;

    nu_u32_t active_pool;

    resource_t res[MAX_RESOURCE_COUNT];

    nu_byte_t *mem;
    nu_u32_t   memsize;
    nu_u32_t   memcapa;
};

NU_API nu_status_t vm_init(vm_t *vm, const vm_config_t *config);
NU_API void        vm_free(vm_t *vm);
NU_API nu_status_t vm_load(vm_t *vm, const nu_char_t *name);
NU_API nu_status_t vm_tick(vm_t *vm);
NU_API void        vm_save_state(const vm_t *vm, void *state);
NU_API nu_status_t vm_load_state(vm_t *vm, const void *state);

NU_API void      vm_config_default(vm_config_t *config);
NU_API nu_size_t vm_config_state_memsize(const vm_config_t *config);

void     vm_log(vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...);
void     vm_vlog(vm_t            *vm,
                 nu_log_level_t   level,
                 const nu_char_t *fmt,
                 va_list          args);
nu_u32_t vm_malloc(vm_t *vm, nu_u32_t n);
resource_t *vm_set_res(vm_t *vm, nu_u32_t id, resource_type_t type);
resource_t *vm_get_res(vm_t *vm, nu_u32_t id, resource_type_t type);

#endif
