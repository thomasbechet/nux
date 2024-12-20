#ifndef NU_CONFIG_H
#define NU_CONFIG_H

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

#endif
