#ifndef VM_ERROR_H
#define VM_ERROR_H

#include <nulib.h>

typedef enum
{
    VM_ERROR_IO
} vm_error_code_t;

typedef struct
{
    vm_error_code_t code;
} vm_error_t;

NU_API void vm_error_print(const vm_error_t *error);
NU_API void vm_error_free(vm_error_t *error);

#endif
