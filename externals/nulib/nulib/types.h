#ifndef NU_TYPES_H
#define NU_TYPES_H

#ifdef NU_STDLIB
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#endif

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

#define NU_DEFAULT_ALIGN   16
#define NU_COLOR_WHITE     nu_color(255, 255, 255, 255)
#define NU_COLOR_GREY      nu_color(128, 128, 128, 255)
#define NU_COLOR_BLACK     nu_color(0, 0, 0, 255)
#define NU_COLOR_RED       nu_color(255, 0, 0, 255)
#define NU_COLOR_GREEN     nu_color(0, 255, 0, 255)
#define NU_COLOR_BLUE      nu_color(0, 0, 255, 255)
#define NU_COLOR_BLUE_SKY  nu_color(52, 181, 248, 255)
#define NU_COLOR_BLUE_DARK nu_color(25, 27, 43, 255)
#define NU_PI              3.14159265359
#define NU_FLT_MAX         3.402823E+38
#define NU_FLT_MIN         1.175494e-38
#define NU_I32_MAX         2147483647
#define NU_I32_MIN         -2147483648

#define NU_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define NU_CLAMP(x, min, max) (NU_MAX(min, NU_MIN(max, x)))
#define NU_CLAMP01(x)         (NU_CLAMP(x, 0, 1))
#define NU_ABS(x)             (((x) < 0) ? -(x) : (x))
#define NU_SWAP(x, y, T) \
    do                   \
    {                    \
        T SWAP = x;      \
        x      = y;      \
        y      = SWAP;   \
    } while (0)

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
typedef uint8_t  nu_u8_t;
typedef int8_t   nu_i8_t;
typedef uint16_t nu_u16_t;
typedef int16_t  nu_i16_t;
typedef int32_t  nu_i32_t;
typedef uint32_t nu_u32_t;
typedef int64_t  nu_i64_t;
typedef uint64_t nu_u64_t;
typedef float    nu_f32_t;
typedef double   nu_f64_t;

typedef int      nu_bool_t;
typedef nu_i32_t nu_int_t;
typedef size_t   nu_size_t;
typedef intptr_t nu_intptr_t;
typedef nu_u8_t  nu_byte_t;
typedef int      nu_word_t;
typedef char     nu_char_t;
typedef nu_i32_t nu_wchar_t;

typedef enum
{
    NU_SUCCESS = 1,
    NU_FAILURE = 0
} nu_status_t;

typedef enum
{
    NU_LOG_DEBUG   = 4,
    NU_LOG_INFO    = 3,
    NU_LOG_WARNING = 2,
    NU_LOG_ERROR   = 1,
} nu_log_level_t;

#endif
