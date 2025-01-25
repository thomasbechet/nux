#include "renderer.h"

#include "shaders_data.h"
#include "logger.h"
#include "window.h"
#include "core/platform.h"

#include <glad/gl.h>
#define RGFW_IMPORT
#include <rgfw/RGFW.h>

#define VERTEX_POSITION_OFFSET 0
#define VERTEX_UV_OFFSET       3
#define VERTEX_COLOR_OFFSET    5
#define VERTEX_SIZE            8

typedef struct
{
    nu_u32_t offset;
} mesh_t;

static struct
{
    GLuint   textures[GPU_MAX_TEXTURE];
    mesh_t   meshes[GPU_MAX_MESH];
    nu_u32_t vbo_offset;
    GLuint   vbo;
    GLuint   vao;
    GLuint   unlit_program;
    GLuint   screen_blit_program;
    GLuint   surface_fbo;
    GLuint   surface_texture;
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
            logger_log(NU_LOG_ERROR,
                       "GL: %s, message = %s",
                       message_type_string(type),
                       message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            logger_log(NU_LOG_ERROR,
                       "GL: %s, message = %s",
                       message_type_string(type),
                       message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            logger_log(NU_LOG_INFO,
                       "GL: %s, message = %s",
                       message_type_string(type),
                       message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            logger_log(NU_LOG_INFO,
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
    const GLchar *psource[] = { (const GLchar *)source.data };
    const GLint   psize[]   = { source.size };
    glShaderSource(*shader, 1, psource, psize);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &max_length);
        GLchar *log = (GLchar *)malloc(sizeof(GLchar) * max_length);
        glGetShaderInfoLog(*shader, max_length, &max_length, log);
        logger_log(NU_LOG_ERROR, "Failed to compile shader: %s", log);
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
        logger_log(NU_LOG_ERROR, "Failed to link shader: %s", log);
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
        logger_log(NU_LOG_ERROR, "Failed to load GL functions");
        return NU_FAILURE;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, NU_NULL);

    // Compile shaders
    glEnableVertexAttribArray(0);
    status = compile_program(
        shader_unlit_vert, shader_unlit_frag, &renderer.unlit_program);
    NU_CHECK(status, goto cleanup0);

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
                 VM_SCREEN_WIDTH,
                 VM_SCREEN_HEIGHT,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &renderer.surface_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.surface_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           renderer.surface_texture,
                           0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER,
    //                        GL_DEPTH_STENCIL_ATTACHMENT,
    //                        GL_TEXTURE_2D,
    //                        target->depth,
    //                        0);
    NU_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)
              == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return status;

cleanup0:
    renderer_free();
    return status;
}
void
renderer_free (void)
{
    if (renderer.unlit_program)
    {
        glDeleteProgram(renderer.unlit_program);
    }
    if (renderer.screen_blit_program)
    {
        glDeleteProgram(renderer.screen_blit_program);
    }
    for (nu_size_t i = 0; i < GPU_MAX_TEXTURE; ++i)
    {
        if (renderer.textures[i])
        {
            glDeleteTextures(1, renderer.textures + i);
        }
    }
    if (renderer.vao)
    {
        glDeleteVertexArrays(1, &renderer.vao);
        glDeleteVertexArrays(1, &renderer.vbo);
    }
}

void
os_gpu_init (vm_t *vm)
{
    // Initialize memory
    nu_memset(
        &renderer.textures, 0, sizeof(*renderer.textures) * GPU_MAX_TEXTURE);

    // Initialize vertices
    renderer.vbo_offset = 0;
    glGenVertexArrays(1, &renderer.vao);
    glBindVertexArray(renderer.vao);

    // VBO
    const nu_size_t vertex_size = VERTEX_SIZE * sizeof(nu_f32_t);
    glGenBuffers(1, &renderer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vm->gpu.config.max_vertex_count * vertex_size,
                 NU_NULL,
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          vertex_size,
                          (void *)(VERTEX_UV_OFFSET * sizeof(nu_f32_t)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          vertex_size,
                          (void *)(VERTEX_COLOR_OFFSET * sizeof(nu_f32_t)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void
os_gpu_init_texture (vm_t *vm, nu_u32_t index, const void *p)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 64 << vm->gpu.textures[index].size,
                 64 << vm->gpu.textures[index].size,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 p);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderer.textures[index] = handle;
}
void
os_gpu_free_texture (vm_t *vm, nu_u32_t index)
{
    glDeleteTextures(1, &renderer.textures[index]);
}
void
os_gpu_write_texture (vm_t       *vm,
                      nu_u32_t    index,
                      nu_u32_t    x,
                      nu_u32_t    y,
                      nu_u32_t    w,
                      nu_u32_t    h,
                      const void *p)
{
    GLuint handle = renderer.textures[index];
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, p);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
os_gpu_init_mesh (vm_t *vm, nu_u32_t index, const void *p)
{
    renderer.meshes[index].offset = renderer.vbo_offset;
    renderer.vbo_offset += vm->gpu.meshes[index].count;
    if (p)
    {
        os_gpu_write_mesh(vm,
                          index,
                          vm->gpu.meshes[index].attributes,
                          0,
                          vm->gpu.meshes[index].count,
                          p);
    }
}
void
os_gpu_free_mesh (vm_t *vm, nu_u32_t index)
{
}
void
os_gpu_write_mesh (vm_t                  *vm,
                   nu_u32_t               index,
                   gpu_vertex_attribute_t write_attributes,
                   nu_u32_t               first,
                   nu_u32_t               count,
                   const void            *p)
{
    const nu_f32_t              *data = p;
    nu_f32_t                    *ptr  = NU_NULL;
    const gpu_vertex_attribute_t mesh_attributes
        = vm->gpu.meshes[index].attributes;

    // Compute vertex index in vbo
    first = renderer.meshes[index].offset + first;

    // Compute user vertex stride
    size_t vertex_stride = 0;
    vertex_stride += write_attributes & GPU_VERTEX_POSTIION ? NU_V3_SIZE : 0;
    vertex_stride += write_attributes & GPU_VERTEX_UV ? NU_V2_SIZE : 0;
    vertex_stride += write_attributes & GPU_VERTEX_COLOR ? NU_V3_SIZE : 0;

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    nu_size_t write_attribute_offset = 0;
    if (write_attributes & GPU_VERTEX_POSTIION)
    {
        if (mesh_attributes & GPU_VERTEX_POSTIION)
        {
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu_size_t offset = vertex_stride * i;
                nu_size_t vbo_offset
                    = (first + i) * VERTEX_SIZE + VERTEX_POSITION_OFFSET;
                ptr[vbo_offset + 0] = data[offset + write_attribute_offset + 0];
                ptr[vbo_offset + 1] = data[offset + write_attribute_offset + 1];
                ptr[vbo_offset + 2] = data[offset + write_attribute_offset + 2];
            }
        }
        write_attribute_offset += NU_V3_SIZE;
    }
    if (write_attributes & GPU_VERTEX_UV)
    {
        if (mesh_attributes & GPU_VERTEX_UV)
        {
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu_size_t offset = vertex_stride * i;
                nu_size_t vbo_offset
                    = (first + i) * VERTEX_SIZE + VERTEX_UV_OFFSET;
                ptr[vbo_offset + 0] = data[offset + write_attribute_offset + 0];
                ptr[vbo_offset + 1] = data[offset + write_attribute_offset + 1];
            }
        }
        write_attribute_offset += NU_V2_SIZE;
    }
    if (write_attributes & GPU_VERTEX_COLOR)
    {
        if (mesh_attributes & GPU_VERTEX_COLOR)
        {
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu_size_t offset = vertex_stride * i;
                nu_size_t vbo_offset
                    = (first + i) * VERTEX_SIZE + VERTEX_COLOR_OFFSET;
                ptr[vbo_offset + 0] = data[offset + write_attribute_offset + 0];
                ptr[vbo_offset + 1] = data[offset + write_attribute_offset + 1];
                ptr[vbo_offset + 2] = data[offset + write_attribute_offset + 2];
            }
        }
        write_attribute_offset += NU_V3_SIZE;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void
os_gpu_begin (vm_t *vm)
{
    glUseProgram(renderer.unlit_program);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    // glDepthMask(GL_TRUE);
    // glDepthFunc(GL_LESS);
    // glFrontFace(GL_CCW);
    // glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render on surface framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.surface_fbo);
    glViewport(0, 0, VM_SCREEN_WIDTH, VM_SCREEN_HEIGHT);

    // Clear color
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    nu_v2u_t size = nu_v2u(VM_SCREEN_WIDTH, VM_SCREEN_HEIGHT);
    glUniform2uiv(glGetUniformLocation(renderer.unlit_program, "viewport_size"),
                  1,
                  size.data);
}
void
os_gpu_end (vm_t *vm)
{
    // Blit surface
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    nu_v4_t clear
        = nu_color_to_vec4(nu_color_to_linear(nu_color(25, 27, 43, 255)));
    glUseProgram(renderer.screen_blit_program);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    nu_b2i_t viewport = window_get_render_viewport();
    nu_v2u_t size     = nu_b2i_size(viewport);
    glViewport(viewport.min.x, viewport.min.y, size.x, size.y);
    glClearColor(clear.x, clear.y, clear.z, clear.w);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);
}
void
os_gpu_set_transform (vm_t *vm, gpu_transform_t transform)
{
    switch (transform)
    {
        case GPU_TRANSFORM_MODEL: {
            glUniformMatrix4fv(
                glGetUniformLocation(renderer.unlit_program, "model"),
                1,
                GL_FALSE,
                vm->gpu.state.model.data);
        }
        break;
        case GPU_TRANSFORM_PROJECTION:
        case GPU_TRANSFORM_VIEW: {
            nu_m4_t view_projection
                = nu_m4_mul(vm->gpu.state.projection, vm->gpu.state.view);
            glUniformMatrix4fv(
                glGetUniformLocation(renderer.unlit_program, "view_projection"),
                1,
                GL_FALSE,
                view_projection.data);
        }
        break;
    }
}
void
os_gpu_set_texture (vm_t *vm, nu_u32_t index)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.textures[index]);
}
void
os_gpu_draw_submesh (vm_t *vm, nu_u32_t mesh, nu_u32_t first, nu_u32_t count)
{
    nu_u32_t offset = renderer.meshes[mesh].offset;
    glBindVertexArray(renderer.vao);
    glDrawArrays(GL_TRIANGLES, offset + first, count);
    glBindVertexArray(0);
}
void
os_gpu_draw_nodes (vm_t *vm, nu_u32_t first, nu_u32_t count)
{
    nu_m4_t previous_transform = vm->gpu.state.model;
    for (nu_u32_t i = first; i < first + count; ++i)
    {
        const gpu_node_t *node = vm->gpu.nodes + i;
        vm->gpu.state.model    = nu_m4_mul(previous_transform, node->transform);
        os_gpu_set_transform(vm, GPU_TRANSFORM_MODEL);
        os_gpu_set_texture(vm, node->texture);
        os_gpu_draw_submesh(
            vm, node->mesh, 0, vm->gpu.meshes[node->mesh].count);
    }
    vm->gpu.state.model = previous_transform;
    os_gpu_set_transform(vm, GPU_TRANSFORM_MODEL);
}
