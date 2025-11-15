#ifndef NUX_CONFIG_H
#define NUX_CONFIG_H

#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NUX_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Ensure ppoll definition for glfw
#endif
#define NUX_PLATFORM_UNIX
#elif defined(__APPLE__)
#define NUX_PLATFORM_APPLE
#endif

#define nux_version_major(num)          ((num >> 16) & 0xFF)
#define nux_version_minor(num)          ((num >> 8) & 0xFF)
#define nux_version_patch(num)          (num & 0xFF)
#define nux_version_make(maj, min, pat) (maj << 16 | min << 8 | pat)

#define NUX_VERSION nux_version_make(0, 0, 1)

#define NUX_API extern

#define NUX_BUILD_VARARGS
#define NUX_BUILD_IMPORTER

#ifndef NDEBUG
#define NUX_BUILD_DEBUG
#endif
#define NUX_BUILD_STDLIB

#endif
