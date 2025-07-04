#include "internal.h"

#include <shaders_data.c.inc>

static struct
{
    GLuint empty_vao;
} renderer;

static nux_status_t
compile_shader (const char *source,
                size_t      source_len,
                GLuint      shader_type,
                GLuint     *shader)
{
    GLint success;
    *shader                 = glCreateShader(shader_type);
    const GLchar *psource[] = { (const GLchar *)source };
    const GLint   psize[]   = { source_len };
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
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static nux_status_t
compile_program (const char *vert,
                 size_t      vert_len,
                 const char *frag,
                 size_t      frag_len,
                 GLuint     *program)
{
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    nux_status_t status = NUX_SUCCESS;

    status = compile_shader(vert, vert_len, GL_VERTEX_SHADER, &vertex_shader);
    CHECK(status, goto cleanup0);

    status
        = compile_shader(frag, frag_len, GL_FRAGMENT_SHADER, &fragment_shader);
    CHECK(status, goto cleanup1);

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

        return NUX_FAILURE;
    }

    glDeleteShader(fragment_shader);
cleanup1:
    glDeleteShader(vertex_shader);
cleanup0:
    return status;
}

nux_status_t
nux_os_create_pipeline (void               *userdata,
                        nux_u32_t           slot,
                        nux_gpu_pass_type_t type)
{
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->pipelines), return NUX_FAILURE);
    pipeline_t *pipeline = inst->pipelines + slot;

    nux_status_t status = NUX_SUCCESS;
    switch (type)
    {
        case NUX_GPU_PASS_MAIN: {
            status       = compile_program(shader_main_vert,
                                     ARRAY_LEN(shader_main_vert),
                                     shader_main_frag,
                                     ARRAY_LEN(shader_main_frag),
                                     &pipeline->handle);
            GLuint index = glGetProgramResourceIndex(
                pipeline->handle, GL_UNIFORM_BLOCK, "Constants_std140");
            glUniformBlockBinding(pipeline->handle, index, 1);
            index = glGetProgramResourceIndex(
                pipeline->handle, GL_SHADER_STORAGE_BLOCK, "StructuredBuffer");
            glShaderStorageBlockBinding(pipeline->handle, index, 2);
            index = glGetProgramResourceIndex(
                pipeline->handle, GL_SHADER_STORAGE_BLOCK, "transforms");
            glShaderStorageBlockBinding(pipeline->handle, index, 3);
        }
        break;
        case NUX_GPU_PASS_CANVAS: {
            status       = compile_program(shader_canvas_vert,
                                     ARRAY_LEN(shader_canvas_vert),
                                     shader_canvas_frag,
                                     ARRAY_LEN(shader_canvas_frag),
                                     &pipeline->handle);
            GLuint index = glGetProgramResourceIndex(
                pipeline->handle, GL_UNIFORM_BLOCK, "Constants_std140");
            glUniformBlockBinding(pipeline->handle, index, 1);
        }
        break;
    }
    CHECK(status, return NUX_FAILURE);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_framebuffer (void *userdata, nux_u32_t slot, nux_u32_t texture)
{
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->framebuffers), return NUX_FAILURE);
    CHECK(slot < ARRAY_LEN(inst->textures), return NUX_FAILURE);
    framebuffer_t *fb  = inst->framebuffers + slot;
    texture_t     *tex = inst->textures + texture;

    if (!fb->handle)
    {
        glGenFramebuffers(1, &fb->handle);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->handle, 0);
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_DEPTH24_STENCIL8,
    //              800,
    //              600,
    //              0,
    //              GL_DEPTH_STENCIL,
    //              GL_UNSIGNED_INT_24_8,
    //              NULL);
    // glFramebufferTexture2D(
    //     GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture,
    //     0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return NUX_FAILURE;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // depth buffer
    // GLuint rbo;
    // glGenRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    // glFramebufferRenderbuffer(
    //     GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_texture (void                         *userdata,
                       nux_u32_t                     slot,
                       const nux_gpu_texture_info_t *info)
{
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->textures), return NUX_FAILURE);
    texture_t *tex = inst->textures + slot;

    switch (info->type)
    {
        case NUX_TEXTURE_IMAGE_RGBA:
            tex->internal_format = GL_RGBA8;
            tex->format          = GL_RGBA;
            break;
        case NUX_TEXTURE_IMAGE_INDEX:
            tex->internal_format = GL_R8UI;
            tex->format          = GL_RED_INTEGER;
            break;
        case NUX_TEXTURE_RENDER_TARGET:
            tex->internal_format = GL_RGBA8;
            tex->format          = GL_RGB;
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

    // Create texture object
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
                 NULL);
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
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->textures), return NUX_FAILURE);
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
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->buffers), return NUX_FAILURE);
    buffer_t *buffer = inst->buffers + slot;

    switch (type)
    {
        case NUX_GPU_BUFFER_UNIFORM:
            buffer->buffer_type = GL_UNIFORM_BUFFER;
            break;
        case NUX_GPU_BUFFER_STORAGE:
            buffer->buffer_type = GL_SHADER_STORAGE_BUFFER;
            break;
    }

    if (!buffer->handle)
    {
        glGenBuffers(1, &buffer->handle);
    }
    glBindBuffer(buffer->buffer_type, buffer->handle);
    glBufferData(buffer->buffer_type, size, NULL, GL_DYNAMIC_DRAW);
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
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->buffers), return NUX_FAILURE);
    buffer_t *buffer = inst->buffers + slot;

    glBindBuffer(buffer->buffer_type, buffer->handle);
    glBufferSubData(buffer->buffer_type, offset, size, data);
    glBindBuffer(buffer->buffer_type, 0);

    return NUX_SUCCESS;
}
void
nux_os_gpu_submit_pass (void                    *userdata,
                        const nux_gpu_pass_t    *pass,
                        const nux_gpu_command_t *cmds)
{
    instance_t *inst = userdata;

    glEnable(GL_FRAMEBUFFER_SRGB);

    if (pass->framebuffer)
    {
        framebuffer_t *fb = inst->framebuffers + pass->framebuffer;
        glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(inst->viewport.x,
                   inst->viewport.y,
                   inst->viewport.w,
                   inst->viewport.h);
    }

    switch (pass->type)
    {
        case NUX_GPU_PASS_MAIN: {
            GLuint program = inst->pipelines[pass->pipeline].handle;
            glUseProgram(program);

            buffer_t *buffer = inst->buffers + pass->main.constants_buffer;
            assert(buffer->buffer_type == GL_UNIFORM_BUFFER);
            glBindBufferBase(buffer->buffer_type, 1, buffer->handle);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_MULTISAMPLE);

            for (nux_u32_t i = 0; i < pass->count; ++i)
            {
                const nux_gpu_command_t *cmd = cmds + i;

                buffer = inst->buffers + cmd->main.vertices;
                assert(buffer->buffer_type == GL_SHADER_STORAGE_BUFFER);
                glBindBufferBase(buffer->buffer_type, 2, buffer->handle);

                buffer = inst->buffers + cmd->main.transforms;
                assert(buffer->buffer_type == GL_SHADER_STORAGE_BUFFER);
                glBindBufferBase(buffer->buffer_type, 3, buffer->handle);

                GLuint location = glGetUniformLocation(
                    program, "entryPointParams.vertexFirst");
                glUniform1ui(location, cmd->main.vertex_first);
                location = glGetUniformLocation(
                    program, "entryPointParams.transformIndex");
                glUniform1ui(location, cmd->main.transform_index);

                if (cmd->main.texture)
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D,
                                  inst->textures[cmd->main.texture].handle);
                    location = glGetUniformLocation(
                        program, "SPIRV_Cross_Combinedcanvassampler0");
                    glUniform1i(location, 0);

                    location = glGetUniformLocation(
                        program, "entryPointParams.hasTexture");
                    glUniform1ui(location, 1);
                }
                else
                {
                    location = glGetUniformLocation(
                        program, "entryPointParams.hasTexture");
                    glUniform1ui(location, 0);
                }

                glBindVertexArray(renderer.empty_vao);
                glDrawArrays(GL_TRIANGLES, 0, cmd->main.vertex_count);
                glBindVertexArray(0);
            }

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_MULTISAMPLE);
        }
        break;
        case NUX_GPU_PASS_CANVAS: {
            GLuint program = inst->pipelines[pass->pipeline].handle;
            glUseProgram(program);

            buffer_t *buffer = inst->buffers + pass->main.constants_buffer;
            assert(buffer->buffer_type == GL_UNIFORM_BUFFER);
            glBindBufferBase(buffer->buffer_type, 1, buffer->handle);

            for (nux_u32_t i = 0; i < pass->count; ++i)
            {
                const nux_gpu_command_t *cmd = cmds + i;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,
                              inst->textures[cmd->canvas.texture].handle);

                GLuint location = glGetUniformLocation(
                    program, "SPIRV_Cross_Combinedcanvassampler0");
                glUniform1i(location, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,
                              inst->textures[cmd->canvas.colormap].handle);
                location = glGetUniformLocation(
                    program, "SPIRV_Cross_Combinedcolormapsampler1"),
                glUniform1i(location, 1);

                glBindVertexArray(renderer.empty_vao);
                glDrawArrays(GL_TRIANGLES, 0, cmd->canvas.vertex_count);
                glBindVertexArray(0);
            }
        }
        break;
    }

    glDisable(GL_FRAMEBUFFER_SRGB);
}

nux_status_t
renderer_init (void)
{
    nux_status_t status = NUX_SUCCESS;

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
static float
color_to_linear (float x)
{
    if (x > 0.04045)
    {
        return powf((x + 0.055) / 1.055, 2.4);
    }
    else
    {
        return x / 12.92;
    }
}
void
renderer_clear (struct nk_rect viewport, struct nk_vec2i window_size)
{
    float clear[] = { 25. / 255, 27. / 255, 43. / 255, 1 };
    for (int i = 0; i < (int)ARRAY_LEN(clear); ++i)
    {
        clear[i] = color_to_linear(clear[i]);
    }
    struct nk_vec2i pos  = { viewport.x, viewport.y };
    struct nk_vec2i size = { viewport.w, viewport.h };
    // Patch pos (bottom left in opengl)
    pos.y = window_size.y - (pos.y + size.y);
    glViewport(0, 0, window_size.x, window_size.y);
    glEnable(GL_SCISSOR_TEST);
    glScissor(pos.x, pos.y, size.x, size.y);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(clear[0], clear[1], clear[2], clear[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_SCISSOR_TEST);
}
