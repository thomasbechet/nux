#ifndef NUX_API_H
#define NUX_API_H

#ifdef NUX_INCLUDE_FIXED_TYPES
#else
typedef unsigned char nux_u8_t;
typedef char          nux_c8_t;
typedef int           nux_i16_t;
typedef unsigned int  nux_u16_t;
typedef int           nux_i32_t;
typedef unsigned int  nux_u32_t;
typedef long          nux_i64_t;
typedef unsigned long nux_u64_t;
typedef float         nux_f32_t;
typedef double        nux_f64_t;
#endif

typedef nux_u32_t nux_id_t;

typedef struct nux_env *nux_env_t;

typedef enum
{
    // 16:9
    // NUX_SCREEN_WIDTH  = 1920,
    // NUX_SCREEN_HEIGHT = 1080,

    // 16:10, too high ?
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 400,

    // 4:3
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 480,

    // 16:10
    // NUX_SCREEN_WIDTH  = 512,
    // NUX_SCREEN_HEIGHT = 320,

    // 16:10
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 300,

    // 4:3
    NUX_SCREEN_WIDTH  = 480,
    NUX_SCREEN_HEIGHT = 360,

    // 16:10
    // NUX_SCREEN_WIDTH  = 360,
    // NUX_SCREEN_HEIGHT = 225,

    // 4:3
    // NUX_SCREEN_WIDTH  = 320,
    // NUX_SCREEN_HEIGHT = 240

    NUX_TEXTURE_MIN_SIZE = 32,
    NUX_TEXTURE_MAX_SIZE = 256,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NODE_MAX         = (1 << 16) - 1,
    NUX_NAME_MAX         = 64,
} nux_constants_t;

typedef enum
{
    NUX_ERROR_NONE                    = 0,
    NUX_ERROR_OUT_OF_MEMORY           = 1,
    NUX_ERROR_OUT_OF_POOL_ITEM        = 2,
    NUX_ERROR_OUT_OF_COMMANDS         = 3,
    NUX_ERROR_OUT_OF_OBJECTS          = 12,
    NUX_ERROR_INVALID_TEXTURE_SIZE    = 4,
    NUX_ERROR_INVALID_OBJECT_ID       = 5,
    NUX_ERROR_INVALID_OBJECT_TYPE     = 6,
    NUX_ERROR_WASM_RUNTIME            = 8,
    NUX_ERROR_INVALID_OBJECT_CREATION = 9,
    NUX_ERROR_CART_EOF                = 10,
    NUX_ERROR_CART_MOUNT              = 11,
} nux_error_t;

typedef enum
{
    NUX_SUCCESS = 1,
    NUX_FAILURE = 0
} nux_status_t;

typedef enum
{
    NUX_OBJECT_MEMORY,
    NUX_OBJECT_FREE, // use for object pool
    NUX_OBJECT_STACK,
    NUX_OBJECT_POOL,

    NUX_OBJECT_WASM,
    NUX_OBJECT_RAW,
    NUX_OBJECT_TEXTURE,
    NUX_OBJECT_MESH,
    NUX_OBJECT_SPRITESHEET,
    NUX_OBJECT_SCENE,
    NUX_OBJECT_NODE,
    NUX_OBJECT_NODE_TRANSFORM,
    NUX_OBJECT_NODE_CAMERA,
    NUX_OBJECT_NODE_MODEL,

    NUX_OBJECT_TYPE_MAX = 11,
} nux_object_type_t;

typedef enum
{
    NUX_VERTEX_POSITION = 1 << 0,
    NUX_VERTEX_UV       = 1 << 1,
    NUX_VERTEX_COLOR    = 1 << 2,
    NUX_VERTEX_INDICES  = 1 << 3,
} nux_vertex_attribute_t;

typedef enum
{
    NUX_PRIMITIVE_TRIANGLES = 0,
    NUX_PRIMITIVE_LINES     = 1,
    NUX_PRIMITIVE_POINTS    = 2,
} nux_primitive_t;

typedef enum
{
    NUX_BUTTON_A     = 1 << 0,
    NUX_BUTTON_X     = 1 << 1,
    NUX_BUTTON_Y     = 1 << 2,
    NUX_BUTTON_B     = 1 << 3,
    NUX_BUTTON_UP    = 1 << 4,
    NUX_BUTTON_DOWN  = 1 << 5,
    NUX_BUTTON_LEFT  = 1 << 6,
    NUX_BUTTON_RIGHT = 1 << 7,
    NUX_BUTTON_LB    = 1 << 8,
    NUX_BUTTON_RB    = 1 << 9,
} nux_button_t;

typedef enum
{
    NUX_AXIS_LEFTX  = 0,
    NUX_AXIS_LEFTY  = 1,
    NUX_AXIS_RIGHTX = 2,
    NUX_AXIS_RIGHTY = 3,
    NUX_AXIS_RT     = 4,
    NUX_AXIS_LT     = 5,
} nux_axis_t;

typedef enum
{
    NUX_CONSOLE_MEMORY_CAPACITY = 0,
    NUX_CONSOLE_MEMORY_USAGE    = 1,
} nux_console_info_t;

void      nux_trace(nux_env_t env, const nux_c8_t *text);
void      nux_inspect_i32(nux_env_t env, const nux_c8_t *name, nux_i32_t *p);
void      nux_inspect_f32(nux_env_t env, const nux_c8_t *name, nux_f32_t *p);
nux_u32_t nux_console_info(nux_env_t env, nux_console_info_t info);
nux_f32_t nux_global_time(nux_env_t env);
nux_f32_t nux_delta_time(nux_env_t env);

nux_id_t nux_stack_new(nux_env_t env, nux_id_t allocator, nux_u32_t size);

nux_id_t nux_create_texture(nux_env_t env, nux_id_t stack, nux_u32_t size);
void     nux_update_texture(nux_env_t   env,
                            nux_id_t    id,
                            nux_u32_t   x,
                            nux_u32_t   y,
                            nux_u32_t   w,
                            nux_u32_t   h,
                            const void *p);

nux_id_t nux_create_mesh(nux_env_t              env,
                         nux_id_t               stack,
                         nux_u32_t              count,
                         nux_primitive_t        primitive,
                         nux_vertex_attribute_t attributes);
void     nux_update_mesh(nux_env_t              env,
                         nux_id_t               id,
                         nux_vertex_attribute_t attributes,
                         nux_u32_t              first,
                         nux_u32_t              count,
                         const void            *p);

nux_id_t nux_create_spritesheet(nux_env_t env,
                                nux_id_t  stack,
                                nux_id_t  texture,
                                nux_u32_t row,
                                nux_u32_t col,
                                nux_u32_t fwidth,
                                nux_u32_t fheight);

nux_id_t nux_create_scene(nux_env_t env, nux_id_t stack, nux_u32_t object_capa);
nux_id_t nux_node_root(nux_env_t env, nux_id_t scene);
nux_id_t nux_create_node(nux_env_t env, nux_id_t parent);
nux_id_t nux_create_instance_node(nux_env_t env,
                                  nux_id_t  parent,
                                  nux_id_t  instance);
void     nux_delete_node(nux_env_t env, nux_id_t id);
void     nux_node_translation(nux_env_t env, nux_id_t id, nux_f32_t *pos);
void     nux_node_rotation(nux_env_t env, nux_id_t id, nux_f32_t *rot);
void     nux_node_scale(nux_env_t env, nux_id_t id, nux_f32_t *scale);
void nux_set_node_translation(nux_env_t env, nux_id_t id, const nux_f32_t *pos);
void nux_set_node_rotation(nux_env_t env, nux_id_t id, const nux_f32_t *rot);
void nux_set_node_scale(nux_env_t env, nux_id_t id, const nux_f32_t *scale);
nux_id_t nux_node_scene(nux_env_t env, nux_id_t id);
nux_id_t nux_node_parent(nux_env_t env, nux_id_t id);
nux_id_t nux_node_next(nux_env_t env, nux_id_t id);
nux_id_t nux_node_child(nux_env_t env, nux_id_t id);

nux_id_t nux_create_camera(nux_env_t env, nux_id_t parent);
void     nux_set_camera_perspective(
        nux_env_t env, nux_id_t id, nux_f32_t fov, nux_f32_t near, nux_f32_t far);

nux_status_t nux_create_model(nux_env_t env,
                              nux_id_t  node,
                              nux_id_t  mesh,
                              nux_id_t  texture);

void nux_push_scissor(
    nux_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);
void nux_push_viewport(
    nux_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);
void nux_push_cursor(nux_env_t env, nux_u32_t x, nux_u32_t y);
void nux_push_color(nux_env_t env, nux_u32_t color);
void nux_clear(nux_env_t env, nux_u32_t color);
// void nux_draw_cube(nux_env_t env, const nux_f32_t *c, const nux_f32_t *s);
// void nux_draw_lines(nux_env_t env, const nux_f32_t *p, nux_u32_t n);
// void nux_draw_linestrip(nux_env_t env, const nux_f32_t *p, nux_u32_t n);
void nux_draw_text(nux_env_t env, const nux_c8_t *text);
void nux_print(nux_env_t env, const nux_c8_t *text);
void nux_blit(nux_env_t env,
              nux_id_t  texture,
              nux_u32_t x,
              nux_u32_t y,
              nux_u32_t w,
              nux_u32_t h);
void nux_draw_sprite(nux_env_t env, nux_id_t spritesheet, nux_u32_t sprite);
void nux_draw_scene(nux_env_t env, nux_id_t scene, nux_id_t camera);

nux_u32_t nux_button(nux_env_t env, nux_u32_t player);
nux_f32_t nux_axis(nux_env_t env, nux_u32_t player, nux_axis_t axis);

#endif
