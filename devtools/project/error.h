#ifndef PROJECT_ERROR_H
#define PROJECT_ERROR_H

#include <nulib.h>

typedef enum
{
    PROJECT_ERROR_IO_CART,
    PROJECT_ERROR_IO_PROJECT,
    PROJECT_ERROR_MALFORMED
} project_error_code_t;

typedef struct
{
    project_error_code_t code;
} project_error_t;

NU_API void project_error_print(const project_error_t *error);

#endif
