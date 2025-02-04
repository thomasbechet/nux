#include "renderer.h"

#include "shaders_data.h"
#include "fonts_data.h"
#include "logger.h"
#include "window.h"
#include "core/platform.h"
#include "core/gpu.h"
#include "core/vm.h"

#include <glad/gl.h>
#define RGFW_IMPORT
#include <rgfw/RGFW.h>

#define VERTEX_POSITION_OFFSET 0
#define VERTEX_UV_OFFSET       3
#define VERTEX_COLOR_OFFSET    5
#define VERTEX_SIZE            8
#define VERTEX_INIT_SIZE       NU_MEM_1M
#define NODE_INIT_SIZE         2048
#define MAX_BLIT_COUNT         4096

typedef struct
{
    nu_u32_t offset;
} mesh_t;

typedef struct
{
    nu_u32_t first_node;
} model_t;

typedef struct
{
    GLuint     texture;
    nu_b2i_t  *glyphs;
    nu_size_t  glyphs_count;
    nu_v2u_t   glyph_size;
    nu_wchar_t min_char;
    nu_wchar_t max_char;
} font_t;

typedef struct
{
    nu_u32_t pos;
    nu_u32_t tex;
    nu_u32_t size;
} blit_t;

static struct
{
    GLuint           textures[GPU_MAX_TEXTURE];
    GLuint           white_texture;
    mesh_t           meshes[GPU_MAX_MESH];
    gpu_model_node_t nodes[NODE_INIT_SIZE];
    nu_u32_t         nodes_next;
    model_t          models[GPU_MAX_MODEL];
    font_t           font;
    nu_size_t        blit_count;
    GLuint           blit_vbo;
    GLuint           blit_vao;
    nu_u32_t         vbo_offset;
    GLuint           vbo;
    GLuint           vao;
    GLuint           unlit_program;
    GLuint           screen_blit_program;
    GLuint           canvas_blit_program;
    GLuint           surface_fbo;
    GLuint           surface_texture;
    GLuint           surface_depth;
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
static nu_status_t
init_default_font (void)
{
    font_t *font = &renderer.font;

    // Find min/max characters
    font->min_char             = 127;
    font->max_char             = -128;
    const nu_size_t char_count = sizeof(default_font_data_chars);
    for (nu_size_t i = 0; i < char_count; ++i)
    {
        font->min_char = NU_MIN(font->min_char, default_font_data_chars[i]);
        font->max_char = NU_MAX(font->max_char, default_font_data_chars[i]);
    }

    const nu_size_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;

    font->glyphs_count = font->max_char - font->min_char + 1;
    font->glyph_size
        = nu_v2u(DEFAULT_FONT_DATA_WIDTH, DEFAULT_FONT_DATA_HEIGHT);
    font->glyphs = (nu_b2i_t *)malloc(sizeof(nu_b2i_t) * font->glyphs_count);
    NU_CHECK(font->glyphs, return NU_NULL);

    NU_ASSERT(((sizeof(default_font_data) * 8) / pixel_per_glyph)
              == char_count);

    // Load default font data into image
    nu_v2u_t   texture_size = nu_v2u(DEFAULT_FONT_DATA_WIDTH * char_count,
                                   DEFAULT_FONT_DATA_HEIGHT);
    nu_byte_t *texture_data
        = malloc(sizeof(nu_byte_t) * texture_size.x * texture_size.y * 4);

    nu_b2i_t extent
        = nu_b2i_xywh(0, 0, DEFAULT_FONT_DATA_WIDTH, DEFAULT_FONT_DATA_HEIGHT);
    for (nu_size_t ci = 0; ci < char_count; ++ci)
    {
        for (nu_size_t p = 0; p < pixel_per_glyph; ++p)
        {
            nu_size_t bit_offset = ci * pixel_per_glyph + p;
            NU_ASSERT((bit_offset / 8) < sizeof(default_font_data));
            nu_byte_t byte    = default_font_data[bit_offset / 8];
            nu_byte_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nu_size_t px = extent.min.x + p % DEFAULT_FONT_DATA_WIDTH;
            nu_size_t py = extent.min.y + p / DEFAULT_FONT_DATA_WIDTH;
            nu_size_t pi = py * texture_size.x + px;

            nu_byte_t color = bit_set ? 0xFF : 0x00;
            NU_ASSERT(pi < (texture_size.x * texture_size.y));
            texture_data[pi * 4 + 0] = color;
            texture_data[pi * 4 + 1] = color;
            texture_data[pi * 4 + 2] = color;
            texture_data[pi * 4 + 3] = color;
        }
        nu_size_t gi = default_font_data_chars[ci] - font->min_char;
        NU_ASSERT(gi < font->glyphs_count);
        font->glyphs[gi] = extent;
        extent = nu_b2i_translate(extent, nu_v2i(DEFAULT_FONT_DATA_WIDTH, 0));
    }

    // Generate texture
    glGenTextures(1, &font->texture);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 texture_size.x,
                 texture_size.y,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(texture_data);

    return NU_SUCCESS;
}
static void
init_canvas (void)
{
    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Create VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Configure VAO
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribIPointer(
        0, 1, GL_UNSIGNED_INT, sizeof(blit_t), (void *)(offsetof(blit_t, pos)));
    glVertexAttribIPointer(
        1, 1, GL_UNSIGNED_INT, sizeof(blit_t), (void *)(offsetof(blit_t, tex)));
    glVertexAttribIPointer(2,
                           1,
                           GL_UNSIGNED_INT,
                           sizeof(blit_t),
                           (void *)(offsetof(blit_t, size)));

    nu_size_t buffer_size = sizeof(blit_t) * MAX_BLIT_COUNT;
    glBufferData(GL_ARRAY_BUFFER, buffer_size, NU_NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    renderer.blit_vbo   = vbo;
    renderer.blit_vao   = vao;
    renderer.blit_count = 0;
}
static void
free_canvas (void)
{
    // TODO:
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

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    status = compile_program(shader_canvas_blit_vert,
                             shader_canvas_blit_frag,
                             &renderer.canvas_blit_program);
    NU_CHECK(status, goto cleanup0);
    glUseProgram(renderer.canvas_blit_program);
    glUniform1i(glGetUniformLocation(renderer.canvas_blit_program, "texture0"),
                0);

    // Create render target
    glGenTextures(1, &renderer.surface_texture);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB,
                 GPU_SCREEN_WIDTH,
                 GPU_SCREEN_HEIGHT,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &renderer.surface_depth);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_depth);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH24_STENCIL8,
                 GPU_SCREEN_WIDTH,
                 GPU_SCREEN_HEIGHT,
                 0,
                 GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8,
                 NU_NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &renderer.surface_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.surface_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           renderer.surface_texture,
                           0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D,
                           renderer.surface_depth,
                           0);
    NU_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)
              == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create white texture
    glGenTextures(1, &renderer.white_texture);
    glBindTexture(GL_TEXTURE_2D, renderer.white_texture);
    nu_color_t white = NU_COLOR_WHITE;
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 1,
                 1,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 &white.rgba);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create default font
    init_default_font();

    // Create canvas resources
    init_canvas();

    return status;

cleanup0:
    renderer_free();
    return status;
}
static void
free_default_font (void)
{
    font_t *font = &renderer.font;
    glDeleteTextures(1, &font->texture);
    free(font->glyphs);
}
void
renderer_free (void)
{
    if (renderer.canvas_blit_program)
    {
        glDeleteProgram(renderer.canvas_blit_program);
    }
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
    glDeleteTextures(1, &renderer.white_texture);
    glDeleteTextures(1, &renderer.surface_texture);
    glDeleteTextures(1, &renderer.surface_depth);
    glDeleteFramebuffers(1, &renderer.surface_fbo);
    if (renderer.font.texture)
    {
        free_default_font();
    }
    if (renderer.blit_vao)
    {
        free_canvas();
    }
}

void
os_gpu_init (vm_t *vm)
{
    // Initialize memory
    nu_memset(
        &renderer.textures, 0, sizeof(*renderer.textures) * GPU_MAX_TEXTURE);

    renderer.nodes_next = 0;

    // Initialize vertices
    renderer.vbo_offset = 0;
    glGenVertexArrays(1, &renderer.vao);
    glBindVertexArray(renderer.vao);

    // VBO
    const nu_size_t vertex_size = VERTEX_SIZE * sizeof(nu_f32_t);
    glGenBuffers(1, &renderer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 VERTEX_INIT_SIZE * vertex_size,
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
os_gpu_init_texture (vm_t *vm, nu_u32_t index)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 vm->gpu.textures[index].size,
                 vm->gpu.textures[index].size,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
os_gpu_update_texture (vm_t *vm, nu_u32_t index)
{
    gpu_texture_t *texture = vm->gpu.textures + index;
    GLuint         handle  = renderer.textures[index];
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    texture->size,
                    texture->size,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    vm->gpu.vram + texture->addr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
os_gpu_init_mesh (vm_t *vm, nu_u32_t index)
{
    renderer.meshes[index].offset = renderer.vbo_offset;
    renderer.vbo_offset += vm->gpu.meshes[index].count;
    // Initialize colors to white
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < vm->gpu.meshes[index].count; ++i)
    {
        nu_size_t vbo_offset = (renderer.meshes[index].offset + i) * VERTEX_SIZE
                               + VERTEX_COLOR_OFFSET;
        ptr[vbo_offset + 0] = 1;
        ptr[vbo_offset + 1] = 1;
        ptr[vbo_offset + 2] = 1;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
void
os_gpu_free_mesh (vm_t *vm, nu_u32_t index)
{
}
void
os_gpu_update_mesh (vm_t *vm, nu_u32_t index)
{
    const gpu_mesh_t *mesh = vm->gpu.meshes + index;

    // Compute vertex index in vbo
    nu_size_t first = renderer.meshes[index].offset;

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);

    if (mesh->attributes & GPU_VERTEX_POSITION)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(vm->gpu.vram + mesh->addr
                                 + gpu_vertex_offset(mesh->attributes,
                                                     GPU_VERTEX_POSITION,
                                                     mesh->count)
                                       * sizeof(nu_f32_t));
        for (nu_size_t i = 0; i < mesh->count; ++i)
        {
            nu_size_t vbo_offset
                = (first + i) * VERTEX_SIZE + VERTEX_POSITION_OFFSET;
            ptr[vbo_offset + 0] = data[i * 3 + 0];
            ptr[vbo_offset + 1] = data[i * 3 + 1];
            ptr[vbo_offset + 2] = data[i * 3 + 2];
        }
    }
    if (mesh->attributes & GPU_VERTEX_UV)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(vm->gpu.vram + mesh->addr
                                 + gpu_vertex_offset(mesh->attributes,
                                                     GPU_VERTEX_UV,
                                                     mesh->count)
                                       * sizeof(nu_f32_t));
        for (nu_size_t i = 0; i < mesh->count; ++i)
        {
            nu_size_t vbo_offset = (first + i) * VERTEX_SIZE + VERTEX_UV_OFFSET;
            ptr[vbo_offset + 0]  = data[i * 2 + 0];
            ptr[vbo_offset + 1]  = data[i * 2 + 1];
        }
    }
    if (mesh->attributes & GPU_VERTEX_COLOR)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(vm->gpu.vram + mesh->addr
                                 + gpu_vertex_offset(mesh->attributes,
                                                     GPU_VERTEX_COLOR,
                                                     mesh->count)
                                       * sizeof(nu_f32_t));
        for (nu_size_t i = 0; i < mesh->count; ++i)
        {
            nu_size_t vbo_offset
                = (first + i) * VERTEX_SIZE + VERTEX_COLOR_OFFSET;
            ptr[vbo_offset + 0] = data[i * 3 + 0];
            ptr[vbo_offset + 1] = data[i * 3 + 1];
            ptr[vbo_offset + 2] = data[i * 3 + 2];
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void
os_gpu_init_model (vm_t *vm, nu_u32_t index)
{
    gpu_model_t *model = vm->gpu.models + index;
    NU_ASSERT(model->node_count + renderer.nodes_next < NODE_INIT_SIZE);
    renderer.models[index].first_node = renderer.nodes_next;
    for (nu_size_t i = 0; i < model->node_count; ++i)
    {
        gpu_model_node_t *node
            = renderer.nodes + i + renderer.models[index].first_node;
        node->texture         = -1;
        node->mesh            = -1;
        node->parent          = -1;
        node->local_to_parent = nu_m4_identity();
    }
    renderer.nodes_next += model->node_count;
}
void
os_gpu_free_model (vm_t *vm, nu_u32_t index)
{
}
void
os_gpu_update_model (vm_t                   *vm,
                     nu_u32_t                index,
                     nu_u32_t                node_index,
                     const gpu_model_node_t *node)
{
    renderer.nodes[renderer.models[index].first_node + node_index] = *node;
}
void
os_gpu_begin_frame (vm_t *vm)
{
    renderer.blit_count = 0;

    // Render on surface framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.surface_fbo);
    glViewport(0, 0, GPU_SCREEN_WIDTH, GPU_SCREEN_HEIGHT);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void
os_gpu_end_frame (vm_t *vm)
{
    // Clear window
    nu_v4_t clear
        = nu_color_to_vec4(nu_color_to_linear(nu_color(25, 27, 43, 255)));
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(clear.x, clear.y, clear.z, clear.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_FRAMEBUFFER_SRGB);
    // Blit surface
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(renderer.screen_blit_program);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    nu_b2i_t viewport = window_get_render_viewport();
    nu_v2u_t size     = nu_b2i_size(viewport);
    glViewport(viewport.min.x, viewport.min.y, size.x, size.y);
    glBindTexture(GL_TEXTURE_2D, renderer.surface_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glUseProgram(0);
}
void
os_gpu_push_transform (vm_t *vm, gpu_transform_t transform)
{
    glUseProgram(renderer.unlit_program);
    switch (transform)
    {
        case GPU_TRANSFORM_MODEL:
            break;
        case GPU_TRANSFORM_PROJECTION:
        case GPU_TRANSFORM_VIEW: {
        }
        break;
    }
    glUseProgram(0);
}
void
draw_model (vm_t *vm, nu_u32_t index, nu_m4_t transform)
{
    // Setup GL states
    glUseProgram(renderer.unlit_program);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Push constants
    nu_v2u_t size = nu_v2u(GPU_SCREEN_WIDTH, GPU_SCREEN_HEIGHT);
    glUniform2uiv(glGetUniformLocation(renderer.unlit_program, "viewport_size"),
                  1,
                  size.data);

    nu_m4_t view_projection
        = nu_m4_mul(vm->gpu.state.projection, vm->gpu.state.view);
    glUniformMatrix4fv(
        glGetUniformLocation(renderer.unlit_program, "view_projection"),
        1,
        GL_FALSE,
        view_projection.data);

    // Draw model
    const gpu_model_t *model = vm->gpu.models + index;
    for (nu_size_t i = 0; i < model->node_count; ++i)
    {
        gpu_model_node_t *node
            = renderer.nodes + renderer.models[index].first_node + i;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      (node->texture != -1) ? renderer.textures[node->texture]
                                            : renderer.white_texture);
        nu_m4_t  global_transform = node->local_to_parent;
        nu_u32_t parent           = node->parent;
        while (parent != (nu_u32_t)-1)
        {
            gpu_model_node_t *parent_node
                = renderer.nodes + renderer.models[index].first_node + parent;
            global_transform = nu_m4_mul(parent_node->local_to_parent,
                                         node->local_to_parent);
            parent           = parent_node->parent;
        }
        global_transform = nu_m4_mul(transform, global_transform);
        glUniformMatrix4fv(
            glGetUniformLocation(renderer.unlit_program, "model"),
            1,
            GL_FALSE,
            global_transform.data);
        nu_u32_t offset = renderer.meshes[node->mesh].offset;
        glBindVertexArray(renderer.vao);
        glDrawArrays(GL_TRIANGLES, offset, vm->gpu.meshes[node->mesh].count);
        glBindVertexArray(0);
    }

    // Reset state
    glUseProgram(0);
}
void
os_gpu_draw_model (vm_t *vm, nu_u32_t index)
{
    draw_model(vm, index, vm->gpu.state.model);
}
void
os_gpu_draw_text (
    vm_t *vm, nu_u32_t x, nu_u32_t y, const void *text, nu_u32_t len)
{
    // Transfer buffer
    glBindBuffer(GL_ARRAY_BUFFER, renderer.blit_vbo);
    blit_t *blits = (blit_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(blits);
    nu_size_t blit_start = renderer.blit_count;

    const font_t *font = &renderer.font;
    nu_sv_t       sv   = nu_sv_cstr(text);
    nu_b2i_t extent = nu_b2i_xywh(x, y, font->glyph_size.x, font->glyph_size.y);
    nu_size_t  it   = 0;
    nu_wchar_t c;
    while (nu_sv_next(sv, &it, &c))
    {
        if (c == '\n')
        {
            extent = nu_b2i_moveto(
                extent, nu_v2i(x, extent.min.y + font->glyph_size.y));
            continue;
        }
        if (c < font->min_char || c > font->max_char)
        {
            continue;
        }
        nu_size_t gi         = c - font->min_char;
        nu_b2i_t  tex_extent = font->glyphs[gi];

        // blit(extent, tex_extent, font->texture);

        nu_v2i_t pos = extent.min;
        nu_v2u_t tex = nu_v2u(tex_extent.min.x, tex_extent.min.y);
        nu_v2u_t size
            = nu_v2u_min(nu_b2i_size(extent), nu_b2i_size(tex_extent));

        NU_ASSERT(renderer.blit_count < MAX_BLIT_COUNT);
        blit_t *blit = blits + renderer.blit_count++;
        blit->pos    = ((nu_u32_t)pos.y << 16) | (nu_u32_t)pos.x;
        blit->tex    = (tex.y << 16) | tex.x;
        blit->size   = (size.y << 16) | size.x;

        extent = nu_b2i_translate(extent, nu_v2i(font->glyph_size.x, 0));
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Draw blits
    if (renderer.blit_count - blit_start)
    {
        glUseProgram(renderer.canvas_blit_program);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        nu_v2u_t size = nu_v2u(GPU_SCREEN_WIDTH, GPU_SCREEN_HEIGHT);
        glUniform2uiv(
            glGetUniformLocation(renderer.canvas_blit_program, "viewport_size"),
            1,
            size.data);
        glBindVertexArray(renderer.blit_vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font->texture);
        glDrawArraysInstancedBaseInstance(
            GL_TRIANGLES, 0, 6, renderer.blit_count - blit_start, blit_start);

        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(0);
    }
}
