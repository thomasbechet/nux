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
    PLAYER_MAX       = 4,
    BUTTON_MAX       = 10,
    AXIS_MAX         = 6,
    NODE_MAX         = (1 << 16) - 1,
    NAME_MAX         = 64
} constants_t;

typedef enum
{
    ERROR_NONE                        = 0,
    ERROR_OUT_OF_MEMORY               = 1,
    ERROR_OUT_OF_POOL_ITEM            = 2,
    ERROR_OUT_OF_COMMANDS             = 3,
    ERROR_OUT_OF_DYNAMIC_OBJECTS      = 12,
    ERROR_INVALID_OBJECT_STATIC_INDEX =,
    ERROR_INVALID_TEXTURE_SIZE        = 4,
    ERROR_INVALID_OBJECT_ID           = 5,
    ERROR_INVALID_OBJECT_TYPE         = 6,
    ERROR_WASM_RUNTIME                = 8,
    ERROR_INVALID_OBJECT_CREATION     = 9,
    ERROR_CART_EOF                    = 10,
    ERROR_CART_MOUNT                  = 11
} error_t;

typedef enum
{
    SUCCESS = 1,
    FAILURE = 0
} status_t;

typedef enum
{
    OBJECT_MEMORY         =,
    OBJECT_STACK          =,
    OBJECT_POOL           =,
    OBJECT_WASM           =,
    OBJECT_RAW            =,
    OBJECT_TEXTURE        =,
    OBJECT_MESH           =,
    OBJECT_SPRITESHEET    =,
    OBJECT_SCENE          =,
    OBJECT_NODE           =,
    OBJECT_NODE_TRANSFORM =,
    OBJECT_NODE_CAMERA    =,
    OBJECT_NODE_MODEL     =,
    OBJECT_TYPE_MAX       = 11
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
WASM_EXPORT("object_type")
object_type(u32 id);
WASM_EXPORT("object_put")
void object_put(u32 id, u32 index);
WASM_EXPORT("stack_new")
u32 stack_new(u32 stack, u32 size);
WASM_EXPORT("texture_create")
u32 texture_create(u32 stack, u32 size);
WASM_EXPORT("texture_update")
void texture_update(u32 id, u32 x, u32 y, u32 w, u32 h, const void *p);
WASM_EXPORT("mesh_create")
u32 mesh_create(u32 stack, u32 count, u32 primitive, u32 attributes);
WASM_EXPORT("mesh_update")
void mesh_update(u32 id, u32 attributes, u32 first, u32 count, const void *p);
WASM_EXPORT("spritesheet_create")
u32 spritesheet_create(
    u32 stack, u32 texture, u32 row, u32 col, u32 fwidth, u32 fheight);
WASM_EXPORT("scene_create")
u32 scene_create(u32 stack, u32 object_capa);
WASM_EXPORT("node_root")
u32 node_root(u32 scene);
WASM_EXPORT("node_create")
u32 node_create(u32 parent);
WASM_EXPORT("node_create_instance")
u32 node_create_instance(u32 parent, u32 instance);
WASM_EXPORT("node_delete")
void node_delete(u32 id);
WASM_EXPORT("node_translation")
void node_translation(u32 id, f32 *pos);
WASM_EXPORT("node_rotation")
void node_rotation(u32 id, f32 *rot);
WASM_EXPORT("node_scale")
void node_scale(u32 id, f32 *scale);
WASM_EXPORT("node_set_translation")
void node_set_translation(u32 id, const f32 *pos);
WASM_EXPORT("node_set_rotation")
void node_set_rotation(u32 id, const f32 *rot);
WASM_EXPORT("node_set_scale")
void node_set_scale(u32 id, const f32 *scale);
WASM_EXPORT("node_scene")
u32 node_scene(u32 id);
WASM_EXPORT("node_parent")
u32 node_parent(u32 id);
WASM_EXPORT("node_next")
u32 node_next(u32 id);
WASM_EXPORT("node_child")
u32 node_child(u32 id);
WASM_EXPORT("camera_create")
u32 camera_create(u32 parent);
WASM_EXPORT("camera_set_perspective")
void camera_set_perspective(u32 id, f32 fov, f32 near, f32 far);
WASM_EXPORT("model_create")
void model_create(u32 parent, u32 mesh, u32 texture);
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