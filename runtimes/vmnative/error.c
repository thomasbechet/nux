#include "error.h"

void
vmn_error_print (const vmn_error_t *error)
{
    switch (error->code)
    {
        case VMN_ERROR_VM:
            fprintf(stderr, "Failed to initialize VM:\n");
            vm_error_print(&error->vm);
            break;
        case VMN_ERROR_RENDERER_GL_LOADING:
            fprintf(stderr, "Failed to load GL functions\n");
            break;
        case VMN_ERROR_RENDERER_SHADER_COMPILATION:
            fprintf(stderr, "Failed to compile GL shader\n");
            if (error->shader_log)
            {
                fprintf(stderr, "Error: %s\n", error->shader_log);
            }
            break;
    }
}
void
vmn_error_free (vmn_error_t *error)
{
    switch (error->code)
    {
        case VMN_ERROR_VM:
            vm_error_free(&error->vm);
            break;
        case VMN_ERROR_RENDERER_SHADER_COMPILATION:
            if (error->shader_log)
            {
                free(error->shader_log);
            }
            break;
        default:
            break;
    }
}
