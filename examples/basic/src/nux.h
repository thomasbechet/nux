#ifndef NUX_H
#define NUX_H

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

//////////////////////////////////////////////////////////////////////////
//////                            MODULE                            //////
//////////////////////////////////////////////////////////////////////////

// Module start callback
WASM_EXPORT("start") void start();
// Module update callback
WASM_EXPORT("update") void update();

// Trace a message to the runtime (UTF-8 format)
WASM_IMPORT("trace") void trace(const void *str, u32 n);

//////////////////////////////////////////////////////////////////////////
//////                              GPU                             //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    TEXTURE64  = 0,
    TEXTURE128 = 1,
    TEXTURE256 = 2,
    TEXTURE512 = 3,
} gpu_texture_size_t;

typedef enum
{
    VERTEX_POSTIION = 1 << 0,
    VERTEX_UV       = 1 << 1,
    VERTEX_COLOR    = 1 << 2,
} gpu_vertex_flags_t;

typedef enum
{
    PRIMITIVE_TRIANGLES = 0,
    PRIMITIVE_LINES     = 1,
    PRIMITIVE_POINTS    = 2,
} gpu_primitive_t;

typedef enum
{
    TRANSFORM_MODEL      = 0,
    TRANSFORM_VIEW       = 1,
    TRANSFORM_PROJECTION = 2,
} gpu_transform_t;

WASM_IMPORT("alloc_texture")
void alloc_texture(u32 index, u32 texture_size, const void *p);
WASM_IMPORT("write_texture")
void write_texture(u32 index, u32 x, u32 y, u32 w, u32 h, const void *p);

WASM_IMPORT("alloc_mesh")
void alloc_mesh(
    u32 index, u32 count, u32 primitive, u32 attributes, const void *p);
WASM_IMPORT("write_mesh")
void write_mesh(u32 index, u32 attributes, u32 first, u32 count, const void *p);

WASM_IMPORT("alloc_model")
void alloc_model(u32 index, u32 node_count);
WASM_IMPORT("update_model")
void update_model(
    u32 index, u32 node_index, u32 mesh, u32 texture, const f32 *transform);

WASM_IMPORT("push_transform")
void push_transform(u32 transform, const f32 *m);
WASM_IMPORT("draw_model")
void draw_model(u32 model);

//////////////////////////////////////////////////////////////////////////
//////                            CARTRIDGE                         //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                            CONTROLLER                        //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    BUTTON_A  = 1 << 0,
    BUTTON_X  = 1 << 1,
    BUTTON_Y  = 1 << 2,
    BUTTON_B  = 1 << 3,
    BUTTON_LB = 1 << 4,
    BUTTON_RB = 1 << 5,
} button_t;

typedef enum
{
    AXIS_LEFTX  = 0,
    AXIS_LEFTY  = 1,
    AXIS_RIGHTX = 2,
    AXIS_RIGHTY = 3,
} axis_t;

WASM_IMPORT("button")
u32 button(u32 player);
WASM_IMPORT("axis")
f32 axis(u32 player, u32 axis);

#endif
