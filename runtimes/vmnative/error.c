#include "error.h"

static struct
{
    vmn_error_code_t code;
    vmn_error_data_t data;
    const nu_byte_t *source;
} _error;

vmn_error_code_t
vmn_error (vmn_error_code_t        code,
           const nu_byte_t        *source,
           const vmn_error_data_t *error)
{
    _error.code   = code;
    _error.source = source;
    if (error)
    {
        _error.data = *error;
    }
    return code;
}
void
vmn_error_init (void)
{
    _error.code   = VMN_ERROR_NONE;
    _error.source = NU_NULL;
    nu_memset(&_error.data, 0, sizeof(_error.data));
}
void
vmn_error_free (void)
{
    switch (_error.code)
    {
        case VMN_ERROR_RENDERER_SHADER_COMPILATION:
            if (_error.data.shader_log)
            {
                free(_error.data.shader_log);
            }
            break;
        default:
            break;
    }
}
void
vmn_error_print (void)
{
    switch (_error.code)
    {
        case VMN_ERROR_NONE:
            return;
        case VMN_ERROR_VM_INITIALIZATION:
            fprintf(stderr, "Failed to initialize VM\n");
            break;
        case VMN_ERROR_RENDERER_GL_LOADING:
            fprintf(stderr, "Failed to load GL functions\n");
            break;
        case VMN_ERROR_RENDERER_SHADER_COMPILATION:
            fprintf(stderr, "Failed to compile GL shader\n");
            if (_error.data.shader_log)
            {
                fprintf(stderr, "Error: %s\n", _error.data.shader_log);
            }
            break;
    }
    if (_error.source)
    {
        fprintf(stderr, "source: %s\n", _error.source);
    }
}
