#include "internal.h"

#include "shaders_data.c.inc"

static struct
{
    GLuint screen_blit_program;
    GLuint surface_texture;
} renderer;

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
    (void)source;
    (void)id;
    (void)length;
    if (type == GL_DEBUG_TYPE_OTHER) // Skip other messages
    {
        return;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    message_type_string(type),
                    message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            fprintf(stderr,
                    "GL: %s, message = %s",
                    message_type_string(type),
                    message);
            break;
    }
    NU_ASSERT(severity != GL_DEBUG_SEVERITY_HIGH);
}
static nu_status_t
compile_shader (nu_sv_t source, GLuint shader_type, GLuint *shader)
{
    GLint success;
    *shader                 = glCreateShader(shader_type);
    const GLchar *psource[] = { (const GLchar *)source.ptr };
    const GLint   psize[]   = { source.len };
    glShaderSource(*shader, 1, psource, psize);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &max_length);
        GLchar *log = (GLchar *)malloc(sizeof(GLchar) * max_length);
        glGetShaderInfoLog(*shader, max_length, &max_length, log);
        fprintf(stderr, "Failed to compile shader: %s", log);
        free(log);
        glDeleteShader(*shader);
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
static nu_status_t
compile_program (nu_sv_t vert, nu_sv_t frag, GLuint *program)
{
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    nu_status_t status = NU_SUCCESS;

    status = compile_shader(vert, GL_VERTEX_SHADER, &vertex_shader);
    NU_CHECK(status, goto cleanup0);

    status = compile_shader(frag, GL_FRAGMENT_SHADER, &fragment_shader);
    NU_CHECK(status, goto cleanup1);

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
        fprintf(stderr, "Failed to link shader: %s", log);
        free(log);

        glDeleteProgram(*program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return NU_FAILURE;
    }

    glDeleteShader(fragment_shader);
cleanup1:
    glDeleteShader(vertex_shader);
cleanup0:
    return status;
}

nu_status_t
renderer_init (void)
{
    nu_status_t status = NU_SUCCESS;

    // Initialize GL functions
    if (!gladLoadGL((GLADloadfunc)RGFW_getProcAddress))
    {
        fprintf(stderr, "Failed to load GL functions");
        return NU_FAILURE;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, NU_NULL);

    // Compile shaders
    glEnableVertexAttribArray(0);
    status = compile_program(shader_screen_blit_vert,
                             shader_screen_blit_frag,
                             &renderer.screen_blit_program);
    NU_CHECK(status, goto cleanup0);

    // Create render target
    glGenTextures(1, &renderer.surface_texture);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB,
                 NUX_SCREEN_WIDTH,
                 NUX_SCREEN_HEIGHT,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return status;

cleanup0:
    renderer_free();
    return status;

    return NU_SUCCESS;
}
void
renderer_free (void)
{
    if (renderer.screen_blit_program)
    {
        glDeleteProgram(renderer.screen_blit_program);
    }
    glDeleteTextures(1, &renderer.surface_texture);
}
void
renderer_clear (nu_b2i_t viewport, nu_v2u_t window_size)
{
    nu_v4_t clear
        = nu_color_to_vec4(nu_color_to_linear(nu_color(25, 27, 43, 255)));
    nu_v2i_t pos  = viewport.min;
    nu_v2u_t size = nu_b2i_size(viewport);
    // Patch pos (bottom left in opengl)
    pos.y = window_size.y - (pos.y + size.y);
    glViewport(0, 0, window_size.x, window_size.y);
    glEnable(GL_SCISSOR_TEST);
    glScissor(pos.x, pos.y, size.x, size.y);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(clear.x, clear.y, clear.z, clear.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_SCISSOR_TEST);
}
void
renderer_render_instance (nux_instance_t inst,
                          nu_b2i_t       viewport,
                          nu_v2u_t       window_size)
{
    // Update surface texture
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    NUX_SCREEN_WIDTH,
                    NUX_SCREEN_HEIGHT,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    nux_instance_get_framebuffer(inst));
    glBindTexture(GL_TEXTURE_2D, 0);

    // Blit surface to screen
    nu_v2i_t pos  = viewport.min;
    nu_v2u_t size = nu_b2i_size(viewport);
    // Patch pos (bottom left in opengl)
    pos.y = window_size.y - (pos.y + size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(renderer.screen_blit_program);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(pos.x, pos.y, size.x, size.y);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glUseProgram(0);
}
