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

typedef nux_u32_t nux_oid_t; // object id (must be 32bits)
typedef nux_u16_t nux_nid_t; // node id (must be 16bits)

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
    NUX_OBJECT_MAX       = 1024,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NODE_ROOT        = 1,
    NUX_NULL             = 0,
    NUX_NODE_MAX         = (1 << 16) - 1,
    NUX_NAME_MAX         = 64,
} nux_constants_t;

typedef enum
{
    NUX_ERROR_NONE        = 0,
    NUX_ERROR_ALLOCATION  = 1,
    NUX_ERROR_OUT_OF_NODE = 2,
    NUX_ERROR_INVALID_ID  = 3,
} nux_error_t;

typedef enum
{
    NUX_COMPONENT_CAMERA  = 1 << 0,
    NUX_COMPONENT_MODEL   = 1 << 1,
    NUX_COMPONENT_LIGHT   = 1 << 2,
    NUX_COMPONENT_VOLUME  = 1 << 3,
    NUX_COMPONENT_EMITTER = 1 << 4,
    NUX_COMPONENT_SOUND   = 1 << 5,
    NUX_COMPONENT_USER    = 1 << 6,
} nux_component_type_t;

typedef enum
{
    NUX_OBJECT_FREE        = 0,
    NUX_OBJECT_NULL        = 1,
    NUX_OBJECT_SCOPE       = 2,
    NUX_OBJECT_WASM        = 3,
    NUX_OBJECT_RAW         = 4,
    NUX_OBJECT_CAMERA      = 5,
    NUX_OBJECT_TEXTURE     = 6,
    NUX_OBJECT_MESH        = 7,
    NUX_OBJECT_SPRITESHEET = 9,
    NUX_OBJECT_SCENE       = 10,
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

nux_error_t nux_create_scope(nux_env_t env, nux_oid_t oid, nux_u32_t size);
void        nux_rewind_scope(nux_env_t env, nux_oid_t oid);
void        nux_set_active_scope(nux_env_t env, nux_oid_t oid);

nux_error_t nux_create_texture(nux_env_t env, nux_oid_t oid, nux_u32_t size);
void        nux_update_texture(nux_env_t   env,
                               nux_oid_t   oid,
                               nux_u32_t   x,
                               nux_u32_t   y,
                               nux_u32_t   w,
                               nux_u32_t   h,
                               const void *p);

nux_error_t nux_create_mesh(nux_env_t              env,
                            nux_oid_t              oid,
                            nux_u32_t              count,
                            nux_primitive_t        primitive,
                            nux_vertex_attribute_t attribs);
void        nux_update_mesh(nux_env_t              env,
                            nux_oid_t              oid,
                            nux_vertex_attribute_t attribs,
                            nux_u32_t              first,
                            nux_u32_t              count,
                            const void            *p);

nux_error_t nux_create_spritesheet(nux_env_t env,
                                   nux_oid_t oid,
                                   nux_u32_t texture,
                                   nux_u32_t row,
                                   nux_u32_t col,
                                   nux_oid_t oidth,
                                   nux_u32_t fheight);

nux_error_t nux_create_scene(nux_env_t env, nux_oid_t oid, nux_u32_t node_capa);

void      nux_bind_scene(nux_env_t env, nux_oid_t oid);
nux_nid_t nux_node_add(nux_env_t env, nux_nid_t parent);
void      nux_node_remove(nux_env_t env, nux_nid_t nid);
void      nux_node_get_position(nux_env_t env, nux_nid_t nid, nux_f32_t *pos);
void nux_node_set_position(nux_env_t env, nux_nid_t nid, const nux_f32_t *pos);
void nux_node_get_rotation(nux_env_t env, nux_nid_t nid, nux_f32_t *rot);
void nux_node_set_rotation(nux_env_t env, nux_nid_t nid, const nux_f32_t *rot);
void nux_node_get_scale(nux_env_t env, nux_nid_t nid, nux_f32_t *scale);
void nux_node_set_scale(nux_env_t env, nux_nid_t nid, const nux_f32_t *scale);
nux_u32_t nux_node_get_parent(nux_env_t env, nux_nid_t nid);

nux_error_t nux_camera_add(nux_env_t env, nux_nid_t nid);
void        nux_camera_remove(nux_env_t env, nux_nid_t nid);
void        nux_camera_set_perspective(
           nux_env_t env, nux_nid_t nid, nux_f32_t fov, nux_f32_t near, nux_f32_t far);

nux_error_t nux_model_add(nux_env_t env, nux_nid_t nid);
void        nux_model_remove(nux_env_t env, nux_nid_t nid);

nux_error_t nux_light_add(nux_env_t env, nux_nid_t nid);

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
              nux_oid_t texture,
              nux_u32_t x,
              nux_u32_t y,
              nux_u32_t w,
              nux_u32_t h);
void nux_draw_sprite(nux_env_t env, nux_oid_t spritesheet, nux_u32_t sprite);
void nux_draw_scene(nux_env_t env, nux_oid_t scene, nux_u32_t camera);

nux_u32_t nux_button(nux_env_t env, nux_u32_t player);
nux_f32_t nux_axis(nux_env_t env, nux_u32_t player, nux_axis_t axis);

#endif
