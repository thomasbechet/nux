#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include "platform.h"
#include "types.h"

#define NU_MEM_1K   (1 << 10)
#define NU_MEM_2K   (1 << 11)
#define NU_MEM_4K   (1 << 12)
#define NU_MEM_8K   (1 << 13)
#define NU_MEM_16K  (1 << 14)
#define NU_MEM_32K  (1 << 15)
#define NU_MEM_64K  (1 << 16)
#define NU_MEM_1M   (1 << 20)
#define NU_MEM_2M   (1 << 21)
#define NU_MEM_4M   (1 << 22)
#define NU_MEM_8M   (1 << 23)
#define NU_MEM_16M  (1 << 24)
#define NU_MEM_32M  (1 << 25)
#define NU_MEM_64M  (1 << 26)
#define NU_MEM_128M (1 << 27)
#define NU_MEM_256M (1 << 28)
#define NU_MEM_512M (1 << 29)
#define NU_MEM_1G   (1 << 30)
#define NU_MEM_2G   (1 << 31)
#define NU_MEM_4G   (1 << 32)

NU_API nu_int_t nu_memcmp(const void *p0, const void *p1, nu_size_t n);
NU_API void    *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void     nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void     nu_memswp(void *a, void *b, nu_size_t n);
NU_API void    *nu_memalign(void *ptr, nu_size_t align);

NU_API nu_u32_t nu_u32_le(nu_u32_t v);
NU_API nu_f32_t nu_f32_le(nu_f32_t v);

#endif
