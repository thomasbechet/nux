#include "internal.h"

#include <shaders_data.c.inc>

static struct
{
    GLuint      empty_vao;
    pipeline_t *active_pipeline;
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
nux_os_create_pipeline (void                          *userdata,
                        nux_u32_t                      slot,
                        const nux_gpu_pipeline_info_t *info)
{
    instance_t *inst = userdata;
    CHECK(slot < ARRAY_LEN(inst->pipelines), return NUX_FAILURE);
    pipeline_t *pipeline = inst->pipelines + slot;
    pipeline->type       = info->type;

    nux_status_t status = NUX_SUCCESS;
    switch (info->type)
    {
        case NUX_GPU_PIPELINE_UBER: {
            status = compile_program(shader_uber_vert,
                                     ARRAY_LEN(shader_uber_vert),
                                     shader_uber_frag,
                                     ARRAY_LEN(shader_uber_frag),
                                     &pipeline->program);

            GLuint index = glGetProgramResourceIndex(
                pipeline->program, GL_UNIFORM_BLOCK, "Constants_std140");
            glUniformBlockBinding(pipeline->program, index, 1);
            index = glGetProgramResourceIndex(
                pipeline->program, GL_SHADER_STORAGE_BLOCK, "StructuredBuffer");
            glShaderStorageBlockBinding(pipeline->program, index, 2);
            index = glGetProgramResourceIndex(
                pipeline->program, GL_SHADER_STORAGE_BLOCK, "vertices");
            glShaderStorageBlockBinding(pipeline->program, index, 3);
            index = glGetProgramResourceIndex(
                pipeline->program, GL_SHADER_STORAGE_BLOCK, "transforms");
            glShaderStorageBlockBinding(pipeline->program, index, 4);

            pipeline->indices[NUX_GPU_DESC_UBER_CONSTANTS]  = 1;
            pipeline->indices[NUX_GPU_DESC_UBER_BATCHES]    = 2;
            pipeline->indices[NUX_GPU_DESC_UBER_VERTICES]   = 3;
            pipeline->indices[NUX_GPU_DESC_UBER_TRANSFORMS] = 4;

            pipeline->locations[NUX_GPU_DESC_UBER_TEXTURE0]
                = glGetUniformLocation(pipeline->program,
                                       "SPIRV_Cross_Combinedcanvassampler0");
            pipeline->locations[NUX_GPU_DESC_UBER_BATCH_INDEX]
                = glGetUniformLocation(pipeline->program,
                                       "entryPointParams.batchIndex");

            pipeline->units[NUX_GPU_DESC_UBER_TEXTURE0] = 0;
        }
        break;
        case NUX_GPU_PIPELINE_CANVAS: {
            status = compile_program(shader_canvas_vert,
                                     ARRAY_LEN(shader_canvas_vert),
                                     shader_canvas_frag,
                                     ARRAY_LEN(shader_canvas_frag),
                                     &pipeline->program);

            GLuint index = glGetProgramResourceIndex(
                pipeline->program, GL_UNIFORM_BLOCK, "Constants_std140");
            glUniformBlockBinding(pipeline->program, index, 1);
            index = glGetProgramResourceIndex(
                pipeline->program, GL_SHADER_STORAGE_BLOCK, "StructuredBuffer");
            glShaderStorageBlockBinding(pipeline->program, index, 2);
            index = glGetProgramResourceIndex(
                pipeline->program, GL_SHADER_STORAGE_BLOCK, "quads");
            glShaderStorageBlockBinding(pipeline->program, index, 3);

            pipeline->indices[NUX_GPU_DESC_CANVAS_CONSTANTS] = 1;
            pipeline->indices[NUX_GPU_DESC_CANVAS_BATCHES]   = 2;
            pipeline->indices[NUX_GPU_DESC_CANVAS_QUADS]     = 3;

            pipeline->locations[NUX_GPU_DESC_CANVAS_TEXTURE]
                = glGetUniformLocation(pipeline->program,
                                       "SPIRV_Cross_Combinedtexture0sampler0");
            pipeline->locations[NUX_GPU_DESC_CANVAS_BATCH_INDEX]
                = glGetUniformLocation(pipeline->program,
                                       "entryPointParams.batchIndex");

            pipeline->units[NUX_GPU_DESC_CANVAS_TEXTURE] = 0;
        }
        break;
        case NUX_GPU_PIPELINE_BLIT: {
            status = compile_program(shader_blit_vert,
                                     ARRAY_LEN(shader_blit_vert),
                                     shader_blit_frag,
                                     ARRAY_LEN(shader_blit_frag),
                                     &pipeline->program);

            pipeline->locations[NUX_GPU_DESC_BLIT_TEXTURE]
                = glGetUniformLocation(pipeline->program,
                                       "SPIRV_Cross_Combinedtexture0sampler0");
            pipeline->locations[NUX_GPU_DESC_BLIT_TEXTURE_WIDTH]
                = glGetUniformLocation(pipeline->program,
                                       "entryPointParams.textureWidth");
            pipeline->locations[NUX_GPU_DESC_BLIT_TEXTURE_HEIGHT]
                = glGetUniformLocation(pipeline->program,
                                       "entryPointParams.textureHeight");

            pipeline->units[NUX_GPU_DESC_BLIT_TEXTURE] = 0;
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
    fb->width  = tex->info.width;
    fb->height = tex->info.height;
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
    tex->info      = *info;

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
nux_os_gpu_submit (void                    *userdata,
                   const nux_gpu_command_t *cmds,
                   nux_u32_t                count)
{
    instance_t *inst = userdata;

    // const nux_u8_t *it = cmds;
    // while (nux_gpu_command_decode(&it, &cmd))
    // {
    //
    // }

    // Execute commands
    for (nux_u32_t i = 0; i < count; ++i)
    {
        const nux_gpu_command_t *cmd = cmds + i;
        switch ((nux_gpu_command_type_t)cmd->type)
        {
            case NUX_GPU_COMMAND_BIND_FRAMEBUFFER: {
                if (cmd->bind_framebuffer.slot)
                {
                    framebuffer_t *fb
                        = inst->framebuffers + cmd->bind_framebuffer.slot;
                    glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
                    glViewport(0, 0, fb->width, fb->height);
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(0, 0, fb->width, fb->height);
                }
                else
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(inst->viewport.x,
                               inst->viewport.y,
                               inst->viewport.w,
                               inst->viewport.h);
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(inst->viewport.x,
                              inst->viewport.y,
                              inst->viewport.w,
                              inst->viewport.h);
                }
            }
            break;
            case NUX_GPU_COMMAND_BIND_PIPELINE: {
                renderer.active_pipeline
                    = inst->pipelines + cmd->bind_pipeline.slot;
                glUseProgram(renderer.active_pipeline->program);
                switch ((nux_gpu_pipeline_type_t)renderer.active_pipeline->type)
                {
                    case NUX_GPU_PIPELINE_UBER: {
                        glEnable(GL_DEPTH_TEST);
                        glEnable(GL_MULTISAMPLE);
                    }
                    break;
                    case NUX_GPU_PIPELINE_BLIT: {
                        glDisable(GL_DEPTH_TEST);
                        glDisable(GL_MULTISAMPLE);
                        glEnable(GL_BLEND);
                    }
                    break;
                    case NUX_GPU_PIPELINE_CANVAS: {
                        glDisable(GL_DEPTH_TEST);
                        glDisable(GL_MULTISAMPLE);
                    }
                    break;
                }
            }
            break;
            case NUX_GPU_COMMAND_BIND_BUFFER: {
                buffer_t *buffer = inst->buffers + cmd->bind_buffer.slot;
                GLuint    index
                    = renderer.active_pipeline->indices[cmd->bind_buffer.index];
                glBindBufferBase(buffer->buffer_type, index, buffer->handle);
            }
            break;
            case NUX_GPU_COMMAND_BIND_TEXTURE: {
                GLuint unit
                    = renderer.active_pipeline->units[cmd->bind_texture.index];
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D,
                              inst->textures[cmd->bind_texture.slot].handle);
                GLuint location = renderer.active_pipeline
                                      ->locations[cmd->bind_texture.index];
                glUniform1i(location, unit);
            }
            break;
            case NUX_GPU_COMMAND_PUSH_U32: {
                GLuint location
                    = renderer.active_pipeline->locations[cmd->push_u32.index];
                glUniform1ui(location, cmd->push_u32.value);
            }
            break;
            case NUX_GPU_COMMAND_PUSH_F32: {
                GLuint location
                    = renderer.active_pipeline->locations[cmd->push_f32.index];
                glUniform1f(location, cmd->push_f32.value);
            }
            break;
            case NUX_GPU_COMMAND_DRAW: {
                glBindVertexArray(renderer.empty_vao);
                glDrawArrays(GL_TRIANGLES, 0, cmd->draw.count);
                glBindVertexArray(0);
            }
            break;
            case NUX_GPU_COMMAND_CLEAR: {
                float r = ((cmd->clear.color & 0xFF0000) >> 16) / 255.;
                float g = ((cmd->clear.color & 0xFF00) >> 8) / 255.;
                float b = ((cmd->clear.color & 0xFF)) / 255.;
                glClearColor(r, g, b, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            break;
        }
    }
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
renderer_clear (void)
{
    float clear[] = { 25. / 255, 27. / 255, 43. / 255, 1 };
    for (int i = 0; i < (int)ARRAY_LEN(clear); ++i)
    {
        clear[i] = color_to_linear(clear[i]);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(clear[0], clear[1], clear[2], clear[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_FRAMEBUFFER_SRGB);
}
void
renderer_begin (struct nk_rect viewport, struct nk_vec2i window_size)
{
    struct nk_vec2i pos  = { viewport.x, viewport.y };
    struct nk_vec2i size = { viewport.w, viewport.h };
    // Patch pos (bottom left in opengl)
    pos.y = window_size.y - (pos.y + size.y);
    glViewport(0, 0, window_size.x, window_size.y);
    // glScissor(pos.x, pos.y, size.x, size.y);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_SCISSOR_TEST);
}
void
renderer_end (void)
{
    instance_t *inst = runtime_instance();

    // Restore state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        inst->viewport.x, inst->viewport.y, inst->viewport.w, inst->viewport.h);
    glDisable(GL_FRAMEBUFFER_SRGB);
}
