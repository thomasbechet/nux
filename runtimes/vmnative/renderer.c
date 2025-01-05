#include "renderer.h"

#include <nulib.h>
#include <vmcore/platform.h>
#include <glad/gl.h>
#define RGFW_IMPORT
#include <rgfw/RGFW.h>

#define MAX_TEXTURE 256

static struct
{
    GLuint textures[MAX_TEXTURE];
    GLuint vbo_positions;
    GLuint vbo_uvs;
    GLuint vbo_normals;
    GLuint vao;
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

    // Initialize textures
    nu_memset(
        &_renderer.textures, 0, sizeof(*_renderer.textures) * MAX_TEXTURE);

    // Initialize vertices
    glGenBuffers(1, &_renderer.vbo_positions);
    glGenBuffers(1, &_renderer.vbo_uvs);
    glGenBuffers(1, &_renderer.vbo_normals);
    glGenVertexArrays(1, &_renderer.vao);
    glBindVertexArray(_renderer.vao);
    // positions
    glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_positions);
    glBufferData(GL_ARRAY_BUFFER,
                 config->gpu_vertex_count * NU_V3_SIZE * sizeof(nu_f32_t),
                 NU_NULL,
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V3_SIZE, (void *)0);
    glEnableVertexAttribArray(0);
    // uvs
    glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_uvs);
    glBufferData(GL_ARRAY_BUFFER,
                 config->gpu_vertex_count * NU_V2_SIZE * sizeof(nu_f32_t),
                 NU_NULL,
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V2_SIZE, (void *)0);
    glEnableVertexAttribArray(1);
    // normals
    // TODO
    // glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_normals);
    // glBufferData(GL_ARRAY_BUFFER,
    //              config->gpu_vertex_count * NU_V3_SIZE * sizeof(nu_f32_t),
    //              NU_NULL,
    //              GL_DYNAMIC_DRAW);
    // glVertexAttribPointer(
    //     2, 3, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V3_SIZE, (void *)0);
    // glEnableVertexAttribArray(2);

    return NUX_ERROR_NONE;
}
nux_error_code_t
nux_renderer_free (void)
{
    for (nu_size_t i = 0; i < MAX_TEXTURE; ++i)
    {
        if (_renderer.textures[i])
        {
            glDeleteTextures(1, _renderer.textures + i);
        }
    }
    glDeleteVertexArrays(1, &_renderer.vao);
    glDeleteBuffers(1, &_renderer.vbo_positions);
    glDeleteBuffers(1, &_renderer.vbo_uvs);
    // glDeleteBuffers(1, &_renderer.vbo_normals);
    return NUX_ERROR_NONE;
}
void
nux_renderer_render (void)
{
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
os_write_texture (void       *user,
                  nu_u32_t    slot,
                  nu_u32_t    x,
                  nu_u32_t    y,
                  nu_u32_t    w,
                  nu_u32_t    h,
                  const void *p)
{
    GLuint handle = _renderer.textures[slot];
    if (!handle)
    {
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     NUX_TEXTURE_SIZE,
                     NUX_TEXTURE_SIZE,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     NU_NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        _renderer.textures[slot] = handle;
    }

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, p);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void
os_write_vertex (void *user, nu_u32_t first, nu_u32_t count, const void *p)
{
    const nu_f32_t *data = p;
    nu_f32_t       *ptr  = NU_NULL;
    // positions
    glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_positions);
    ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < count; ++i)
    {
        ptr[(first + i) * NU_V3_SIZE + 0]
            = data[(first + i) * NUX_VERTEX_SIZE_F32 + 0];
        ptr[(first + i) * NU_V3_SIZE + 1]
            = data[(first + i) * NUX_VERTEX_SIZE_F32 + 1];
        ptr[(first + i) * NU_V3_SIZE + 2]
            = data[(first + i) * NUX_VERTEX_SIZE_F32 + 2];
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    // uvs
    glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_uvs);
    ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < count; ++i)
    {
        ptr[(first + i) * NU_V2_SIZE + 0]
            = data[(first + i) * NUX_VERTEX_SIZE_F32 + 3];
        ptr[(first + i) * NU_V2_SIZE + 1]
            = data[(first + i) * NUX_VERTEX_SIZE_F32 + 4];
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // normals
    // TODO
}
void
os_bind_texture (void *user, nu_u32_t slot)
{
}
void
os_draw (void *user)
{
}
