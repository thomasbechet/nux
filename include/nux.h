#ifndef NUX_H
#define NUX_H

//////////////////////////////////////////////////////////////////////////
//////                     Import/Export Macros                     //////
//////////////////////////////////////////////////////////////////////////

#if defined(NU_PLATFORM_WINDOWS)

#define NU_API_EXPORT __declspec(dllexport)
#define NU_API_IMPORT __declspec(dllimport)
#if defined(_MSC_VER)
#define NU_ALIGN(X) __declspec(align(X))
#else
#define NU_ALIGN(X) __attribute((aligned(X)))
#endif

#elif defined(NU_PLATFORM_UNIX)

#define NU_API_EXPORT __attribute__((visibility("default")))
#define NU_API_IMPORT
#define NU_ALIGN(X) __attribute((aligned(X)))

#else

#define NU_API_EXPORT
#define NU_API_IMPORT
#define NU_ALIGN(X)
#pragma warning Unknown linkage directive import / export semantics.

#endif

#define NU_API NU_API_EXPORT

//////////////////////////////////////////////////////////////////////////
//////                          Core Types                          //////
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

typedef unsigned char nu_byte_t;
typedef int           nu_i32_t;
typedef unsigned int  nu_u32_t;
typedef long          nu_i64_t;
typedef unsigned long nu_u64_t;
typedef float         nu_f32_t;
typedef double        nu_f64_t;

//////////////////////////////////////////////////////////////////////////
//////                            GPU API                           //////
//////////////////////////////////////////////////////////////////////////

// gpu resources
NU_API void write_texture(
    nu_u32_t x, nu_u32_t y, nu_u32_t h, nu_u32_t w, const nu_byte_t *data);
NU_API void write_vertex_buffer(nu_u32_t         first,
                                nu_u32_t         count,
                                const nu_byte_t *data);

// gpu state
NU_API void push_gpu_state(nu_byte_t *p);
NU_API void pop_gpu_state(const nu_byte_t *p);

NU_API void bind_texture(nu_u32_t slot);
NU_API void bind_vertex_buffer(nu_u32_t slot);

// gpu commands
NU_API void draw(nu_u32_t first, nu_u32_t count);
NU_API void blit(
    nu_u32_t x, nu_u32_t y, nu_u32_t dx, nu_u32_t dy, nu_u32_t h, nu_u32_t w);

#endif
