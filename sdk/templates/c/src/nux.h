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
    SCREEN_WIDTH     = 320,
    SCREEN_HEIGHT    = 240,
    TEXTURE_MIN_SIZE = 32,
    TEXTURE_MAX_SIZE = 256,
    PLAYER_MAX       = 4,
    BUTTON_MAX       = 10,
    AXIS_MAX         = 6,
    NODE_MAX         = (1 << 16) - 1,
    NAME_MAX         = 64,
    PALETTE_LEN      = 256
} constants_t;

typedef enum
{
    ERROR_NONE                 = 0,
    ERROR_OUT_OF_MEMORY        = 1,
    ERROR_INVALID_TEXTURE_SIZE = 4,
    ERROR_WASM_RUNTIME         = 8,
    ERROR_CART_EOF             = 10,
    ERROR_CART_MOUNT           = 11
} error_t;

typedef enum
{
    SUCCESS = 1,
    FAILURE = 0
} status_t;

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

typedef enum
{
    MAP_SCREEN  = 0x0,
    MAP_PALETTE = NUX_MAP_SCREEN + (NUX_SCREEN_WIDTH * NUX_SCREEN_HEIGHT),
    MAP_BUTTONS = NUX_MAP_PALETTE + (NUX_PALETTE_LEN * (sizeof(nux_u32_t))),
    MAP_AXIS    = NUX_MAP_BUTTONS + (NUX_PLAYER_MAX * (sizeof(nux_u32_t))),
    MAP_TIME
    = NUX_MAP_AXIS + ((NUX_PLAYER_MAX * (sizeof(nux_f32_t))) * NUX_AXIS_MAX),
    MAP_FRAME     = NUX_MAP_TIME + (sizeof(nux_f32_t)),
    MAP_DRAWSTATE = NUX_MAP_FRAME + (sizeof(nux_u32_t)),
    MAP_CURSOR    = NUX_MAP_DRAWSTATE,
    MAP_CURSORX   = NUX_MAP_CURSOR + 0,
    MAP_CURSORY   = NUX_MAP_CURSOR + (sizeof(nux_i32_t))
} map_t;

//////////////////////////////////////////////////////////////////////////
//////                           SYSCALL                            //////
//////////////////////////////////////////////////////////////////////////

WASM_EXPORT("trace")
void trace(const void *text);
WASM_EXPORT("dbgi32")
void dbgi32(const void *name, u32 *p);
WASM_EXPORT("dbgf32")
void dbgf32(const void *name, f32 *p);
WASM_EXPORT("stat")
u32 stat(u32 info);
WASM_EXPORT("gtime")
f32 gtime();
WASM_EXPORT("dtime")
f32 dtime();
WASM_EXPORT("frame")
u32 frame();
WASM_EXPORT("pal")
void pal(u32 index, u32 color);
WASM_EXPORT("cls")
void cls(u32 color);
WASM_EXPORT("fill")
void fill(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
WASM_EXPORT("pset")
void pset(u32 x, u32 y, u32 color);
WASM_EXPORT("pget")
pget(u32 x, u32 y);
WASM_EXPORT("line")
void line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
WASM_EXPORT("filltri")
void filltri(u32 x0, u32 y0, u32 x1, u32 y1, u32 x2, u32 y2, u32 color);
WASM_EXPORT("text")
void text(u32 x, u32 y, const void *text, u32 c);
WASM_EXPORT("print")
void print(const void *text, u32 c);
WASM_EXPORT("cursorx")
cursorx();
WASM_EXPORT("cursory")
cursory();
WASM_EXPORT("cursor")
void cursor(u32 x, u32 y);
WASM_EXPORT("btn")
u32 btn(u32 player);
WASM_EXPORT("axs")
f32 axs(u32 player, u32 axis);

#ifdef __cplusplus
}
#endif

#endif