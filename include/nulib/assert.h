#ifndef NU_ERROR_H
#define NU_ERROR_H

#include <assert.h>

#if !defined(NU_NDEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#if defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#define NU_UNREACHABLE() NU_ASSERT(NU_FALSE)

#define _NU_CHECK(check, action, source) \
    if (!(check))                        \
    {                                    \
        action;                          \
    }
#define NU_CHECK(check, action) _NU_CHECK(check, action, __SOURCE__)
#define NU_CHECK_ERROR(error, action) \
    _NU_CHECK(error == NU_ERROR_NONE, action, __SOURCE__)
#define NU_CHECK_PANIC(check, format, ...) \
    if (!(check))                          \
    {                                      \
        NU_PANIC(format, ##__VA_ARGS__);   \
    }

#endif
