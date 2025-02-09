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

//////////////////////////////////////////////////////////////////////////
//////                            MODULE                            //////
//////////////////////////////////////////////////////////////////////////

WASM_EXPORT("start")
void start();
WASM_EXPORT("update")
void update();
WASM_IMPORT("trace")
void trace(const void *s);

//////////////////////////////////////////////////////////////////////////
//////                              GPU                             //////
//////////////////////////////////////////////////////////////////////////

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

WASM_IMPORT("allocgpool")
void allocgpool(u32 idx, u32 size);
WASM_IMPORT("gpool")
void gpool(u32 idx);

WASM_IMPORT("alloctex")
void alloctex(u32 idx, u32 size, const void *p);
WASM_IMPORT("writetex")
void writetex(u32 idx, u32 x, u32 y, u32 w, u32 h, const void *p);

WASM_IMPORT("allocmesh")
void allocmesh(u32 idx, u32 count, u32 primitive, u32 attribs, const void *p);
WASM_IMPORT("writemesh")
void writemesh(u32 idx, u32 attribs, u32 first, u32 count, const void *p);

WASM_IMPORT("allocmodel")
void allocmodel(u32 idx, u32 count);
WASM_IMPORT("writemodel")
void writemodel(u32 idx, u32 node, u32 mesh, u32 texture, const f32 *transform);

WASM_IMPORT("transform")
void transform(u32 transform, const f32 *m);
WASM_IMPORT("cursor")
void cursor(u32 x, u32 y);
WASM_IMPORT("clear")
void clear(u32 color);
WASM_IMPORT("draw")
void draw(u32 model);
WASM_IMPORT("text")
void text(const void *text);
WASM_IMPORT("print")
void print(const void *text);
WASM_IMPORT("blit")
void blit(u32 idx, u32 x, u32 y, u32 w, u32 h);
WASM_IMPORT("fogmode")
void fogmode(u32 mode);
WASM_IMPORT("fogcolor")
void fogcolor(u32 color);
WASM_IMPORT("fogdensity")
void fogdensity(f32 density);

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

#ifdef __cplusplus
}
#endif

#endif
