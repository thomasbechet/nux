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
    TRANSFORM_MODEL      = 0,
    TRANSFORM_VIEW       = 1,
    TRANSFORM_PROJECTION = 2
} transform_t;

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
    AXIS_LEFTX    = 0,
    AXIS_LEFTY    = 1,
    AXIS_RIGHTX   = 2,
    AXIS_RIGHTY   = 3,
    AXIS_RT       = 4,
    AXIS_LT       = 5,
    AXIS_ENUM_MAX = 6
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
WASM_EXPORT("console_info")
u32 console_info(u32 info);
WASM_EXPORT("add_group")
u32 add_group(u32 size);
WASM_EXPORT("clear_group")
void clear_group(u32 group);
WASM_EXPORT("find")
u32 find(const void *name);
WASM_EXPORT("add_texture")
u32 add_texture(u32 size);
WASM_EXPORT("write_texture")
void write_texture(u32 id, u32 x, u32 y, u32 w, u32 h, const void *p);
WASM_EXPORT("add_mesh")
u32 add_mesh(u32 count, u32 primitive, u32 attribs);
WASM_EXPORT("write_mesh")
void write_mesh(u32 id, u32 attribs, u32 first, u32 count, const void *p);
WASM_EXPORT("add_model")
u32 add_model(u32 count);
WASM_EXPORT("write_model")
void write_model(
    u32 id, u32 node, u32 mesh, u32 texture, u32 parent, const f32 *transform);
WASM_EXPORT("add_spritesheet")
u32 add_spritesheet(u32 texture, u32 row, u32 col, u32 fwidth, u32 fheight);
WASM_EXPORT("transform")
void transform(u32 transform, const f32 *m);
WASM_EXPORT("cursor")
void cursor(u32 x, u32 y);
WASM_EXPORT("fog_params")
void fog_params(const f32 *params);
WASM_EXPORT("fog_color")
void fog_color(u32 color);
WASM_EXPORT("clear")
void clear(u32 color);
WASM_EXPORT("color")
void color(u32 color);
WASM_EXPORT("draw")
void draw(u32 id);
WASM_EXPORT("draw_cube")
void draw_cube(const f32 *c, const f32 *s);
WASM_EXPORT("draw_lines")
void draw_lines(const f32 *p, u32 n);
WASM_EXPORT("draw_linestrip")
void draw_linestrip(const f32 *p, u32 n);
WASM_EXPORT("text")
void text(const void *text);
WASM_EXPORT("print")
void print(const void *text);
WASM_EXPORT("blit")
void blit(u32 id, u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("sprite")
void sprite(u32 spritesheet, u32 sprite);
WASM_EXPORT("button")
u32 button(u32 player);
WASM_EXPORT("axis")
f32 axis(u32 player, u32 axis);

#ifdef __cplusplus
}
#endif

#endif