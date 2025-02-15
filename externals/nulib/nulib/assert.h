#ifndef NU_ERROR_H
#define NU_ERROR_H

#include <assert.h>

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef __FILE_NAME__
#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__ " "
#else
#define __SOURCE__ ""
#endif

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
