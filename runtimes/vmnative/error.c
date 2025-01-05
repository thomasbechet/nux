#include "error.h"

static struct
{
    nux_error_code_t code;
    nux_error_data_t data;
    const nu_byte_t *source;
} _error;

nux_error_code_t
nux_error (nux_error_code_t        code,
           const nu_byte_t        *source,
           const nux_error_data_t *error)
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
nux_error_init (void)
{
    _error.code   = NUX_ERROR_NONE;
    _error.source = NU_NULL;
    nu_memset(&_error.data, 0, sizeof(_error.data));
}
void
nux_error_free (void)
{
    switch (_error.code)
    {
        case NUX_ERROR_RENDERER_SHADER_COMPILATION:
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
nux_error_print (void)
{
    switch (_error.code)
    {
        case NUX_ERROR_NONE:
            return;
        case NUX_ERROR_VM_INITIALIZATION:
            fprintf(stderr, "Failed to initialize VM\n");
            break;
        case NUX_ERROR_RENDERER_GL_LOADING:
            fprintf(stderr, "Failed to load GL functions\n");
            break;
        case NUX_ERROR_RENDERER_SHADER_COMPILATION:
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
