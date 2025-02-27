#ifndef VM_H
#define VM_H

#include "syscall.h"

#define VM_VERSION_MAJOR(num)          ((num >> 16) & 0xFF)
#define VM_VERSION_MINOR(num)          ((num >> 8) & 0xFF)
#define VM_VERSION_PATCH(num)          (num & 0xFF)
#define VM_VERSION_MAKE(maj, min, pat) (maj << 16 | min << 8 | pat)
#define VM_VERSION                     VM_VERSION_MAKE(0, 0, 1)

#define CART_HEADER_SIZE      sizeof(cart_header_t)
#define CART_CHUNK_ENTRY_SIZE (sizeof(nu_u32_t) * 4)

#define ID_NULL      (0)
#define ADDR_INVALID (nu_u32_t)(0xFFFFFFFF)

typedef enum
{
    RESOURCE_FREE = 0,
    RESOURCE_NULL,
    RESOURCE_POOL,
    RESOURCE_TEXTURE,
    RESOURCE_MESH,
    RESOURCE_MODEL,
    RESOURCE_SPRITESHEET,
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
    nu_u32_t          id;
    nu_u32_t          offset;
    nu_u32_t          length;
} cart_chunk_entry_t;

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} cart_header_t;

typedef struct
{
    nu_v4u_t   scissor;
    nu_v4u_t   viewport;
    nu_m4_t    model;
    nu_m4_t    view;
    nu_m4_t    projection;
    nu_v2u_t   cursor;
    nu_color_t fog_color;
    nu_f32_t   fog_density;
    nu_f32_t   fog_near;
    nu_f32_t   fog_far;
    nu_color_t color;
} gfx_state_t;

typedef struct
{
    nu_u32_t texture;
    nu_u32_t mesh;
    nu_m4_t  local_to_parent;
    nu_u32_t parent;
} gfx_model_node_t;

typedef struct
{
    gfx_state_t state;
} gfx_t;

typedef enum
{
    WASM_EVENT_START,
    WASM_EVENT_UPDATE,
} wasm_event_t;

typedef struct
{
    nu_bool_t loaded;
} wasm_t;

#define MAX_PLAYER 4

typedef struct
{
    nu_u32_t buttons[MAX_PLAYER];
    nu_f32_t axis[MAX_PLAYER][SYS_AXIS_ENUM_MAX];
} gamepad_t;

nu_status_t gpad_init(vm_t *vm);
void        gpad_update(vm_t *vm);

struct vm
{
    wasm_t    wasm;
    gfx_t     gfx;
    gamepad_t gamepad;
    nu_bool_t running;

    nu_f32_t time;
    nu_u32_t tps;

    nu_u32_t active_pool;

    resource_t res[SYS_MAX_RESOURCE_COUNT];

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

nu_status_t bios_load_cart(vm_t *vm, const nu_char_t *name);

nu_size_t   cart_read(vm_t *vm, void *p, nu_size_t n);
nu_status_t cart_read_u32(vm_t *vm, nu_u32_t *v);
nu_status_t cart_read_f32(vm_t *vm, nu_f32_t *v);
nu_status_t cart_read_m4(vm_t *vm, nu_m4_t *v);

nu_status_t cart_parse_header(const void *data, cart_header_t *header);
nu_status_t cart_parse_entries(const void         *data,
                               nu_u32_t            count,
                               cart_chunk_entry_t *entries);

nu_status_t gfx_init(vm_t *vm);
nu_status_t gfx_free(vm_t *vm);
void        gfx_begin_frame(vm_t *vm);
void        gfx_end_frame(vm_t *vm);

nu_u32_t gfx_texture_memsize(nu_u32_t size);
nu_u32_t gfx_vertex_memsize(sys_vertex_attribute_t attributes, nu_u32_t count);
nu_u32_t gfx_vertex_offset(sys_vertex_attribute_t attributes,
                           sys_vertex_attribute_t attribute,
                           nu_u32_t               count);

nu_status_t wasm_init(vm_t *vm);
void        wasm_free(vm_t *vm);
nu_status_t wasm_call_event(vm_t *vm, wasm_event_t event);

#endif
