#ifndef NU_ERROR_H
#define NU_ERROR_H

#include <assert.h>

#if defined(NU_DEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#define NU_UNREACHABLE() NU_ASSERT(NU_FALSE)

#define NU_CHECK(check, action) \
    if (!(check))               \
    {                           \
        action;                 \
    }

#endif
