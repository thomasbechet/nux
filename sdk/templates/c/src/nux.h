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
WASM_EXPORT("button")
u32 button(u32 player);
WASM_EXPORT("axis")
f32 axis(u32 player, u32 axis);

#ifdef __cplusplus
}
#endif

#endif