#ifndef NUX_NATIVE_ERROR_H
#define NUX_NATIVE_ERROR_H

#include <nulib.h>

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef __FILE_NAME__
#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__ " "
#else
#define __SOURCE__ ""
#endif

#define NUX_ERROR(code, error) \
    nux_error(code, (const nu_byte_t *)__SOURCE__, error)

typedef struct
{
} nux_error_t;

typedef enum
{
    NUX_ERROR_NONE = 0,
    NUX_ERROR_RENDERER_GL_LOADING
} nux_error_code_t;

nux_error_code_t nux_error(nux_error_code_t   code,
                           const nu_byte_t   *source,
                           const nux_error_t *error);
void             nux_error_print(void);

#endif
