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

#define GPU_TEX64  0
#define GPU_TEX128 1
#define GPU_TEX256 2

// Resources
WASM_IMPORT("write_texture")
void write_texture(u32 type, u32 slot, const void *p);
WASM_IMPORT("write_vertex")
void write_vertex(u32 first, u32 count, const void *p);

// State
WASM_IMPORT("push_gpu_state") void push_gpu_state(void *p);
WASM_IMPORT("pop_gpu_state") void pop_gpu_state(const void *p);
WASM_IMPORT("bind_texture") void bind_texture(u32 type, u32 slot);

// Commands
WASM_IMPORT("draw") void draw(u32 first, u32 count);
WASM_IMPORT("blit") void blit(i32 x, i32 y, i32 dx, i32 dy, u32 h, u32 w);

//////////////////////////////////////////////////////////////////////////
//////                            CARTRIDGE                         //////
//////////////////////////////////////////////////////////////////////////

// Load raw data from cart
WASM_IMPORT("load") void load(u32 chunk);
// Load raw data from cart with overrided destination
// @chunk chunk index
// @dst   destination (slot for gpu, addr for memory...)
WASM_IMPORT("loadd") void loadd(u32 chunk, u32 dst);

//////////////////////////////////////////////////////////////////////////
//////                            CONTROLLER                        //////
//////////////////////////////////////////////////////////////////////////

#endif
