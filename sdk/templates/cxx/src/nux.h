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

typedef enum
{
    RENDER_VIEWPORT    = 0,
    RENDER_SCISSOR     = 1,
    RENDER_TRANSFORM   = 2,
    RENDER_VIEW        = 3,
    RENDER_PROJECTION  = 4,
    RENDER_CURSOR      = 5,
    RENDER_FOG_COLOR   = 6,
    RENDER_FOG_DENSITY = 7,
    RENDER_FOG_NEAR    = 8,
    RENDER_FOG_FAR     = 9,
    RENDER_COLOR       = 10
} render_state_t;

typedef enum
{
    INSPECT_I32 = 0,
    INSPECT_F32 = 0
} inspect_type_t;

//////////////////////////////////////////////////////////////////////////
//////                           SYSCALL                            //////
//////////////////////////////////////////////////////////////////////////

WASM_EXPORT("trace")
void trace(const void *text);
WASM_EXPORT("inspect")
void inspect(const void *name, u32 type, void *p);
WASM_EXPORT("console_info")
u32 console_info(u32 info);
WASM_EXPORT("global_time")
f32 global_time();
WASM_EXPORT("delta_time")
f32 delta_time();
WASM_EXPORT("init_scope")
void init_scope(u32 id, u32 size);
WASM_EXPORT("rewind_scope")
void rewind_scope(u32 id);
WASM_EXPORT("set_active_scope")
void set_active_scope(u32 id);
WASM_EXPORT("init_texture")
void init_texture(u32 id, u32 size);
WASM_EXPORT("update_texture")
void update_texture(u32 id, u32 x, u32 y, u32 w, u32 h, const void *p);
WASM_EXPORT("init_mesh")
void init_mesh(u32 id, u32 count, u32 primitive, u32 attribs);
WASM_EXPORT("update_mesh")
void update_mesh(u32 id, u32 attribs, u32 first, u32 count, const void *p);
WASM_EXPORT("init_model")
void init_model(u32 id, u32 count);
WASM_EXPORT("update_model")
void update_model(
    u32 id, u32 node, u32 mesh, u32 texture, u32 parent, const f32 *transform);
WASM_EXPORT("init_spritesheet")
void init_spritesheet(
    u32 id, u32 texture, u32 row, u32 col, u32 fwidth, u32 fheight);
WASM_EXPORT("set_render_state")
void set_render_state(u32 state, const void *p);
WASM_EXPORT("get_render_state")
void get_render_state(u32 state, void *p);
WASM_EXPORT("set_scissor")
void set_scissor(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("set_viewport")
void set_viewport(u32 x, u32 y, u32 w, u32 h);
WASM_EXPORT("set_view")
void set_view(const f32 *m);
WASM_EXPORT("set_projection")
void set_projection(const f32 *m);
WASM_EXPORT("set_transform")
void set_transform(const f32 *m);
WASM_EXPORT("set_cursor")
void set_cursor(u32 x, u32 y);
WASM_EXPORT("set_fog_near")
void set_fog_near(f32 near);
WASM_EXPORT("set_fog_far")
void set_fog_far(f32 far);
WASM_EXPORT("set_fog_density")
void set_fog_density(f32 density);
WASM_EXPORT("set_fog_color")
void set_fog_color(u32 color);
WASM_EXPORT("set_color")
void set_color(u32 color);
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