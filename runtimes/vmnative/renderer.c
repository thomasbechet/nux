#include "renderer.h"

#include "shaders_data.h"

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
    GLuint unlit_shader;
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
    // NU_ASSERT(severity != GL_DEBUG_SEVERITY_HIGH);
}
static vmn_error_code_t
compile_shader (nu_sv_t source, GLuint shader_type, GLuint *shader)
{
    GLint success;
    *shader                 = glCreateShader(shader_type);
    const GLchar *psource[] = { (const GLchar *)source.data };
    const GLint   psize[]   = { source.size };
    glShaderSource(*shader, 1, psource, psize);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &max_length);
        vmn_error_data_t error;
        GLchar          *log = (GLchar *)malloc(sizeof(GLchar) * max_length);
        error.shader_log     = log;
        glGetShaderInfoLog(*shader, max_length, &max_length, log);
        glDeleteShader(*shader);
        return VMN_ERROR(VMN_ERROR_RENDERER_SHADER_COMPILATION, &error);
    }
    return VMN_ERROR_NONE;
}
static vmn_error_code_t
compile_program (nu_sv_t vert, nu_sv_t frag, GLuint *program)
{
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    vmn_error_code_t error = VMN_ERROR_NONE;

    error = compile_shader(vert, GL_VERTEX_SHADER, &vertex_shader);
    NU_CHECK(!error, goto cleanup0);

    error = compile_shader(frag, GL_FRAGMENT_SHADER, &fragment_shader);
    NU_CHECK(!error, goto cleanup1);

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader);
    glAttachShader(*program, fragment_shader);

    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &max_length);
        GLchar *log = (GLchar *)malloc(sizeof(GLchar) * max_length);
        glGetProgramInfoLog(*program, max_length, &max_length, log);

        glDeleteProgram(*program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        vmn_error_data_t error_data;
        error_data.shader_log = log;
        return VMN_ERROR(VMN_ERROR_RENDERER_SHADER_COMPILATION, &error_data);
    }

    glDeleteShader(fragment_shader);
cleanup1:
    glDeleteShader(vertex_shader);
cleanup0:
    return error;
}
vmn_error_code_t
vmn_renderer_init (const vm_config_t *config)
{
    vmn_error_code_t error = VMN_ERROR_NONE;

    // Initialize GL functions
    if (!gladLoadGL((GLADloadfunc)RGFW_getProcAddress))
    {
        return VMN_ERROR(VMN_ERROR_RENDERER_GL_LOADING, NU_NULL);
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

    // Compile shaders
    error = compile_program(
        shader_unlit_vert, shader_unlit_frag, &_renderer.unlit_shader);
    NU_CHECK(!error, goto cleanup0);

cleanup0:
    return error;
}
vmn_error_code_t
vmn_renderer_free (void)
{
    glDeleteProgram(_renderer.unlit_shader);
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
    return VMN_ERROR_NONE;
}
void
vmn_renderer_render (void)
{
    // glClearColor(1, 0, 0, 1);
    // glClear(GL_COLOR_BUFFER_BIT);
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
                     VM_TEXTURE_SIZE,
                     VM_TEXTURE_SIZE,
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
            = data[(first + i) * VM_VERTEX_SIZE_F32 + 0];
        ptr[(first + i) * NU_V3_SIZE + 1]
            = data[(first + i) * VM_VERTEX_SIZE_F32 + 1];
        ptr[(first + i) * NU_V3_SIZE + 2]
            = data[(first + i) * VM_VERTEX_SIZE_F32 + 2];
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    // uvs
    glBindBuffer(GL_ARRAY_BUFFER, _renderer.vbo_uvs);
    ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < count; ++i)
    {
        ptr[(first + i) * NU_V2_SIZE + 0]
            = data[(first + i) * VM_VERTEX_SIZE_F32 + 3];
        ptr[(first + i) * NU_V2_SIZE + 1]
            = data[(first + i) * VM_VERTEX_SIZE_F32 + 4];
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
os_draw (void *user, nu_u32_t first, nu_u32_t count)
{
    glUseProgram(_renderer.unlit_shader);
    glBindVertexArray(_renderer.vao);
    glDrawArrays(GL_TRIANGLES, first, count);
    glBindVertexArray(0);
}
