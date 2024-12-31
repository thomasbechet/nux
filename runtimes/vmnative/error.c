#include "error.h"

static struct
{
    nux_error_code_t code;
    nux_error_t      error;
    const nu_byte_t *source;
} _error;

nux_error_code_t
nux_error (nux_error_code_t   code,
           const nu_byte_t   *source,
           const nux_error_t *error)
{
    _error.code   = code;
    _error.source = source;
    if (error)
    {
        _error.error = *error;
    }
    return code;
}
void
nux_error_print (void)
{
    switch (_error.code)
    {
        case NUX_ERROR_NONE:
            return;
        case NUX_ERROR_RENDERER_GL_LOADING:
            fprintf(stderr, "Failed to load GL functions\n");
            break;
    }
    if (_error.source)
    {
        fprintf(stderr, "source: %s\n", _error.source);
    }
}
