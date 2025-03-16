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
    SCREEN_WIDTH       = 480,
    SCREEN_HEIGHT      = 360,
    MIN_TEXTURE_SIZE   = 32,
    MAX_TEXTURE_SIZE   = 256,
    MAX_RESOURCE_COUNT = 1024,
    MAX_PLAYER_COUNT   = 4
} constants_t;

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
    BUTTON_RB    = 1 << 9,
    BUTTON_COUNT = 10
} button_t;

typedef enum
{
    AXIS_LEFTX  = 0,
    AXIS_LEFTY  = 1,
    AXIS_RIGHTX = 2,
    AXIS_RIGHTY = 3,
    AXIS_RT     = 4,
    AXIS_LT     = 5,
    AXIS_COUNT  = 6
} axis_t;

typedef enum
{
    CONSOLE_MEMORY_CAPACITY = 0,
    CONSOLE_MEMORY_USAGE    = 1
} console_info_t;

typedef enum
{
    INSPECT_I32 = 0,
    INSPECT_F32 = 1
} inspect_type_t;

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
void create_scope(u32 id, u32 size);
WASM_EXPORT("rewind_scope")
void rewind_scope(u32 id);
WASM_EXPORT("set_active_scope")
void set_active_scope(u32 id);
WASM_EXPORT("create_camera")
void create_camera(u32 id);
WASM_EXPORT("set_camera_view")
void set_camera_view(u32 id, const f32 *m);
WASM_EXPORT("set_camera_projection")
void set_camera_projection(u32 id, const f32 *m);
WASM_EXPORT("set_camera_lookat")
void set_camera_lookat(u32        id,
                       const f32 *eye,
                       const f32 *center,
                       const f32 *up);
WASM_EXPORT("set_camera_perspective")
void set_camera_perspective(u32 id, f32 fov, f32 near, f32 far);
WASM_EXPORT("create_texture")
void create_texture(u32 id, u32 size);
WASM_EXPORT("update_texture")
void update_texture(u32 id, u32 x, u32 y, u32 w, u32 h, const void *p);
WASM_EXPORT("create_mesh")
void create_mesh(u32 id, u32 count, u32 primitive, u32 attribs);
WASM_EXPORT("update_mesh")
void update_mesh(u32 id, u32 attribs, u32 first, u32 count, const void *p);
WASM_EXPORT("create_model")
void create_model(u32 id, u32 count);
WASM_EXPORT("update_model")
void update_model(
    u32 id, u32 node, u32 mesh, u32 texture, u32 parent, const f32 *transform);
WASM_EXPORT("create_spritesheet")
void create_spritesheet(
    u32 id, u32 texture, u32 row, u32 col, u32 fwidth, u32 fheight);
WASM_EXPORT("push_scissor")
void push_scissor(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("push_viewport")
void push_viewport(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("push_camera")
void push_camera(u32 id);
WASM_EXPORT("push_translation")
void push_translation(f32 x, f32 y, f32 z);
WASM_EXPORT("push_cursor")
void push_cursor(u32 x, u32 y);
WASM_EXPORT("push_color")
void push_color(u32 color);
WASM_EXPORT("clear")
void clear(u32 color);
WASM_EXPORT("draw_model")
void draw_model(u32 id);
WASM_EXPORT("draw_volume")
void draw_volume();
WASM_EXPORT("draw_cube")
void draw_cube(const f32 *c, const f32 *s);
WASM_EXPORT("draw_lines")
void draw_lines(const f32 *p, u32 n);
WASM_EXPORT("draw_linestrip")
void draw_linestrip(const f32 *p, u32 n);
WASM_EXPORT("draw_text")
void draw_text(const void *text);
WASM_EXPORT("print")
void print(const void *text);
WASM_EXPORT("blit")
void blit(u32 id, u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("draw_sprite")
void draw_sprite(u32 spritesheet, u32 sprite);
WASM_EXPORT("button")
u32 button(u32 player);
WASM_EXPORT("axis")
f32 axis(u32 player, u32 axis);

#ifdef __cplusplus
}
#endif

#endif