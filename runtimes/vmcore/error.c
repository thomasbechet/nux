#include "error.h"

void
vm_error_print (const vm_error_t *error)
{
    switch (error->code)
    {
        case VM_ERROR_IO:
            break;
    }
}
void
vm_error_free (vm_error_t *error)
{
}
