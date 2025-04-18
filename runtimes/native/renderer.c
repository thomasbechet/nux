#include "internal.h"

#include "shaders_data.c.inc"
#include "fonts_data.c.inc"

#include <glad/gl.h>
#define RGFW_IMPORT
#include <rgfw/RGFW.h>

#define VERTEX_POSITION_OFFSET 0
#define VERTEX_UV_OFFSET       3
#define VERTEX_COLOR_OFFSET    5
#define VERTEX_SIZE            8
#define VERTEX_INIT_SIZE       NU_MEM_1M
#define MAX_BLIT_COUNT         4096

typedef struct
{
    nu_u32_t  offset;
    nu_u32_t  update_counter;
    nu_bool_t initialized;
} mesh_t;

typedef struct
{
    GLuint   handle;
    nu_u32_t update_counter;
} texture_t;

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

typedef struct
{
    nu_m4_t   view;
    nu_m4_t   projection;
    nu_v4_t   color;
    nu_v4_t   fog_color;
    nu_v2u_t  viewport_size;
    nu_f32_t  fog_density;
    nu_f32_t  fog_near;
    nu_f32_t  fog_far;
    nu_bool_t is_volume;
} ubo_t;

typedef union
{
    texture_t texture;
    mesh_t    mesh;
} gl_object_t;

static struct
{
    GLuint      white_texture;
    gl_object_t objects[1024];
    font_t      font;
    nu_size_t   blit_count;
    GLuint      blit_vbo;
    GLuint      blit_vao;
    nu_u32_t    mesh_vbo_offset;
    GLuint      mesh_vbo;
    GLuint      mesh_vao;
    nu_u32_t    im_vbo_offset;
    GLuint      im_vbo;
    nu_f32_t   *im_vbo_data;
    GLuint      im_vao;
    nu_bool_t   ubo_dirty;
    GLuint      ubo_buffer;
    ubo_t       ubo;
    GLuint      unlit_program;
    GLuint      screen_blit_program;
    GLuint      canvas_blit_program;
    GLuint      surface_fbo;
    GLuint      surface_texture;
    GLuint      surface_depth;
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
static void
gen_vertex_vbo (GLuint *vbo, GLuint *vao, nu_u32_t capacity)
{
    const nu_size_t vertex_size = VERTEX_SIZE * sizeof(nu_f32_t);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
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

    // Initialize colors to white
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < capacity; ++i)
    {
        nu_size_t vbo_offset = i * VERTEX_SIZE + VERTEX_COLOR_OFFSET;
        ptr[vbo_offset + 0]  = 1;
        ptr[vbo_offset + 1]  = 1;
        ptr[vbo_offset + 2]  = 1;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
static void
init_texture (const nux_texture_t *texture, nux_id_t id)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 texture->size,
                 texture->size,
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
    // glTexParameteri(
    //     GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderer.objects[id].texture.handle         = handle;
    renderer.objects[id].texture.update_counter = 0;
}
static void
free_texture (nux_id_t id)
{
    glDeleteTextures(1, &renderer.objects[NUX_ID_INDEX(id)].texture.handle);
}
static void
update_texture (nux_instance_t inst, const nux_texture_t *texture, nux_id_t id)
{
    GLuint handle = renderer.objects[NUX_ID_INDEX(id)].texture.handle;
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    texture->size,
                    texture->size,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    nux_instance_get_object_unchecked(inst, texture->data));
    // glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderer.objects[NUX_ID_INDEX(id)].texture.update_counter
        = texture->update_counter;
}

static void
init_mesh (const nux_mesh_t *mesh, nux_id_t id)
{
    renderer.objects[id].mesh.offset = renderer.mesh_vbo_offset;
    renderer.mesh_vbo_offset += mesh->count;
    // Initialize colors to white
    glBindBuffer(GL_ARRAY_BUFFER, renderer.mesh_vbo);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < mesh->count; ++i)
    {
        nu_size_t vbo_offset
            = (renderer.objects[id].mesh.offset + i) * VERTEX_SIZE
              + VERTEX_COLOR_OFFSET;
        ptr[vbo_offset + 0] = 1;
        ptr[vbo_offset + 1] = 1;
        ptr[vbo_offset + 2] = 1;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
static void
update_mesh (nux_instance_t inst, const nux_mesh_t *mesh, nux_id_t id)
{
    // Compute vertex index in vbo
    nu_size_t first = renderer.objects[NUX_ID_INDEX(id)].mesh.offset;

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, renderer.mesh_vbo);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);

    if (mesh->attributes & NUX_VERTEX_POSITION)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(nux_instance_get_object_unchecked(inst,
                                                                   mesh->data)
                                 + nux_vertex_offset(mesh->attributes,
                                                     NUX_VERTEX_POSITION,
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
    if (mesh->attributes & NUX_VERTEX_UV)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(nux_instance_get_object_unchecked(inst,
                                                                   mesh->data)
                                 + nux_vertex_offset(mesh->attributes,
                                                     NUX_VERTEX_UV,
                                                     mesh->count)
                                       * sizeof(nu_f32_t));
        for (nu_size_t i = 0; i < mesh->count; ++i)
        {
            nu_size_t vbo_offset = (first + i) * VERTEX_SIZE + VERTEX_UV_OFFSET;
            ptr[vbo_offset + 0]  = data[i * 2 + 0];
            ptr[vbo_offset + 1]  = data[i * 2 + 1];
        }
    }
    if (mesh->attributes & NUX_VERTEX_COLOR)
    {
        const nu_f32_t *data
            = (const nu_f32_t *)(nux_instance_get_object_unchecked(inst,
                                                                   mesh->data)
                                 + nux_vertex_offset(mesh->attributes,
                                                     NUX_VERTEX_COLOR,
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
    renderer.objects[NUX_ID_INDEX(id)].mesh.update_counter
        = mesh->update_counter;
}

static void
init_ubo (void)
{
    renderer.ubo.view          = nu_m4_identity();
    renderer.ubo.projection    = nu_m4_identity();
    renderer.ubo.color         = nu_color_to_vec4(NU_COLOR_WHITE);
    renderer.ubo.fog_color     = nu_color_to_vec4(NU_COLOR_WHITE);
    renderer.ubo.viewport_size = nu_v2u(NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);
    renderer.ubo.fog_density   = 0;
    renderer.ubo.fog_near      = 0;
    renderer.ubo.fog_far       = 100;
    renderer.ubo.is_volume     = NU_FALSE;
}
static void
update_ubo (void)
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.ubo_buffer);
    glBufferData(
        GL_UNIFORM_BUFFER, sizeof(renderer.ubo), &renderer.ubo, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
static void
draw_node (nux_instance_t inst, nux_id_t node)
{
    nux_node_t *n = nux_instance_get_object_unchecked(inst, node);
    if (n->object
        && nux_instance_get_object_type(inst, n->object) == NUX_OBJECT_MODEL)
    {
        const nux_f32_t *local_to_world
            = nux_instance_get_object_unchecked(inst, n->local_to_world);
        nux_model_t *m = nux_instance_get_object_unchecked(inst, n->object);
        if (m->visible)
        {
            // Update texture
            texture_t *texture
                = m->texture
                      ? &renderer.objects[NUX_ID_INDEX(m->texture)].texture
                      : NU_NULL;
            if (texture)
            {
                nux_texture_t *tex
                    = nux_instance_get_object_unchecked(inst, m->texture);
                if (!texture->handle)
                {
                    init_texture(tex, m->texture);
                }
                if (texture->update_counter != tex->update_counter)
                {
                    update_texture(inst, tex, m->texture);
                }
            }

            // Update mesh
            mesh_t *renderer_mesh
                = &renderer.objects[NUX_ID_INDEX(m->mesh)].mesh;
            nux_mesh_t *mesh = nux_instance_get_object_unchecked(inst, m->mesh);
            if (!renderer_mesh->initialized)
            {
                init_mesh(mesh, m->mesh);
            }
            if (renderer_mesh->update_counter != mesh->update_counter)
            {
                update_mesh(inst, mesh, m->mesh);
            }

            // Bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,
                          texture ? texture->handle : renderer.white_texture);

            // Bind model matrix
            glUniformMatrix4fv(
                glGetUniformLocation(renderer.unlit_program, "model"),
                1,
                GL_FALSE,
                local_to_world);

            // Draw mesh
            glBindVertexArray(renderer.mesh_vao);
            glDrawArrays(GL_TRIANGLES, renderer_mesh->offset, mesh->count);
            glBindVertexArray(0);
        }
    }
    nux_id_t c = n->child;
    while (c)
    {
        draw_node(inst, c);
        c = ((nux_node_t *)nux_instance_get_object_unchecked(inst, c))->next;
    }
}
static void
draw_scene_instance (nux_instance_t inst, nux_id_t scene)
{
    // Acquire scene
    nux_scene_t *s = nux_instance_get_object_unchecked(inst, scene);
    draw_node(inst, s->root);
}
static void
draw_scene (nux_instance_t inst, nux_id_t camera)
{
    // Get scene camera
    nux_node_t   *cam_node = nux_instance_get_object_unchecked(inst, camera);
    nux_camera_t *cam
        = nux_instance_get_object_unchecked(inst, cam_node->object);

    // Update ubo with camera info
    const nux_f32_t *local_to_world
        = nux_instance_get_object_unchecked(inst, cam_node->local_to_world);
    nu_m4_t cam_transform = nu_m4(local_to_world);
    nu_v3_t eye           = nu_m4_mulv3(cam_transform, NU_V3_ZEROS);
    nu_v3_t center        = nu_m4_mulv3(cam_transform, NU_V3_FORWARD);
    nu_v3_t up = nu_v3_v4(nu_m4_mulv(cam_transform, nu_v4_v3(NU_V3_UP, 0)));
    renderer.ubo.view = nu_lookat(eye, center, up);
    renderer.ubo.projection
        = nu_perspective(cam->fov,
                         (nu_f32_t)NUX_SCREEN_WIDTH / NUX_SCREEN_HEIGHT,
                         cam->near,
                         cam->far);
    renderer.ubo.is_volume = NU_FALSE;
    update_ubo();

    // Setup GL states
    glUseProgram(renderer.unlit_program);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw scene
    draw_scene_instance(inst, cam_node->scene);

    // Reset state
    glUseProgram(0);
}
static nu_u32_t
push_im_positions (const nu_f32_t *positions, nu_u32_t count)
{
    nu_u32_t offset = renderer.im_vbo_offset;
    renderer.im_vbo_offset += count;
    nu_f32_t *data = renderer.im_vbo_data;
    data += offset * VERTEX_SIZE;
    for (nu_size_t i = 0; i < count; ++i)
    {
        data[i * VERTEX_SIZE + VERTEX_POSITION_OFFSET + 0]
            = positions[i * 3 + 0];
        data[i * VERTEX_SIZE + VERTEX_POSITION_OFFSET + 1]
            = positions[i * 3 + 1];
        data[i * VERTEX_SIZE + VERTEX_POSITION_OFFSET + 2]
            = positions[i * 3 + 2];
    }
    return offset;
}
static void
draw_volume (const nu_f32_t *center, const nu_f32_t *size, nu_m4_t transform)
{
    // Update ubo
    renderer.ubo.is_volume = NU_TRUE;
    update_ubo();

    glUseProgram(renderer.unlit_program);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    const nu_b3_t box = nu_b3(nu_v3(center[0], center[1], center[2]),
                              nu_v3(size[0], size[1], size[2]));

    const nu_v3_t v0 = nu_v3(box.min.x, box.min.y, box.min.z);
    const nu_v3_t v1 = nu_v3(box.max.x, box.min.y, box.min.z);
    const nu_v3_t v2 = nu_v3(box.max.x, box.min.y, box.max.z);
    const nu_v3_t v3 = nu_v3(box.min.x, box.min.y, box.max.z);

    const nu_v3_t v4 = nu_v3(box.min.x, box.max.y, box.min.z);
    const nu_v3_t v5 = nu_v3(box.max.x, box.max.y, box.min.z);
    const nu_v3_t v6 = nu_v3(box.max.x, box.max.y, box.max.z);
    const nu_v3_t v7 = nu_v3(box.min.x, box.max.y, box.max.z);

    const nu_v3_t positions[]
        = { v0, v1, v2, v3, v0, v2, v5, v4, v6, v6, v4, v7,
            v0, v3, v7, v0, v7, v4, v1, v5, v6, v1, v6, v2,
            v0, v4, v5, v0, v5, v1, v3, v2, v6, v3, v6, v7 };

    nu_u32_t offset = push_im_positions((const nu_f32_t *)positions,
                                        NU_ARRAY_SIZE(positions));

    glUniformMatrix4fv(glGetUniformLocation(renderer.unlit_program, "model"),
                       1,
                       GL_FALSE,
                       transform.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.white_texture);
    glBindVertexArray(renderer.im_vao);
    glDrawArrays(GL_TRIANGLES, offset, 6 * 2 * 3);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(0);
}
static void
draw_lines (const nu_f32_t *points,
            nu_u32_t        count,
            nu_bool_t       linestrip,
            nu_m4_t         transform)
{
    glUseProgram(renderer.unlit_program);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    update_ubo();
    nu_u32_t offset = push_im_positions(points, count);

    glUniformMatrix4fv(glGetUniformLocation(renderer.unlit_program, "model"),
                       1,
                       GL_FALSE,
                       transform.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.white_texture);
    glBindVertexArray(renderer.im_vao);
    glDrawArrays(linestrip ? GL_LINE_STRIP : GL_LINES, offset, count);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
}
static void
draw_cube (const nu_f32_t *pos, const nu_f32_t *size, nu_m4_t transform)
{
    nu_b3_t box = nu_b3(nu_v3(pos[0], pos[1], pos[2]),
                        nu_v3(size[0], size[1], size[2]));

    const nu_v3_t v0 = nu_v3(box.min.x, box.min.y, box.min.z);
    const nu_v3_t v1 = nu_v3(box.max.x, box.min.y, box.min.z);
    const nu_v3_t v2 = nu_v3(box.max.x, box.min.y, box.max.z);
    const nu_v3_t v3 = nu_v3(box.min.x, box.min.y, box.max.z);

    const nu_v3_t v4 = nu_v3(box.min.x, box.max.y, box.min.z);
    const nu_v3_t v5 = nu_v3(box.max.x, box.max.y, box.min.z);
    const nu_v3_t v6 = nu_v3(box.max.x, box.max.y, box.max.z);
    const nu_v3_t v7 = nu_v3(box.min.x, box.max.y, box.max.z);

    const nu_v3_t positions[]
        = { v0, v1, v1, v2, v2, v3, v3, v0, v4, v5, v5, v6,
            v6, v7, v7, v4, v0, v4, v1, v5, v2, v6, v3, v7 };

    draw_lines((const nu_f32_t *)positions, 12 * 2, NU_FALSE, transform);
}
static void
blit (GLuint texture, nu_size_t first, nu_size_t count)
{
    glUseProgram(renderer.canvas_blit_program);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    nu_v2u_t size = nu_v2u(NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);
    glUniform2uiv(
        glGetUniformLocation(renderer.canvas_blit_program, "viewport_size"),
        1,
        size.data);
    glBindVertexArray(renderer.blit_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, count, first);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}
static void
draw_text (const void *text, nu_u32_t len, nu_v2u_t cursor)
{
    // Transfer buffer
    glBindBuffer(GL_ARRAY_BUFFER, renderer.blit_vbo);
    blit_t *blits = (blit_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(blits);
    nu_size_t blit_start = renderer.blit_count;

    const font_t *font   = &renderer.font;
    nu_sv_t       sv     = nu_sv(text, len);
    nu_b2i_t      extent = nu_b2i_xywh(
        cursor.x, cursor.y, font->glyph_size.x, font->glyph_size.y);
    nu_size_t  it = 0;
    nu_wchar_t c;
    while (nu_sv_next(sv, &it, &c))
    {
        if (c == '\n')
        {
            extent = nu_b2i_moveto(
                extent, nu_v2i(cursor.x, extent.min.y + font->glyph_size.y));
            continue;
        }
        if (c < font->min_char || c > font->max_char)
        {
            continue;
        }
        nu_size_t gi         = c - font->min_char;
        nu_b2i_t  tex_extent = font->glyphs[gi];

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
        blit(font->texture, blit_start, renderer.blit_count - blit_start);
    }
}
static void
draw_blit (nu_v2u_t cursor,
           nu_u32_t id,
           nu_u32_t x,
           nu_u32_t y,
           nu_u32_t w,
           nu_u32_t h)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer.blit_vbo);
    blit_t *blits = (blit_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(renderer.blit_count < MAX_BLIT_COUNT);
    blit_t *b = blits + renderer.blit_count++;
    b->pos    = ((nu_u32_t)cursor.y << 16) | (nu_u32_t)cursor.x;
    b->tex    = (y << 16) | x;
    b->size   = (h << 16) | w;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    blit(renderer.objects[id].texture.handle, renderer.blit_count - 1, 1);
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
                 NUX_SCREEN_WIDTH,
                 NUX_SCREEN_HEIGHT,
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
                 NUX_SCREEN_WIDTH,
                 NUX_SCREEN_HEIGHT,
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

    // Create state ubo
    glGenBuffers(1, &renderer.ubo_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.ubo_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_t), NULL, GL_STATIC_DRAW);
    glUniformBlockBinding(renderer.unlit_program,
                          glGetUniformBlockIndex(renderer.unlit_program, "UBO"),
                          1);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, renderer.ubo_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    init_ubo();

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

    // Initialize memory
    nu_memset(renderer.objects, 0, sizeof(renderer.objects));

    renderer.ubo_dirty = NU_TRUE;

    // Mesh VBO
    renderer.mesh_vbo_offset = 0;
    gen_vertex_vbo(&renderer.mesh_vbo, &renderer.mesh_vao, VERTEX_INIT_SIZE);
    renderer.im_vbo_offset = 0;
    gen_vertex_vbo(&renderer.im_vbo, &renderer.im_vao, VERTEX_INIT_SIZE);

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
    if (renderer.mesh_vao)
    {
        glDeleteVertexArrays(1, &renderer.mesh_vao);
        glDeleteVertexArrays(1, &renderer.mesh_vbo);
    }
    if (renderer.im_vao)
    {
        glDeleteVertexArrays(1, &renderer.im_vao);
        glDeleteVertexArrays(1, &renderer.im_vbo);
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
static void
begin_frame (void)
{
    renderer.blit_count    = 0;
    renderer.im_vbo_offset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, renderer.im_vbo);
    renderer.im_vbo_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render on surface framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.surface_fbo);
    glViewport(0, 0, NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
static void
end_frame (void)
{
    // Unmap buffers
    glBindBuffer(GL_ARRAY_BUFFER, renderer.im_vbo);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void
renderer_render_instance (nux_instance_t inst,
                          nu_b2i_t       viewport,
                          nu_v2u_t       window_size)
{
    // Begin frame
    begin_frame();
    // Execute vm commands
    // nu_v2u_t             cursor    = NU_V2U_ZEROS;
    nux_u32_t            cmds_count;
    const nux_command_t *cmds = nux_instance_get_commands(inst, &cmds_count);
    for (nu_size_t i = 0; i < cmds_count; ++i)
    {
        const nux_command_t *cmd = cmds + i;
        switch (cmd->type)
        {
            case NUX_COMMAND_PUSH_VIEWPORT:
                break;
            case NUX_COMMAND_PUSH_SCISSOR:
                break;
            case NUX_COMMAND_PUSH_CURSOR:
                // cursor = nu_v2u(cmd->cursor[0], cmd->cursor[1]);
                break;
            case NUX_COMMAND_PUSH_COLOR:
                break;
            case NUX_COMMAND_CLEAR: {
                nu_v4_t c = nu_color_to_vec4(nu_color_from_u32(cmd->clear));
                glClearColor(c.x, c.y, c.z, c.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            break;
            case NUX_COMMAND_DRAW_SCENE:
                draw_scene(inst, cmd->draw_scene.camera);
                break;
            case NUX_COMMAND_DRAW_CUBE:
                break;
            case NUX_COMMAND_DRAW_LINES:
                break;
            case NUX_COMMAND_DRAW_LINESTRIP:
                break;
            case NUX_COMMAND_DRAW_TEXT:
                break;
            case NUX_COMMAND_BLIT:
                // draw_blit(cursor,
                //           cmd->draw_blit.texture,
                //           cmd->blit.x,
                //           cmd->blit.y,
                //           cmd->blit.w,
                //           cmd->blit.h);
                break;
        }
    }
    end_frame();

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
