#ifndef NUX_H
#define NUX_H

#include "nux_config.h"
#include "nux_api.h"

//////////////////////////////////////////////////////////////////////////
//////                               Types                          //////
//////////////////////////////////////////////////////////////////////////

#define NUX_CART_HEADER_SIZE       sizeof(nux_cart_header_t)
#define NUX_CART_OBJECT_ENTRY_SIZE (sizeof(nu_u32_t) * 4)

typedef struct nux_instance *nux_instance_t;
typedef nux_u32_t            nux_ptr_t;

typedef struct
{
    void     *userdata;
    nux_u32_t command_buffer_capacity;
} nux_instance_config_t;

typedef enum
{
    NUX_NODE_DIRTY            = 1 << 0,
    NUX_NODE_CACHED_TRANSFORM = 1 << 1,
} nux_node_flags_t;

typedef enum
{
    NUX_COMMAND_PUSH_VIEWPORT,
    NUX_COMMAND_PUSH_SCISSOR,
    NUX_COMMAND_PUSH_CURSOR,
    NUX_COMMAND_PUSH_COLOR,
    NUX_COMMAND_CLEAR,
    NUX_COMMAND_DRAW_SCENE,
    NUX_COMMAND_DRAW_CUBE,
    NUX_COMMAND_DRAW_LINES,
    NUX_COMMAND_DRAW_LINESTRIP,
    NUX_COMMAND_DRAW_TEXT,
    NUX_COMMAND_BLIT,
} nux_command_type_t;

typedef struct
{
    nux_command_type_t type;
    union
    {
        nux_u32_t clear;
        nux_u32_t scissor[4];
        nux_u32_t viewport[4];
        nux_u32_t cursor[2];
        nux_u32_t color;
        struct
        {
            nux_oid_t scene;
            nux_nid_t camera;
        } draw_scene;
    };
} nux_command_t;

typedef enum
{
    NUX_INSPECT_I32,
    NUX_INSPECT_F32,
} nux_inspect_type_t;

typedef enum
{
    NUX_MEMORY_USAGE_STATE,
    NUX_MEMORY_USAGE_CORE,
} nux_memory_usage_t;

typedef struct
{
    nux_object_type_t type;
    nux_oid_t         oid;
    nux_u32_t         offset;
    nux_u32_t         length;
} nux_cart_object_entry_t;

typedef struct
{
    nux_u32_t version;
    nux_u32_t object_count;
} nux_cart_header_t;

typedef struct
{
    nux_u32_t size;           // 4
    nux_ptr_t data;           // 4
    nux_u32_t update_counter; // 4
} nux_texture_t;              // 12 bytes

typedef struct
{
    nux_u32_t size; // 4
    nux_ptr_t data; // 4
} nux_pool_t;       // 8 bytes

typedef struct
{
    nux_u32_t              count;          // 4
    nux_primitive_t        primitive;      // 4
    nux_vertex_attribute_t attributes;     // 4
    nux_ptr_t              data;           // 4
    nux_u32_t              update_counter; // 4
} nux_mesh_t;                              // 20 bytes

typedef struct
{
    nux_oid_t texture; // 4
    nux_u16_t row;     // 2
    nux_u16_t col;     // 2
    nux_u16_t fwidth;  // 2
    nux_u16_t fheight; // 2
} nux_spritesheet_t;   // 12 bytes

typedef struct
{
    nux_u32_t free;  // 4
    nux_u32_t nodes; // 4
    nux_u32_t size;  // 4
    nux_u32_t capa;  // 4
} nux_scene_t;       // 16 bytes

typedef struct
{
    nux_object_type_t type; // 4
    nux_oid_t         next; // 4
    union
    {
        nux_pool_t        pool;
        nux_texture_t     texture;
        nux_mesh_t        mesh;
        nux_spritesheet_t spritesheet;
        nux_scene_t       scene;
    };
} nux_object_t; // max 32 bytes

typedef struct
{
    nux_u32_t mask;           // 4
    nux_u32_t flags;          // 4
    nux_nid_t parent;         // 2
    nux_nid_t child;          // 2
    nux_nid_t next;           // 2
    nux_nid_t prev;           // 2
    nux_nid_t table;          // 2
    nux_nid_t nid;            // 2 (store nid for validation)
    nux_f32_t translation[3]; // 12
    nux_f32_t rotation[4];    // 16
    nux_f32_t scale[3];       // 12
    nux_u32_t _pad1;          // 4 (TODO: reference cached transform node ?)
} nux_node_t;                 // 64 bytes

typedef struct
{
    nux_f32_t fov;  // 4
    nux_f32_t near; // 4
    nux_f32_t far;  // 4
} nux_camera_t;

typedef struct
{
    nux_oid_t mesh;    // 4
    nux_oid_t texture; // 4
    nux_u32_t visible; // 4
} nux_model_t;

typedef struct
{
    nux_u32_t color;
} nux_light_t;

typedef union
{
    nux_camera_t camera;
    nux_model_t  model;
    nux_light_t  light;
} nux_component_t; // 64 bytes max

typedef struct
{
    nux_nid_t indices[32]; // 64 bytes
} nux_node_table_t;

typedef union // max 64 bytes
{
    nux_node_t       node;                 // 64
    nux_f32_t        cached_transform[16]; // 64
    nux_node_table_t table;                // 64
    nux_nid_t        free;                 // 2
    nux_component_t  component;            // 64
} nux_scene_node_t;

//////////////////////////////////////////////////////////////////////////
//////                       Platform Callbacks                     //////
//////////////////////////////////////////////////////////////////////////

NUX_API void     *nux_platform_malloc(void              *userdata,
                                      nux_memory_usage_t usage,
                                      nux_u32_t          n);
NUX_API void      nux_platform_free(void *userdata, void *p);
NUX_API void     *nux_platform_realloc(void *userdata, void *p, nux_u32_t n);
NUX_API void      nux_platform_mount(nux_instance_t  inst,
                                     const nux_c8_t *name,
                                     nux_u32_t       n);
NUX_API void      nux_platform_seek(nux_instance_t inst, nux_u32_t n);
NUX_API nux_u32_t nux_platform_read(nux_instance_t inst, void *p, nux_u32_t n);
NUX_API void      nux_platform_log(nux_instance_t  inst,
                                   const nux_c8_t *log,
                                   nux_u32_t       n);
NUX_API void      nux_platform_inspect(nux_instance_t     inst,
                                       const nux_c8_t    *name,
                                       nux_u32_t          n,
                                       nux_inspect_type_t type,
                                       void              *p);

//////////////////////////////////////////////////////////////////////////
//////                          Instance API                        //////
//////////////////////////////////////////////////////////////////////////

NUX_API nux_instance_t nux_instance_init(const nux_instance_config_t *config);
NUX_API void           nux_instance_free(nux_instance_t inst);
NUX_API void           nux_instance_tick(nux_instance_t inst);
NUX_API nux_status_t   nux_instance_load(nux_instance_t  inst,
                                         const nux_c8_t *cart,
                                         nux_u32_t       n);
NUX_API void  nux_instance_save_state(nux_instance_t inst, nux_u8_t *state);
NUX_API void  nux_instance_load_state(nux_instance_t  inst,
                                      const nux_u8_t *state);
NUX_API void *nux_instance_get_userdata(nux_instance_t inst);
NUX_API void  nux_instance_set_buttons(nux_instance_t inst,
                                       nux_u32_t      player,
                                       nux_u32_t      state);
NUX_API void  nux_instance_set_axis(nux_instance_t inst,
                                    nux_u32_t      player,
                                    nux_axis_t     axis,
                                    nux_f32_t      value);
NUX_API nux_object_t *nux_instance_get_object(nux_instance_t    inst,
                                              nux_object_type_t type,
                                              nux_oid_t         oid);
NUX_API void *nux_instance_get_memory(nux_instance_t inst, nux_ptr_t ptr);
NUX_API nux_command_t *nux_instance_get_command_buffer(nux_instance_t inst,
                                                       nux_u32_t     *count);
NUX_API nux_env_t      nux_instance_init_env(nux_instance_t inst);

//////////////////////////////////////////////////////////////////////////
//////                           Helper API                         //////
//////////////////////////////////////////////////////////////////////////

NUX_API nux_u32_t        nux_texture_memsize(nux_u32_t size);
NUX_API nux_u32_t        nux_vertex_memsize(nux_vertex_attribute_t attributes,
                                            nux_u32_t              count);
NUX_API nux_u32_t        nux_vertex_offset(nux_vertex_attribute_t attributes,
                                           nux_vertex_attribute_t attribute,
                                           nux_u32_t              count);
NUX_API const nux_c8_t  *nux_error_message(nux_error_t error);
NUX_API nux_nid_t        nux_scene_iter_dfs(const nux_scene_node_t *nodes,
                                            nux_nid_t             **iter,
                                            nux_nid_t              *stack,
                                            nux_u32_t               stack_size);
NUX_API nux_component_t *nux_scene_get_component(
    nux_scene_node_t *nodes, nux_nid_t node, nux_component_type_t component);
NUX_API nux_status_t nux_cart_parse_header(const void        *data,
                                           nux_cart_header_t *header);
NUX_API nux_status_t nux_cart_parse_entries(const void              *data,
                                            nux_u32_t                count,
                                            nux_cart_object_entry_t *entries);

#endif
