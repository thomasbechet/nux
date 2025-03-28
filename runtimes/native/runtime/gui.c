#include "runtime.h"

#include <glad/gl.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <rgfw/RGFW.h>

#ifndef NK_TEXT_MAX
#define NK_TEXT_MAX 256
#endif

#ifndef NK_DOUBLE_CLICK_LO
#define NK_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_DOUBLE_CLICK_HI
#define NK_DOUBLE_CLICK_HI 0.2
#endif

struct nk_vertex
{
    float   position[2];
    float   uv[2];
    nk_byte col[4];
};

#ifdef __APPLE__
#define NK_SHADER_VERSION "#version 150\n"
#else
#define NK_SHADER_VERSION "#version 300 es\n"
#endif

typedef struct
{
    enum nk_keys key;
    nk_bool      pressed;
} gui_key_event_t;

static struct
{
    struct nk_buffer            cmds;
    struct nk_draw_null_texture tex_null;
    GLuint                      vbo, vao, ebo;
    GLuint                      prog;
    GLuint                      vert_shdr;
    GLuint                      frag_shdr;
    GLint                       attrib_pos;
    GLint                       attrib_uv;
    GLint                       attrib_col;
    GLint                       uniform_tex;
    GLint                       uniform_proj;
    GLuint                      font_tex;

    RGFW_window         *win;
    int                  width, height;
    int                  display_width, display_height;
    struct nk_context    ctx;
    struct nk_font_atlas atlas;
    struct nk_vec2       fb_scale;
    unsigned int         text[NK_TEXT_MAX];
    int                  text_len;
    struct nk_vec2       scroll;
    double               last_button_click;
    int                  is_double_click_down;
    struct nk_vec2       double_click_pos;
    float                delta_time_seconds_last;
    gui_key_event_t      key_events[64];
    nu_size_t            key_event_count;

    nu_size_t active_view;
} gui;

static const struct
{
    const nu_char_t *name;
    void (*update)(struct nk_context *ctx, struct nk_rect bounds);
} views[] = { { .name = "Home", .update = view_home },
              { .name = "Controls", .update = view_controls },
              { .name = "Settings", .update = view_settings },
#ifdef NUX_BUILD_SDK
              { .name = "Debug", .update = view_debug }
#endif
};

static double
get_time (void)
{
    return ((double)RGFW_getTimeNS() / 1000000000.);
}

static void
upload_atlas (const void *image, int width, int height)
{
    glGenTextures(1, &gui.font_tex);
    glBindTexture(GL_TEXTURE_2D, gui.font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 (GLsizei)width,
                 (GLsizei)height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image);
}
static void
gui_font_stash_begin (struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&gui.atlas);
    nk_font_atlas_begin(&gui.atlas);
    *atlas = &gui.atlas;
}
static void
gui_font_stash_end (void)
{
    const void *image;
    int         w, h;
    image = nk_font_atlas_bake(&gui.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    upload_atlas(image, w, h);
    nk_font_atlas_end(
        &gui.atlas, nk_handle_id((int)gui.font_tex), &gui.tex_null);
    if (gui.atlas.default_font)
    {
        nk_style_set_font(&gui.ctx, &gui.atlas.default_font->handle);
    }
}
static void
gui_device_destroy (void)
{
    glDetachShader(gui.prog, gui.vert_shdr);
    glDetachShader(gui.prog, gui.frag_shdr);
    glDeleteShader(gui.vert_shdr);
    glDeleteShader(gui.frag_shdr);
    glDeleteProgram(gui.prog);
    glDeleteTextures(1, &gui.font_tex);
    glDeleteBuffers(1, &gui.vbo);
    glDeleteBuffers(1, &gui.ebo);
    nk_buffer_free(&gui.cmds);
}
static void
gui_device_create (void)
{
    GLint                status;
    static const GLchar *vertex_shader = NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader = NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    nk_buffer_init_default(&gui.cmds);
    gui.prog      = glCreateProgram();
    gui.vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    gui.frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(gui.vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(gui.frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(gui.vert_shdr);
    glCompileShader(gui.frag_shdr);
    glGetShaderiv(gui.vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(gui.frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(gui.prog, gui.vert_shdr);
    glAttachShader(gui.prog, gui.frag_shdr);
    glLinkProgram(gui.prog);
    glGetProgramiv(gui.prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    gui.uniform_tex  = glGetUniformLocation(gui.prog, "Texture");
    gui.uniform_proj = glGetUniformLocation(gui.prog, "ProjMtx");
    gui.attrib_pos   = glGetAttribLocation(gui.prog, "Position");
    gui.attrib_uv    = glGetAttribLocation(gui.prog, "TexCoord");
    gui.attrib_col   = glGetAttribLocation(gui.prog, "Color");

    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_vertex);
        size_t  vp = offsetof(struct nk_vertex, position);
        size_t  vt = offsetof(struct nk_vertex, uv);
        size_t  vc = offsetof(struct nk_vertex, col);

        glGenBuffers(1, &gui.vbo);
        glGenBuffers(1, &gui.ebo);
        glGenVertexArrays(1, &gui.vao);

        glBindVertexArray(gui.vao);
        glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui.ebo);

        glEnableVertexAttribArray((GLuint)gui.attrib_pos);
        glEnableVertexAttribArray((GLuint)gui.attrib_uv);
        glEnableVertexAttribArray((GLuint)gui.attrib_col);

        glVertexAttribPointer(
            (GLuint)gui.attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void *)vp);
        glVertexAttribPointer(
            (GLuint)gui.attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void *)vt);
        glVertexAttribPointer((GLuint)gui.attrib_col,
                              4,
                              GL_UNSIGNED_BYTE,
                              GL_TRUE,
                              vs,
                              (void *)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

nu_status_t
gui_init (const runtime_config_t *config)
{
    gui.win = window_get_win();
    nk_init_default(&gui.ctx, 0);
    // gui.ctx.clip.copy     = nk_glfw3_clipboard_copy;
    // gui.ctx.clip.paste    = nk_glfw3_clipboard_paste;
    gui.last_button_click = 0;
    gui_device_create();

    gui.is_double_click_down = nk_false;
    gui.double_click_pos     = nk_vec2(0, 0);

    gui.delta_time_seconds_last = (float)get_time();
    gui.key_event_count         = 0;

    struct nk_font_atlas *atlas;
    gui_font_stash_begin(&atlas);
    gui_font_stash_end();

    return NU_SUCCESS;
}
void
gui_free (void)
{
    nk_font_atlas_clear(&gui.atlas);
    nk_free(&gui.ctx);
    gui_device_destroy();
    nu_memset(&gui, 0, sizeof(gui));
}

static struct nk_context *
gui_new_frame (void)
{
    int                i;
    struct nk_context *ctx = &gui.ctx;

    /* update the timer */
    float delta_time_now        = (float)get_time();
    gui.ctx.delta_time_seconds  = delta_time_now - gui.delta_time_seconds_last;
    gui.delta_time_seconds_last = delta_time_now;

    const nu_f32_t gui_scale = 1.0;

    nu_v2u_t size      = window_get_size();
    gui.width          = size.x / gui_scale;
    gui.height         = size.y / gui_scale;
    gui.display_width  = size.x * window_get_scale_factor();
    gui.display_height = size.y * window_get_scale_factor();
    gui.fb_scale.x     = (float)gui.display_width / (float)gui.width;
    gui.fb_scale.y     = (float)gui.display_height / (float)gui.height;

    nk_input_begin(ctx);
    for (i = 0; i < gui.text_len; ++i)
    {
        nk_input_unicode(ctx, gui.text[i]);
    }

#ifdef NK_GL3_MOUSE_GRABBING
    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
    {
        glfwSetInputMode(glfw.win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else if (ctx->input.mouse.ungrab)
    {
        glfwSetInputMode(gui.win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
#endif

    for (nu_size_t i = 0; i < gui.key_event_count; ++i)
    {
        nk_input_key(ctx, gui.key_events[i].key, gui.key_events[i].pressed);
    }
    gui.key_event_count = 0;

    RGFW_point pos = RGFW_window_getMousePoint(gui.win);
    nk_input_motion(ctx, (int)pos.x / gui_scale, (int)pos.y / gui_scale);
    nk_input_scroll(ctx, gui.scroll);
#ifdef NK_GL3_MOUSE_GRABBING
    if (ctx->input.mouse.grabbed)
    {
        glfwSetCursorPos(
            gui.win, ctx->input.mouse.prev.x, ctx->input.mouse.prev.y);
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }
#endif
    nk_input_button(ctx,
                    NK_BUTTON_LEFT,
                    (int)pos.x / gui_scale,
                    (int)pos.y / gui_scale,
                    RGFW_isMousePressed(gui.win, RGFW_mouseLeft));
    nk_input_button(ctx,
                    NK_BUTTON_MIDDLE,
                    (int)pos.x / gui_scale,
                    (int)pos.y / gui_scale,
                    RGFW_isMousePressed(gui.win, RGFW_mouseMiddle));
    nk_input_button(ctx,
                    NK_BUTTON_RIGHT,
                    (int)pos.x / gui_scale,
                    (int)pos.y / gui_scale,
                    RGFW_isMousePressed(gui.win, RGFW_mouseRight));
    nk_input_button(ctx,
                    NK_BUTTON_DOUBLE,
                    (int)gui.double_click_pos.x,
                    (int)gui.double_click_pos.y,
                    gui.is_double_click_down);

    nk_input_end(&gui.ctx);
    gui.text_len = 0;
    gui.scroll   = nk_vec2(0, 0);
    return &gui.ctx;
}
void
gui_update (void)
{
    // Cartridge / Run
    // - Open cartridge
    // - Open cartridge network
    // - Reset cartridge
    // - Save state
    // - Load state
    // Control window
    // - Input mapping
    // System window
    // - Network config
    // Debug window
    // - Inspector
    // - Resource viewer
    // - Resource hotreload
    // Editor window
    // - Load project
    // - Build project
    // - Resource assembly

    nu_v2u_t           size = window_get_size();
    struct nk_context *ctx  = gui_new_frame();

    // Views panel
    const struct nk_rect menu_bounds = nk_rect(0, 0, size.x, 40);
    if (nk_begin(ctx, "main", menu_bounds, NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_template_begin(ctx, 30);
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(views); ++i)
        {
            nk_layout_row_template_push_static(ctx, 130);
        }
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_end(ctx);

        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(views); ++i)
        {
            if (nk_button_symbol_label(ctx,
                                       (gui.active_view == i)
                                           ? NK_SYMBOL_CIRCLE_SOLID
                                           : NK_SYMBOL_CIRCLE_OUTLINE,
                                       views[i].name,
                                       NK_TEXT_CENTERED))
            {
                gui.active_view = i;
            }
        }

        nk_spacer(ctx);
        if (nk_button_label(ctx, "Exit"))
        {
            runtime_quit();
        }

        nk_end(ctx);
    }

    // Active view
    struct nk_rect viewport
        = nk_rect(0, menu_bounds.h, menu_bounds.w, size.y - menu_bounds.h);
    views[gui.active_view].update(ctx, viewport);

    // Post update
    if (gui.is_double_click_down)
    {
        gui.is_double_click_down = NU_FALSE;
    }
}
void
gui_render (void)
{
    int                         max_vertex_buffer  = NU_MEM_1M;
    int                         max_element_buffer = NU_MEM_1M;
    const enum nk_anti_aliasing aa                 = NK_ANTI_ALIASING_ON;
    struct nk_buffer            vbuf, ebuf;
    GLfloat                     ortho[4][4] = {
        { 2.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, -2.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f, 1.0f },
    };
    ortho[0][0] /= (GLfloat)gui.width;
    ortho[1][1] /= (GLfloat)gui.height;

    // setup global state
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // setup program
    glUseProgram(gui.prog);
    glUniform1i(gui.uniform_tex, 0);
    glUniformMatrix4fv(gui.uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    glViewport(0, 0, (GLsizei)gui.display_width, (GLsizei)gui.display_height);
    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        void                         *vertices, *elements;
        nk_size                       offset = 0;

        /* allocate vertex and element buffer */
        glBindVertexArray(gui.vao);
        glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui.ebo);

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        // load draw vertices & elements directly into vertex + element buffer
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            // fill convert configuration
            struct nk_convert_config                          config;
            static const struct nk_draw_vertex_layout_element vertex_layout[]
                = { { NK_VERTEX_POSITION,
                      NK_FORMAT_FLOAT,
                      NK_OFFSETOF(struct nk_vertex, position) },
                    { NK_VERTEX_TEXCOORD,
                      NK_FORMAT_FLOAT,
                      NK_OFFSETOF(struct nk_vertex, uv) },
                    { NK_VERTEX_COLOR,
                      NK_FORMAT_R8G8B8A8,
                      NK_OFFSETOF(struct nk_vertex, col) },
                    { NK_VERTEX_LAYOUT_END } };
            memset(&config, 0, sizeof(config));
            config.vertex_layout        = vertex_layout;
            config.vertex_size          = sizeof(struct nk_vertex);
            config.vertex_alignment     = NK_ALIGNOF(struct nk_vertex);
            config.tex_null             = gui.tex_null;
            config.circle_segment_count = 22;
            config.curve_segment_count  = 22;
            config.arc_segment_count    = 22;
            config.global_alpha         = 1.0f;
            config.shape_AA             = aa;
            config.line_AA              = aa;

            // setup buffers to load vertices and elements
            nk_buffer_init_fixed(&vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (size_t)max_element_buffer);
            nk_convert(&gui.ctx, &gui.cmds, &vbuf, &ebuf, &config);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        // iterate over and execute each draw command
        nk_draw_foreach(cmd, &gui.ctx, &gui.cmds)
        {
            if (!cmd->elem_count)
            {
                continue;
            }
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor((GLint)(cmd->clip_rect.x * gui.fb_scale.x),
                      (GLint)((gui.height
                               - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h))
                              * gui.fb_scale.y),
                      (GLint)(cmd->clip_rect.w * gui.fb_scale.x),
                      (GLint)(cmd->clip_rect.h * gui.fb_scale.y));
            glDrawElements(GL_TRIANGLES,
                           (GLsizei)cmd->elem_count,
                           GL_UNSIGNED_SHORT,
                           (const void *)offset);
            offset += cmd->elem_count * sizeof(nk_draw_index);
        }
        nk_clear(&gui.ctx);
        nk_buffer_clear(&gui.cmds);
    }

    // default OpenGL state
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void
gui_char_event (RGFW_window *win, unsigned int codepoint)
{
    if (gui.text_len < NK_TEXT_MAX)
    {
        gui.text[gui.text_len++] = codepoint;
    }
}
void
gui_key_event (RGFW_key key, nu_bool_t pressed)
{
    enum nk_keys nkkey = NK_KEY_NONE;
    switch (key)
    {
        case RGFW_delete:
            nkkey = NK_KEY_DEL;
            break;
        case RGFW_KP_Return:
        case RGFW_return:
            nkkey = NK_KEY_ENTER;
            break;
        case RGFW_tab:
            nkkey = NK_KEY_TAB;
            break;
        case RGFW_backSpace:
            nkkey = NK_KEY_BACKSPACE;
            break;
        case RGFW_up:
            nkkey = NK_KEY_UP;
            break;
        case RGFW_down:
            nkkey = NK_KEY_DOWN;
            break;
        case RGFW_left:
            nkkey = NK_KEY_LEFT;
            break;
        case RGFW_right:
            nkkey = NK_KEY_RIGHT;
            break;
        case RGFW_home:
            nkkey = NK_KEY_TEXT_START;
            break;
        case RGFW_end:
            nkkey = NK_KEY_TEXT_END;
            break;
        case RGFW_pageUp:
            nkkey = NK_KEY_SCROLL_UP;
            break;
        case RGFW_pageDown:
            nkkey = NK_KEY_SCROLL_DOWN;
            break;
        case RGFW_shiftR:
        case RGFW_shiftL:
            nkkey = NK_KEY_SHIFT;
            break;
    }
    if (RGFW_isPressed(gui.win, RGFW_controlL)
        || RGFW_isPressed(gui.win, RGFW_controlR))
    {
        switch (key)
        {
            case RGFW_c:
                nkkey = NK_KEY_COPY;
                break;
            case RGFW_v:
                nkkey = NK_KEY_PASTE;
                break;
            case RGFW_x:
                nkkey = NK_KEY_CUT;
                break;
            case RGFW_z:
                nkkey = NK_KEY_TEXT_UNDO;
                break;
            case RGFW_r:
                nkkey = NK_KEY_TEXT_REDO;
                break;
            case RGFW_left:
                nkkey = NK_KEY_TEXT_WORD_LEFT;
                break;
            case RGFW_right:
                nkkey = NK_KEY_TEXT_WORD_RIGHT;
                break;
        }
    }
    else
    {
        // TODO: cancel copy/paste
    }

    if (nkkey == NK_KEY_NONE
        || gui.key_event_count >= NU_ARRAY_SIZE(gui.key_events))
    {
        return;
    }
    gui.key_events[gui.key_event_count].key     = nkkey;
    gui.key_events[gui.key_event_count].pressed = pressed;
    ++gui.key_event_count;
}
void
gui_mouse_button_callback (RGFW_window *win,
                           int          button,
                           double       scroll,
                           int          pressed)
{
    if (button == RGFW_mouseScrollDown || button == RGFW_mouseScrollUp)
    {
        gui.scroll = nk_vec2(0, scroll);
    }
    if (button != RGFW_mouseLeft)
    {
        return;
    }
    RGFW_point p = RGFW_window_getMousePoint(win);
    if (pressed)
    {
        double time = get_time();
        double dt   = time - gui.last_button_click;
        if (dt > NK_DOUBLE_CLICK_LO && dt < NK_DOUBLE_CLICK_HI)
        {
            gui.is_double_click_down = nk_true;
            gui.double_click_pos     = nk_vec2((float)p.x, (float)p.y);
        }
        gui.last_button_click = time;
    }
    else
    {
        gui.is_double_click_down = nk_false;
    }
}
nu_bool_t
gui_is_double_click (void)
{
    return gui.is_double_click_down;
}
