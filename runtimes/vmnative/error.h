#ifndef VMN_ERROR_H
#define VMN_ERROR_H

#include <nulib.h>
#include <vmcore/vm.h>

typedef enum
{
    VMN_ERROR_VM,
    VMN_ERROR_RENDERER_GL_LOADING,
    VMN_ERROR_RENDERER_SHADER_COMPILATION,
} vmn_error_code_t;

typedef struct
{
    vmn_error_code_t code;
    nu_char_t       *shader_log;
    vm_error_t       vm;
} vmn_error_t;

void vmn_error_print(const vmn_error_t *error);
void vmn_error_free(vmn_error_t *error);

#endif
