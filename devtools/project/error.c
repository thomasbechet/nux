#include "error.h"

void
project_error_print (const project_error_t *error)
{
    switch (error->code)
    {
        case PROJECT_ERROR_IO_CART:
            break;
        case PROJECT_ERROR_IO_PROJECT:
            break;
        case PROJECT_ERROR_MALFORMED:
            break;
    }
}
