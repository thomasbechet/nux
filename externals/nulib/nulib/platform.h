#ifndef NU_PLATFORM_H
#define NU_PLATFORM_H

//////////////////////////////////////////////////////////////////////////
//////                       Platform Detection                     //////
//////////////////////////////////////////////////////////////////////////

#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NU_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Ensure ppoll definition for glfw
#endif
#define NU_PLATFORM_UNIX
#elif defined(__APPLE__)
#define NU_PLATFORM_APPLE
#endif

#ifdef __cplusplus
#define NU_CXX
#endif

#ifndef NDEBUG
#define NU_DEBUG
#endif

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

#endif

#define NU_API NU_API_EXPORT

#ifdef NU_CXX
#define NU_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x0100)
#else
#define NU_BIG_ENDIAN (!*(unsigned char *)&(uint16_t) { 1 })
#endif

#define NU_UNUSED0()
#define NU_UNUSED1(a)             (void)(a)
#define NU_UNUSED2(a, b)          (void)(a), NU_UNUSED1(b)
#define NU_UNUSED3(a, b, c)       (void)(a), NU_UNUSED2(b, c)
#define NU_UNUSED4(a, b, c, d)    (void)(a), NU_UNUSED3(b, c, d)
#define NU_UNUSED5(a, b, c, d, e) (void)(a), NU_UNUSED4(b, c, d, e)

#define NU_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define NU_VA_NUM_ARGS(...) \
    NU_VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define NU_ALL_UNUSED_IMPL_(nargs) NU_UNUSED##nargs
#define NU_ALL_UNUSED_IMPL(nargs)  NU_ALL_UNUSED_IMPL_(nargs)
#define NU_UNUSED(...) \
    NU_ALL_UNUSED_IMPL(NU_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#ifdef NU_CXX
#define NU_VOID_CAST(type, expr) (static_cast<decltype(type)>(expr))
#else
#define NU_VOID_CAST(type, expr) (expr)
#endif

#endif
