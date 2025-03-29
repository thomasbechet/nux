#ifndef NUX_H
#define NUX_H

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////
//////                          Core Types                          //////
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

//////////////////////////////////////////////////////////////////////////
//////                        Export Macros                         //////
//////////////////////////////////////////////////////////////////////////

#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define WASM_EXPORT(name) __declspec(dllexport)
#define WASM_IMPORT(name)
#elif defined(__linux__) || defined(__unix__) || defined(__linux)
#define WASM_EXPORT(name) __attribute__((visibility("default")))
#define WASM_IMPORT(name)
#elif defined(__APPLE__)
#define WASM_EXPORT(name)
#define WASM_IMPORT(name)
#elif defined(_POSIX_VERSION)
#define WASM_EXPORT(name)
#define WASM_IMPORT(name)
#else
#define WASM_EXPORT(name)
#define WASM_IMPORT(name)
#endif
#
//////////////////////////////////////////////////////////////////////////
//////                          CALLBACKS                           //////
//////////////////////////////////////////////////////////////////////////

WASM_EXPORT("start")
void start();
WASM_EXPORT("update")
void update();

//////////////////////////////////////////////////////////////////////////
//////                            ENUMS                             //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    SCREEN_WIDTH     = 480,
    SCREEN_HEIGHT    = 360,
    TEXTURE_MIN_SIZE = 32,
    TEXTURE_MAX_SIZE = 256,
    OBJECT_MAX       = 1024,
    PLAYER_MAX       = 4,
    BUTTON_MAX       = 10,
    AXIS_MAX         = 6,
    NODE_ROOT        = 1,
    NODE_MAX         = (1 << 16) - 1,
    NAME_MAX         = 64
} constants_t;

typedef enum
{
    ERROR_NONE                 = 0,
    ERROR_ALLOCATION           = 1,
    ERROR_OUT_OF_NODE          = 2,
    ERROR_INVALID_ID           = 3,
    ERROR_INVALID_TEXTURE_SIZE = 4,
    ERROR_RUNTIME              = 5
} error_t;

typedef enum
{
    SUCCESS = 1,
    FAILURE = 0
} status_t;

typedef enum
{
    COMPONENT_CAMERA  = 1 << 0,
    COMPONENT_MODEL   = 1 << 1,
    COMPONENT_LIGHT   = 1 << 2,
    COMPONENT_VOLUME  = 1 << 3,
    COMPONENT_EMITTER = 1 << 4,
    COMPONENT_SOUND   = 1 << 5,
    COMPONENT_USER    = 1 << 6
} component_type_t;

typedef enum
{
    OBJECT_FREE        = 0,
    OBJECT_NULL        = 1,
    OBJECT_SCOPE       = 2,
    OBJECT_WASM        = 3,
    OBJECT_RAW         = 4,
    OBJECT_CAMERA      = 5,
    OBJECT_TEXTURE     = 6,
    OBJECT_MESH        = 7,
    OBJECT_SPRITESHEET = 9,
    OBJECT_SCENE       = 10
} object_type_t;

typedef enum
{
    VERTEX_POSITION = 1 << 0,
    VERTEX_UV       = 1 << 1,
    VERTEX_COLOR    = 1 << 2,
    VERTEX_INDICES  = 1 << 3
} vertex_attribute_t;

typedef enum
{
    PRIMITIVE_TRIANGLES = 0,
    PRIMITIVE_LINES     = 1,
    PRIMITIVE_POINTS    = 2
} primitive_t;

typedef enum
{
    BUTTON_A     = 1 << 0,
    BUTTON_X     = 1 << 1,
    BUTTON_Y     = 1 << 2,
    BUTTON_B     = 1 << 3,
    BUTTON_UP    = 1 << 4,
    BUTTON_DOWN  = 1 << 5,
    BUTTON_LEFT  = 1 << 6,
    BUTTON_RIGHT = 1 << 7,
    BUTTON_LB    = 1 << 8,
    BUTTON_RB    = 1 << 9
} button_t;

typedef enum
{
    AXIS_LEFTX  = 0,
    AXIS_LEFTY  = 1,
    AXIS_RIGHTX = 2,
    AXIS_RIGHTY = 3,
    AXIS_RT     = 4,
    AXIS_LT     = 5
} axis_t;

typedef enum
{
    CONSOLE_MEMORY_CAPACITY = 0,
    CONSOLE_MEMORY_USAGE    = 1
} console_info_t;

//////////////////////////////////////////////////////////////////////////
//////                           SYSCALL                            //////
//////////////////////////////////////////////////////////////////////////

WASM_EXPORT("trace")
void trace(const void *text);
WASM_EXPORT("inspect_i32")
void inspect_i32(const void *name, u32 *p);
WASM_EXPORT("inspect_f32")
void inspect_f32(const void *name, f32 *p);
WASM_EXPORT("console_info")
u32 console_info(u32 info);
WASM_EXPORT("global_time")
f32 global_time();
WASM_EXPORT("delta_time")
f32 delta_time();
WASM_EXPORT("create_scope")
void create_scope(u32 oid, u32 size);
WASM_EXPORT("rewind_scope")
void rewind_scope(u32 oid);
WASM_EXPORT("set_active_scope")
void set_active_scope(u32 oid);
WASM_EXPORT("create_texture")
void create_texture(u32 oid, u32 size);
WASM_EXPORT("update_texture")
void update_texture(u32 oid, u32 x, u32 y, u32 w, u32 h, const void *p);
WASM_EXPORT("create_mesh")
void create_mesh(u32 oid, u32 count, u32 primitive, u32 attributes);
WASM_EXPORT("update_mesh")
void update_mesh(u32 oid, u32 attributes, u32 first, u32 count, const void *p);
WASM_EXPORT("create_spritesheet")
void create_spritesheet(
    u32 oid, u32 texture, u32 row, u32 col, u32 fwidth, u32 fheight);
WASM_EXPORT("create_scene")
void create_scene(u32 oid, u32 node_capa);
WASM_EXPORT("bind_scene")
void bind_scene(u32 oid);
WASM_EXPORT("node_add")
u32 node_add(u32 parent);
WASM_EXPORT("node_remove")
void node_remove(u32 nid);
WASM_EXPORT("node_get_translation")
void node_get_translation(u32 nid, f32 *pos);
WASM_EXPORT("node_set_translation")
void node_set_translation(u32 nid, const f32 *pos);
WASM_EXPORT("node_get_rotation")
void node_get_rotation(u32 nid, f32 *rot);
WASM_EXPORT("node_set_rotation")
void node_set_rotation(u32 nid, const f32 *rot);
WASM_EXPORT("node_get_scale")
void node_get_scale(u32 nid, f32 *scale);
WASM_EXPORT("node_set_scale")
void node_set_scale(u32 nid, const f32 *scale);
WASM_EXPORT("node_get_parent")
u32 node_get_parent(u32 nid);
WASM_EXPORT("camera_add")
void camera_add(u32 nid);
WASM_EXPORT("camera_remove")
void camera_remove(u32 nid);
WASM_EXPORT("camera_set_perspective")
void camera_set_perspective(u32 nid, f32 fov, f32 near, f32 far);
WASM_EXPORT("model_add")
void model_add(u32 nid, u32 mesh, u32 texture);
WASM_EXPORT("model_remove")
void model_remove(u32 nid);
WASM_EXPORT("push_scissor")
void push_scissor(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("push_viewport")
void push_viewport(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("push_cursor")
void push_cursor(u32 x, u32 y);
WASM_EXPORT("push_color")
void push_color(u32 color);
WASM_EXPORT("clear")
void clear(u32 color);
WASM_EXPORT("draw_text")
void draw_text(const void *text);
WASM_EXPORT("print")
void print(const void *text);
WASM_EXPORT("blit")
void blit(u32 texture, u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("draw_sprite")
void draw_sprite(u32 spritesheet, u32 sprite);
WASM_EXPORT("draw_scene")
void draw_scene(u32 scene, u32 camera);
WASM_EXPORT("button")
u32 button(u32 player);
WASM_EXPORT("axis")
f32 axis(u32 player, u32 axis);

#ifdef __cplusplus
}
#endif

#endif