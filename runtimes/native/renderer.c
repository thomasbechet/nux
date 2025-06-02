#include "internal.h"

#include "shaders_data.c.inc"

static struct
{
    GLuint empty_vao;
} renderer;

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

nux_status_t
nux_os_create_pipeline (void                   *userdata,
                        nux_u32_t               slot,
                        nux_gpu_shader_source_t type,
                        const nux_c8_t         *vertex,
                        nux_u32_t               vertex_len,
                        const nux_c8_t         *fragment,
                        nux_u32_t               fragment_len)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->programs), return NUX_FAILURE);
    nu_status_t status = compile_program(nu_sv(vertex, vertex_len),
                                         nu_sv(fragment, fragment_len),
                                         &inst->programs[slot]);
    NU_CHECK(status, return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_texture (void                         *userdata,
                       nux_u32_t                     slot,
                       const nux_gpu_texture_info_t *info)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->textures), return NUX_FAILURE);
    texture_t *tex = inst->textures + slot;

    switch (info->format)
    {
        case NUX_GPU_TEXTURE_FORMAT_RGBA:
            tex->internal_format = GL_RGBA8;
            tex->format          = GL_RGB;
            break;
        case NUX_GPU_TEXTURE_FORMAT_INDEX:
            tex->internal_format = GL_R8UI;
            tex->format          = GL_RED_INTEGER;
            break;
    }

    switch (info->filter)
    {
        case NUX_GPU_TEXTURE_FILTER_LINEAR:
            tex->filtering = GL_LINEAR;
            break;
        case NUX_GPU_TEXTURE_FILTER_NEAREST:
            tex->filtering = GL_NEAREST;
            break;
    }

    if (!tex->handle)
    {
        glGenTextures(1, &tex->handle);
    }
    glBindTexture(GL_TEXTURE_2D, tex->handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 tex->internal_format,
                 info->width,
                 info->height,
                 0,
                 tex->format,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->filtering);
    glBindTexture(GL_TEXTURE_2D, 0);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_update_texture (void       *userdata,
                       nux_u32_t   slot,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->textures), return NUX_FAILURE);
    texture_t *tex = inst->textures + slot;

    glBindTexture(GL_TEXTURE_2D, tex->handle);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, x, y, w, h, tex->format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_buffer (void                 *userdata,
                      nux_u32_t             slot,
                      nux_gpu_buffer_type_t type,
                      nux_u32_t             size)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->buffers), return NUX_FAILURE);
    buffer_t *buffer = inst->buffers + slot;

    switch (type)
    {
        case NUX_GPU_BUFFER_UNIFORM:
            buffer->buffer_type = GL_SHADER_STORAGE_BUFFER;
            break;
        case NUX_GPU_BUFFER_STORAGE:
            buffer->buffer_type = GL_UNIFORM_BUFFER;
            break;
    }

    if (!buffer->handle)
    {
        glGenBuffers(1, &buffer->handle);
    }
    glBindBuffer(buffer->buffer_type, buffer->handle);
    glBufferData(buffer->buffer_type, size, NU_NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(buffer->buffer_type, 0, buffer->handle);
    glBindBuffer(buffer->buffer_type, 0);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_update_buffer (void       *userdata,
                      nux_u32_t   slot,
                      nux_u32_t   offset,
                      nux_u32_t   size,
                      const void *data)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->buffers), return NUX_FAILURE);
    buffer_t *buffer = inst->buffers + slot;

    glBindBuffer(buffer->buffer_type, buffer->handle);
    glBufferSubData(buffer->buffer_type, offset, size, data);
    glBindBuffer(buffer->buffer_type, 0);

    return NUX_SUCCESS;
}
void
nux_os_submit_commands (void                    *userdata,
                        const nux_gpu_command_t *commands,
                        nux_u32_t                count)
{
    runtime_instance_t *inst = userdata;
    for (nux_u32_t i = 0; i < count; ++i)
    {
        const nux_gpu_command_t *cmd = commands + i;
        switch (cmd->type)
        {
            case NUX_GPU_BIND_PIPELINE: {
                glUseProgram(inst->programs[cmd->data.bind_pipeline.slot]);
            }
            break;
            case NUX_GPU_BIND_TEXTURE: {
                glActiveTexture(GL_TEXTURE0 + cmd->data.bind_texture.binding);
                glBindTexture(
                    GL_TEXTURE_2D,
                    inst->textures[cmd->data.bind_texture.slot].handle);
                glUniform1i(cmd->data.bind_texture.binding,
                            cmd->data.bind_texture.binding);
            }
            break;
            case NUX_GPU_BIND_BUFFER: {
                buffer_t *buffer = inst->buffers + cmd->data.bind_buffer.slot;
                glBindBufferBase(buffer->buffer_type,
                                 cmd->data.bind_buffer.binding,
                                 buffer->handle);
            }
            break;
            case NUX_GPU_DRAW: {
                glBindVertexArray(renderer.empty_vao);
                glDrawArrays(GL_TRIANGLES, 0, cmd->data.draw.count);
                glBindVertexArray(0);
            }
            break;
        }
    }
}

nu_status_t
renderer_init (void)
{
    nu_status_t status = NU_SUCCESS;

    // Create empty vao
    glGenVertexArrays(1, &renderer.empty_vao);

    return status;
}
void
renderer_free (void)
{
    if (renderer.empty_vao)
    {
        glDeleteVertexArrays(1, &renderer.empty_vao);
    }
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
renderer_render_begin (nux_instance_t *inst,
                       nu_b2i_t        viewport,
                       nu_v2u_t        window_size)
{
    // Blit surface to screen
    nu_v2i_t pos  = viewport.min;
    nu_v2u_t size = nu_b2i_size(viewport);
    pos.y
        = window_size.y - (pos.y + size.y); // Patch pos (bottom left in opengl)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(pos.x, pos.y, size.x, size.y);
}
void
renderer_render_end (nux_instance_t *inst,
                     nu_b2i_t        viewport,
                     nu_v2u_t        window_size)
{
    glDisable(GL_FRAMEBUFFER_SRGB);
    glUseProgram(0);
}
