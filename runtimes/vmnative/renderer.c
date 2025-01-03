#include "renderer.h"

#include <nulib.h>
#include <vmcore/platform.h>
#include <glad/gl.h>
#define RGFW_IMPORT
#include <rgfw/RGFW.h>

typedef struct
{
    GLuint handle;
} texture_entry_t;

static struct
{
    texture_entry_t *tex64;
    texture_entry_t *tex128;
    texture_entry_t *tex256;
} _renderer;

static const GLchar *
message_type_string (GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
    }
    return "";
}
static void GLAPIENTRY
message_callback (GLenum        source,
                  GLenum        type,
                  GLuint        id,
                  GLenum        severity,
                  GLsizei       length,
                  const GLchar *message,
                  const void   *userParam)
{
    if (type == GL_DEBUG_TYPE_OTHER) // Skip other messages
    {
        return;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            printf("GL: %s, message = %s", message_type_string(type), message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            printf("GL: %s, message = %s", message_type_string(type), message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            printf("GL: %s, message = %s", message_type_string(type), message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            printf("GL: %s, message = %s", message_type_string(type), message);
            break;
    }
    NU_ASSERT(severity != GL_DEBUG_SEVERITY_HIGH);
}
nux_error_code_t
nux_renderer_init (const nux_vm_config_t *config)
{
    // Initialize GL functions
    if (!gladLoadGL((GLADloadfunc)RGFW_getProcAddress))
    {
        return NUX_ERROR(NUX_ERROR_RENDERER_GL_LOADING, NU_NULL);
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, NU_NULL);

    // Initialize resources
    _renderer.tex64 = malloc(sizeof(*_renderer.tex64) * config->gpu_tex64_unit);
    _renderer.tex128
        = malloc(sizeof(*_renderer.tex128) * config->gpu_tex128_unit);
    _renderer.tex256
        = malloc(sizeof(*_renderer.tex256) * config->gpu_tex256_unit);

    return NUX_ERROR_NONE;
}
nux_error_code_t
nux_renderer_free (void)
{
    return NUX_ERROR_NONE;
}
void
nux_renderer_render (void)
{
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
os_write_texture (void             *user,
                  nux_gpu_texture_t type,
                  nu_u32_t          slot,
                  const void       *p)
{
    switch (type)
    {
        case NUX_TEX64:
            break;
        case NUX_TEX128:
            break;
        case NUX_TEX256:
            break;
    }
}

void
os_draw (void *user)
{
}
