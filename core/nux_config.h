#ifndef NUX_CONFIG_H
#define NUX_CONFIG_H

#define NUX_VERSION_MAJOR(num)          ((num >> 16) & 0xFF)
#define NUX_VERSION_MINOR(num)          ((num >> 8) & 0xFF)
#define NUX_VERSION_PATCH(num)          (num & 0xFF)
#define NUX_VERSION_MAKE(maj, min, pat) (maj << 16 | min << 8 | pat)
#define NUX_VERSION                     NUX_VERSION_MAKE(0, 0, 1)

#define NUX_API extern

#define NUX_BUILD_VARARGS
#define NUX_BUILD_WASM3

// #define NUX_BENCHMARK 1

#endif
