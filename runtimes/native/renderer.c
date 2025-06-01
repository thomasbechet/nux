#include "internal.h"

#include "shaders_data.c.inc"

static struct
{
    GLuint canvas;
    GLuint colormap;
    GLuint texture;
    GLuint buffer;

    GLuint canvas_blit_program;
    GLuint canvas_fbo;
    GLuint empty_vao;
} renderer;

// static nu_v2u_t resolution;

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
nux_os_create_storage_buffer (void *userdata, nux_u32_t slot, nux_u32_t size)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->storage_buffers), return NUX_FAILURE);
    storage_buffer_t *buffer = inst->storage_buffers + slot;

    if (!buffer->handle)
    {
        glGenTextures(1, &buffer->handle);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(nux_f32_t) * size,
                 NU_NULL,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_update_storage_buffer (void            *userdata,
                              nux_u32_t        slot,
                              nux_u32_t        first,
                              nux_u32_t        count,
                              const nux_f32_t *data)
{
    runtime_instance_t *inst = userdata;
    NU_CHECK(slot < NU_ARRAY_SIZE(inst->storage_buffers), return NUX_FAILURE);
    storage_buffer_t *buffer = inst->storage_buffers + slot;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    first * sizeof(nux_f32_t),
                    count * sizeof(nux_f32_t),
                    data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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
                glUniform1i(0, 0);
                glUniform1i(1, 1);
            }
            break;
            case NUX_GPU_BIND_TEXTURE: {
                glActiveTexture(GL_TEXTURE0 + cmd->data.bind_texture.binding);
                glBindTexture(
                    GL_TEXTURE_2D,
                    inst->textures[cmd->data.bind_texture.slot].handle);
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

    // Compile canvas blit program
    // status = compile_program(shader_canvas_blit_vert,
    //                          shader_canvas_blit_frag,
    //                          &renderer.canvas_blit_program);
    // NU_CHECK(status, goto cleanup0);
    // glUseProgram(renderer.canvas_blit_program);
    // glUniform1i(glGetUniformLocation(renderer.canvas_blit_program,
    // "t_canvas"),
    //             0);
    // glUniform1i(
    //     glGetUniformLocation(renderer.canvas_blit_program, "t_colormap"), 1);
    // glUseProgram(0);

    // Create canvas
    // glGenTextures(1, &renderer.canvas);
    // glBindTexture(GL_TEXTURE_2D, renderer.canvas);
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_R8UI,
    //              NUX_CANVAS_WIDTH,
    //              NUX_CANVAS_HEIGHT,
    //              0,
    //              GL_RED_INTEGER,
    //              GL_UNSIGNED_BYTE,
    //              NU_NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Create texture
    // glGenTextures(1, &renderer.texture);
    // glBindTexture(GL_TEXTURE_2D, renderer.texture);
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_R8UI,
    //              NUX_TEXTURE_WIDTH,
    //              NUX_TEXTURE_HEIGHT,
    //              0,
    //              GL_RED_INTEGER,
    //              GL_UNSIGNED_BYTE,
    //              NU_NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Create colormap
    // glGenTextures(1, &renderer.colormap);
    // glBindTexture(GL_TEXTURE_2D, renderer.colormap);
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_RGB8,
    //              NUX_COLORMAP_SIZE,
    //              1,
    //              0,
    //              GL_RGB,
    //              GL_UNSIGNED_BYTE,
    //              NU_NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Create buffer
    // glGenBuffers(1, &renderer.buffer);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer.buffer);

    // Create empty vao
    glGenVertexArrays(1, &renderer.empty_vao);

    return status;

    // renderer_free();
    // return status;

    // return NU_SUCCESS;
}
void
renderer_free (void)
{
    if (renderer.empty_vao)
    {
        glDeleteVertexArrays(1, &renderer.empty_vao);
    }
    if (renderer.canvas_blit_program)
    {
        glDeleteProgram(renderer.canvas_blit_program);
    }
    // if (renderer.canvas_fbo)
    // {
    //     glDeleteFramebuffers(1, &renderer.canvas_fbo);
    // }
    if (renderer.canvas)
    {
        glDeleteTextures(1, &renderer.canvas);
    }
    if (renderer.texture)
    {
        glDeleteTextures(1, &renderer.texture);
    }
    if (renderer.colormap)
    {
        glDeleteTextures(1, &renderer.colormap);
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
    // Update canvas
    // glBindTexture(GL_TEXTURE_2D, renderer.canvas);
    // glTexSubImage2D(GL_TEXTURE_2D,
    //                 0,
    //                 0,
    //                 0,
    //                 NUX_CANVAS_WIDTH,
    //                 NUX_CANVAS_HEIGHT,
    //                 GL_RED_INTEGER,
    //                 GL_UNSIGNED_BYTE,
    //                 nux_instance_get_canvas(inst));
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Update texture
    // glBindTexture(GL_TEXTURE_2D, renderer.texture);
    // glTexSubImage2D(GL_TEXTURE_2D,
    //                 0,
    //                 0,
    //                 0,
    //                 NUX_TEXTURE_WIDTH,
    //                 NUX_TEXTURE_HEIGHT,
    //                 GL_RED_INTEGER,
    //                 GL_UNSIGNED_BYTE,
    //                 nux_instance_get_texture(inst));
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Update colormap
    // glBindTexture(GL_TEXTURE_2D, renderer.colormap);
    // glTexSubImage2D(GL_TEXTURE_2D,
    //                 0,
    //                 0,
    //                 0,
    //                 NUX_COLORMAP_SIZE,
    //                 1,
    //                 GL_RGB,
    //                 GL_UNSIGNED_BYTE,
    //                 nux_instance_get_colormap(inst));
    // glBindTexture(GL_TEXTURE_2D, 0);

    // Update buffer

    // Execute commands

    // Blit surface to screen
    nu_v2i_t pos  = viewport.min;
    nu_v2u_t size = nu_b2i_size(viewport);
    pos.y
        = window_size.y - (pos.y + size.y); // Patch pos (bottom left in opengl)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glUseProgram(renderer.canvas_blit_program);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(pos.x, pos.y, size.x, size.y);

    // resolution = size;

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, renderer.canvas);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, renderer.colormap);
    // glBindVertexArray(renderer.empty_vao);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0);
    // glDisable(GL_FRAMEBUFFER_SRGB);
    // glUseProgram(0);
}
void
renderer_render_end (nux_instance_t *inst,
                     nu_b2i_t        viewport,
                     nu_v2u_t        window_size)
{
    glDisable(GL_FRAMEBUFFER_SRGB);
    glUseProgram(0);
}
