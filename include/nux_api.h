#ifndef NUX_API_H
#define NUX_API_H

//////////////////////////////////////////////////////////////////////////
//////                          Core Types                          //////
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

typedef unsigned char u8;
typedef int           i32;
typedef unsigned int  u32;
typedef long          i64;
typedef unsigned long u64;
typedef float         f32;
typedef double        f64;

//////////////////////////////////////////////////////////////////////////
//////                        Export Macros                         //////
//////////////////////////////////////////////////////////////////////////

#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NU_API_EXPORT __declspec(dllexport)
#elif defined(__linux__) || defined(__unix__) || defined(__linux)
#define NU_API_EXPORT __attribute__((visibility("default")))
#elif defined(__APPLE__)
#define NU_API_EXPORT
#elif defined(_POSIX_VERSION)
#define NU_API_EXPORT
#else
#define NU_API_EXPORT
#endif

#define NU_API NU_API_EXPORT

//////////////////////////////////////////////////////////////////////////
//////                           Constants                          //////
//////////////////////////////////////////////////////////////////////////

#define MAX_TEXTURE_COUNT 128

//////////////////////////////////////////////////////////////////////////
//////                              GPU                             //////
//////////////////////////////////////////////////////////////////////////

// state
NU_API void push_gpu_state(u8 *p);
NU_API void pop_gpu_state(const u8 *p);
NU_API void bind_texture(u32 slot);
NU_API void bind_vertex_buffer(u32 slot);

// resources
NU_API void write_texture(u32 x, u32 y, u32 h, u32 w, const u8 *data);
NU_API void write_vertex_buffer(u32 first, u32 count, const u8 *data);

// commands
NU_API void draw(u32 first, u32 count);
NU_API void blit(u32 x, u32 y, u32 dx, u32 dy, u32 h, u32 w);

//////////////////////////////////////////////////////////////////////////
//////                               IO                             //////
//////////////////////////////////////////////////////////////////////////

#endif
