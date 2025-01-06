#ifndef NU_TYPES_H
#define NU_TYPES_H

#define NU_STDLIB
#ifdef NU_STDLIB
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

#define NU_DEFAULT_ALIGN  16
#define NU_COLOR_WHITE    nu_color(255, 255, 255, 0)
#define NU_COLOR_BLACK    nu_color(0, 0, 0, 0)
#define NU_COLOR_RED      nu_color(255, 0, 0, 0)
#define NU_COLOR_GREEN    nu_color(0, 255, 0, 0)
#define NU_COLOR_BLUE     nu_color(0, 0, 255, 0)
#define NU_COLOR_BLUE_SKY nu_color(52, 181, 248, 0)
#define NU_PI             3.14159265359
#define NU_FLT_MAX        3.402823E+38

#define NU_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define NU_CLAMP(x, min, max) (NU_MAX(min, NU_MIN(max, x)))
#define NU_CLAMP01(x)         (NU_CLAMP(x, 0, 1))

#define NU_V2_SIZE 2
#define NU_V3_SIZE 3
#define NU_V4_SIZE 4
#define NU_Q4_SIZE 4
#define NU_M3_SIZE 9
#define NU_M4_SIZE 16

#define NU_V2_ZEROS nu_v2(0, 0)
#define NU_V2_ONES  nu_v2(1, 1)

#define NU_V2I_ZEROS nu_v2i(0, 0)

#define NU_V2U_ZEROS nu_v2u(0, 0)
#define NU_V2U_ONES  nu_v2u(1, 1)

#define NU_V3U_ONES nu_v3u(1, 1, 1)

#define NU_V3_ZEROS    nu_v3(0, 0, 0)
#define NU_V3_ONES     nu_v3(1, 1, 1)
#define NU_V3_UP       nu_v3(0, 1, 0)
#define NU_V3_DOWN     nu_v3(0, -1, 0)
#define NU_V3_FORWARD  nu_v3(0, 0, -1)
#define NU_V3_BACKWARD nu_v3(0, 0, 1)
#define NU_V3_LEFT     nu_v3(-1, 0, 0)
#define NU_V3_RIGHT    nu_v3(1, 0, 0)

#define NU_V4_ZEROS nu_vec4(0, 0, 0, 0)

// TODO: use stdint types
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef int            nu_i32_t;
typedef unsigned int   nu_u32_t;
typedef long           nu_i64_t;
typedef unsigned long  nu_u64_t;
typedef float          nu_f32_t;
typedef double         nu_f64_t;

typedef int           nu_bool_t;
typedef nu_i32_t      nu_int_t;
typedef unsigned long nu_size_t;
typedef intptr_t      nu_intptr_t;
typedef nu_u8_t       nu_byte_t;
typedef int           nu_word_t;
typedef char          nu_char_t;
typedef nu_i32_t      nu_wchar_t;

#endif
