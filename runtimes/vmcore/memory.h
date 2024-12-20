#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include "common.h"

NU_API nu_int_t nu_memcmp(const void *p0, const void *p1, nu_size_t n);
NU_API void    *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void     nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void     nu_memswp(void *a, void *b, nu_size_t n);
NU_API void    *nu_memalign(void *ptr, nu_size_t align);

#endif
