#ifndef NUX_H
#define NUX_H

//////////////////////////////////////////////////////////////////////////
//////                          Core Types                          //////
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

typedef uint8_t  u8;
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

WASM_EXPORT("start") void start();
WASM_EXPORT("update") void update();

//////////////////////////////////////////////////////////////////////////
//////                           Constants                          //////
//////////////////////////////////////////////////////////////////////////

#define MAX_TEXTURE_COUNT 128

//////////////////////////////////////////////////////////////////////////
//////                              GPU                             //////
//////////////////////////////////////////////////////////////////////////

// State
WASM_IMPORT("push_gpu_state") void push_gpu_state(u8 *p);
WASM_IMPORT("pop_gpu_state") void pop_gpu_state(const u8 *p);
WASM_IMPORT("bind_texture") void bind_texture(u32 slot);
WASM_IMPORT("bind_vertex_buffer") void bind_vertex_buffer(u32 slot);

// Resources
WASM_IMPORT("write_texture")
void write_texture(u32 x, u32 y, u32 h, u32 w, const u8 *data);
WASM_IMPORT("write_vertex_buffer")
void write_vertex_buffer(u32 first, u32 count, const u8 *data);

// Commands
WASM_IMPORT("draw") void draw(u32 first, u32 count);
WASM_IMPORT("blit") void blit(u32 x, u32 y, u32 dx, u32 dy, u32 h, u32 w);

//////////////////////////////////////////////////////////////////////////
//////                               IO                             //////
//////////////////////////////////////////////////////////////////////////

void load(void);
void reload(void);

//////////////////////////////////////////////////////////////////////////
//////                             MEMORY                           //////
//////////////////////////////////////////////////////////////////////////

u8 *memalloc(u32 size);

#endif
