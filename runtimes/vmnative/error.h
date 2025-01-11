#ifndef VMN_ERROR_H
#define VMN_ERROR_H

#include <nulib.h>

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef __FILE_NAME__
#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__ " "
#else
#define __SOURCE__ ""
#endif

#define VMN_ERROR(code, error) \
    vmn_error(code, (const nu_byte_t *)__SOURCE__, error)

typedef struct
{
    char *shader_log;
} vmn_error_data_t;

typedef enum
{
    VMN_ERROR_NONE = 0,
    VMN_ERROR_VM_INITIALIZATION,
    VMN_ERROR_RENDERER_GL_LOADING,
    VMN_ERROR_RENDERER_SHADER_COMPILATION,
} vmn_error_code_t;

vmn_error_code_t vmn_error(vmn_error_code_t        code,
                           const nu_byte_t        *source,
                           const vmn_error_data_t *error);
void             vmn_error_init(void);
void             vmn_error_free(void);
void             vmn_error_print(void);

#endif
