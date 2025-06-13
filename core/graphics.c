#include "internal.h"

#include "fonts_data.c.inc"

enum
{
    PIPELINE_CANVAS    = 0,
    PIPELINE_MAIN      = 1,
    TEXTURE_COLORMAP   = 0,
    TEXTURE_CANVAS     = 1,
    BUFFER_CONSTANTS   = 0,
    BUFFER_VERTICES    = 1,
    FRAMEBUFFER_CANVAS = 0,
};

enum
{
    VERTEX_SIZE           = 5,
    VERTICES_DEFAULT_SIZE = 1024,
};

nux_status_t
nux_graphics_init (nux_env_t *env)
{
    nux_frame_t frame = nux_begin_frame(env);

    // Initialize gpu slots
    NUX_CHECKM(nux_u32_vec_alloc(
                   env, NUX_GPU_TEXTURE_MAX, &env->inst->free_texture_slots),
               "Failed to allocate gpu texture slots",
               goto error);
    nux_u32_vec_fill_reverse_indices(&env->inst->free_texture_slots);

    // Allocate canvas
    env->inst->canvas = nux_arena_alloc(&env->inst->core_arena,
                                        NUX_CANVAS_WIDTH * NUX_CANVAS_HEIGHT);
    NUX_CHECKM(env->inst->canvas, "Failed to allocate canvas", goto error);

    // Initialize state
    nux_palr(env);

    // Create pipelines
    NUX_CHECKM(nux_os_create_pipeline(
                   env->inst->userdata, PIPELINE_MAIN, NUX_GPU_PASS_MAIN),
               "Failed to create main pipeline",
               goto error);
    NUX_CHECKM(nux_os_create_pipeline(
                   env->inst->userdata, PIPELINE_CANVAS, NUX_GPU_PASS_CANVAS),
               "Failed to create canvas pipeline",
               return NUX_FAILURE);

    // Create textures
    env->inst->colormap_info
        = (nux_gpu_texture_info_t) { .format = NUX_TEXTURE_FORMAT_RGBA,
                                     .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
                                     .width  = NUX_COLORMAP_SIZE,
                                     .height = 1 };
    NUX_CHECKM(nux_os_create_texture(env->inst->userdata,
                                     TEXTURE_COLORMAP,
                                     &env->inst->colormap_info),
               "Failed to create colormap texture",
               goto error);

    env->inst->canvas_info
        = (nux_gpu_texture_info_t) { .format = NUX_TEXTURE_FORMAT_INDEX,
                                     .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
                                     .width  = NUX_CANVAS_WIDTH,
                                     .height = NUX_CANVAS_HEIGHT };
    NUX_CHECKM(nux_os_create_texture(env->inst->userdata,
                                     TEXTURE_CANVAS,
                                     &env->inst->canvas_info),
               "Failed to create canvas texture",
               goto error);

    // Create buffers
    NUX_CHECKM(nux_os_create_buffer(env->inst->userdata,
                                    BUFFER_CONSTANTS,
                                    NUX_GPU_BUFFER_UNIFORM,
                                    sizeof(nux_gpu_constants_buffer_t)),
               "Failed to create uniform buffer",
               goto error);

    NUX_CHECKM(nux_os_create_buffer(env->inst->userdata,
                                    BUFFER_VERTICES,
                                    NUX_GPU_BUFFER_STORAGE,
                                    VERTEX_SIZE * VERTICES_DEFAULT_SIZE),
               "Failed to create vertex buffer",
               goto error);
    env->inst->vertex_storage_head = 0;

    // Create canvas framebuffer
    NUX_CHECKM(nux_os_create_framebuffer(
                   env->inst->userdata, FRAMEBUFFER_CANVAS, TEXTURE_CANVAS),
               "Failed to create canvas framebuffer",
               goto error);

    env->inst->test_cube = nux_generate_cube(&env->inst->env, 1, 1, 1);
    NUX_ASSERT(env->inst->test_cube);

    return NUX_SUCCESS;

error:
    nux_reset_frame(env, frame);
    return NUX_FAILURE;
}
nux_status_t
nux_graphics_free (nux_env_t *env)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_render (nux_env_t *env)
{
    // Update colormap
    NUX_CHECKM(nux_os_update_texture(env->inst->userdata,
                                     TEXTURE_COLORMAP,
                                     0,
                                     0,
                                     env->inst->colormap_info.width,
                                     env->inst->colormap_info.height,
                                     env->inst->colormap),
               "Failed to update colormap texture",
               return NUX_FAILURE);

    // Update canvas
    NUX_CHECKM(nux_os_update_texture(env->inst->userdata,
                                     TEXTURE_CANVAS,
                                     0,
                                     0,
                                     env->inst->canvas_info.width,
                                     env->inst->canvas_info.height,
                                     env->inst->canvas),
               "Failed to update canvas texture",
               return NUX_FAILURE);

    // Update storage buffer
    nux_gpu_constants_buffer_t uniform_buffer;
    uniform_buffer.view = nux_lookat(nux_v3s(1), nux_v3s(0), nux_v3(0, 1, 0));
    uniform_buffer.proj
        = nux_perspective(nux_radian(60),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          0.1,
                          100);
    uniform_buffer.model = nux_m4_rotate_y(nux_radian(env->inst->time) * 10);
    uniform_buffer.screen_size
        = nux_v2u(env->inst->stats[NUX_STAT_SCREEN_WIDTH],
                  env->inst->stats[NUX_STAT_SCREEN_HEIGHT]);
    uniform_buffer.canvas_size = nux_v2u(NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    uniform_buffer.time        = env->inst->time;
    nux_os_update_buffer(env->inst->userdata,
                         BUFFER_CONSTANTS,
                         0,
                         sizeof(uniform_buffer),
                         &uniform_buffer);

    // Blit canvas
    {
        nux_gpu_command_t cmds[] = { {
            .canvas.texture      = TEXTURE_CANVAS,
            .canvas.colormap     = TEXTURE_COLORMAP,
            .canvas.vertex_count = 3,
        } };
        nux_gpu_pass_t    pass   = {
                 .type                  = NUX_GPU_PASS_CANVAS,
                 .pipeline              = PIPELINE_CANVAS,
                 .canvas.uniform_buffer = BUFFER_CONSTANTS,
                 .count                 = NUX_ARRAY_SIZE(cmds),
        };
        nux_os_gpu_submit_pass(env->inst->userdata, &pass, cmds);
    }

    // Render cube
    {
        nux_gpu_command_t cmds[] = { { .main.texture      = 0,
                                       .main.colormap     = 0,
                                       .main.storage      = BUFFER_VERTICES,
                                       .main.vertex_base  = 0,
                                       .main.vertex_count = 36 } };
        nux_gpu_pass_t    pass   = {
                 .type                = NUX_GPU_PASS_MAIN,
                 .pipeline            = PIPELINE_MAIN,
                 .main.uniform_buffer = BUFFER_CONSTANTS,
                 .count               = NUX_ARRAY_SIZE(cmds),
        };
        nux_os_gpu_submit_pass(env->inst->userdata, &pass, cmds);
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_graphics_push_vertices (nux_env_t       *env,
                            nux_u32_t        vcount,
                            const nux_f32_t *data,
                            nux_u32_t       *first)
{
    NUX_CHECKM(env->inst->vertex_storage_head + vcount < VERTICES_DEFAULT_SIZE,
               "Out of vertices",
               return NUX_FAILURE);
    NUX_CHECKM(nux_os_update_buffer(env->inst->userdata,
                                    BUFFER_VERTICES,
                                    env->inst->vertex_storage_head * VERTEX_SIZE
                                        * sizeof(nux_f32_t),
                                    vcount * VERTEX_SIZE * sizeof(nux_f32_t),
                                    data),
               "Failed to update vertex buffer",
               return NUX_FAILURE);
    *first = env->inst->vertex_storage_head;
    env->inst->vertex_storage_head += vcount;
    return NUX_SUCCESS;
}

void
nux_pal (nux_env_t *env, nux_u8_t index, nux_u8_t color)
{
    env->inst->pal[index] = color;
}
void
nux_palt (nux_env_t *env, nux_u8_t c)
{
}
void
nux_palr (nux_env_t *env)
{
    for (nux_u32_t i = 0; i < NUX_PALETTE_SIZE; ++i)
    {
        env->inst->pal[i] = i;
    }
    nux_palt(env, 0);
}
nux_u8_t
nux_palc (nux_env_t *env, nux_u8_t index)
{
    return env->inst->pal[index];
}
void
nux_cls (nux_env_t *env, nux_u32_t color)
{
    nux_rectfill(env, 0, 0, NUX_CANVAS_WIDTH - 1, NUX_CANVAS_HEIGHT - 1, color);
}
void
nux_text (
    nux_env_t *env, nux_i32_t x, nux_i32_t y, const nux_c8_t *text, nux_u8_t c)
{
    const nux_u32_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;

    nux_u32_t len = nux_strnlen(text, 1024);
    nux_b2i_t extent
        = nux_b2i_xywh(x, y, DEFAULT_FONT_DATA_WIDTH, DEFAULT_FONT_DATA_HEIGHT);
    const nux_c8_t *end = text + len;
    const nux_c8_t *it  = text;
    for (; it < end; ++it)
    {
        if (*it == '\n')
        {
            extent = nux_b2i_moveto(
                extent, nux_v2i(x, extent.min.y + DEFAULT_FONT_DATA_HEIGHT));
            continue;
        }
        nux_u32_t index = default_font_data_chars[(nux_u8_t)(*it)];

        for (nux_u32_t p = 0; p < pixel_per_glyph; ++p)
        {
            nux_u32_t bit_offset = index * pixel_per_glyph + p;
            NUX_ASSERT((bit_offset / 8) < sizeof(default_font_data));
            nux_u8_t  byte    = default_font_data[bit_offset / 8];
            nux_b32_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nux_i32_t px = extent.min.x + p % DEFAULT_FONT_DATA_WIDTH;
            nux_i32_t py = extent.min.y + p / DEFAULT_FONT_DATA_WIDTH;

            if (bit_set)
            {
                nux_pset(env, px, py, c);

                // Apply shadow
                nux_pset(env, px + 1, py + 1, 0);
            }
        }

        extent = nux_b2i_translate(extent, nux_v2i(DEFAULT_FONT_DATA_WIDTH, 0));
    }
}
void
nux_print (nux_env_t *env, const nux_c8_t *text, nux_u8_t c)
{
    nux_i32_t x = nux_cursorx(env);
    nux_i32_t y = nux_cursory(env);
    nux_text(env, x, y, text, c);
    nux_cursor(env, x, y + DEFAULT_FONT_DATA_HEIGHT);
}
#ifdef NUX_BUILD_VARARGS
void
nux_textfmt (nux_env_t      *env,
             nux_i32_t       x,
             nux_i32_t       y,
             nux_u8_t        c,
             const nux_c8_t *fmt,
             ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_text(env, x, y, buf, c);
}
void
nux_printfmt (nux_env_t *env, nux_u8_t c, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_print(env, buf, c);
}
void
nux_tracefmt (nux_env_t *env, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_trace(env, buf);
}
#endif

nux_i32_t
nux_cursorx (nux_env_t *env)
{
    return env->inst->cursor.x;
}
nux_i32_t
nux_cursory (nux_env_t *env)
{
    return env->inst->cursor.y;
}
void
nux_cursor (nux_env_t *env, nux_i32_t x, nux_i32_t y)
{
    env->inst->cursor = nux_v2i(x, y);
}
nux_u32_t
nux_cget (nux_env_t *env, nux_u8_t index)
{
    const nux_u8_t *map = (const nux_u8_t *)env->inst->colormap;
    return (map[index * 3 + 0] << 16 | map[index * 3 + 1] << 8
            | map[index * 3 + 0]);
}
void
nux_cset (nux_env_t *env, nux_u8_t index, nux_u32_t color)
{
    nux_u8_t *map      = (nux_u8_t *)env->inst->colormap;
    map[index * 3 + 0] = (color & 0xFF0000) >> 16;
    map[index * 3 + 1] = (color & 0xFF00) >> 8;
    map[index * 3 + 2] = color & 0xFF;
}
void
nux_cameye (nux_env_t *env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    env->inst->cam_eye = nux_v3(x, y, z);
}
void
nux_camcenter (nux_env_t *env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    env->inst->cam_center = nux_v3(x, y, z);
}
void
nux_camup (nux_env_t *env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    env->inst->cam_up = nux_v3(x, y, z);
}
void
nux_camviewport (
    nux_env_t *env, nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h)
{
    env->inst->cam_viewport = nux_b2i_xywh(x, y, w, h);
}
void
nux_camfov (nux_env_t *env, nux_f32_t fov)
{
    env->inst->cam_fov = fov;
}

void
nux_write_texture (nux_env_t      *env,
                   nux_u32_t       x,
                   nux_u32_t       y,
                   nux_u32_t       w,
                   nux_u32_t       h,
                   const nux_u8_t *data)
{
    // Clamp to region
    x = NUX_MIN(x, NUX_TEXTURE_WIDTH - 1);
    y = NUX_MIN(y, NUX_TEXTURE_HEIGHT - 1);
    w = NUX_MIN(w, NUX_TEXTURE_WIDTH - x - 1);
    h = NUX_MIN(h, NUX_TEXTURE_HEIGHT - y - 1);

    // Copy row by row
    // nux_u8_t *tex = NUX_MEMPTR(env->inst, NUX_RAM_TEXTURE, nux_u8_t);
    // for (nux_u32_t i = 0; i < h; ++i)
    // {
    //     nux_u8_t       *dst = tex + ((y + i) * NUX_TEXTURE_WIDTH + x);
    //     const nux_u8_t *src = data + (i * w);
    //     nux_memcpy(dst, src, w);
    // }
}

nux_u32_t
nux_generate_cube (nux_env_t *env, nux_f32_t sx, nux_f32_t sy, nux_f32_t sz)
{
    nux_frame_t frame = nux_begin_frame(env);

    const nux_b3_t box = nux_b3(nux_v3s(0), nux_v3(sx / 2, sy / 2, sz / 2));

    const nux_v3_t v0 = nux_v3(box.min.x, box.min.y, box.min.z);
    const nux_v3_t v1 = nux_v3(box.max.x, box.min.y, box.min.z);
    const nux_v3_t v2 = nux_v3(box.max.x, box.min.y, box.max.z);
    const nux_v3_t v3 = nux_v3(box.min.x, box.min.y, box.max.z);

    const nux_v3_t v4 = nux_v3(box.min.x, box.max.y, box.min.z);
    const nux_v3_t v5 = nux_v3(box.max.x, box.max.y, box.min.z);
    const nux_v3_t v6 = nux_v3(box.max.x, box.max.y, box.max.z);
    const nux_v3_t v7 = nux_v3(box.min.x, box.max.y, box.max.z);

    const nux_v3_t positions[]
        = { v0, v1, v2, v2, v3, v0, v4, v6, v5, v6, v4, v7,
            v0, v3, v7, v7, v4, v0, v1, v5, v6, v6, v2, v1,
            v0, v4, v5, v5, v1, v0, v3, v2, v6, v6, v7, v3 };

    const nux_v2_t uvs[] = {
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } }, { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 0, 0 } }, { { 0, 1 } }, { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 1, 0 } },
        { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } },
        { { 0, 1 } }, { { 1, 1 } }, { { 1, 1 } }, { { 1, 0 } }, { { 0, 0 } },
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } },
    };

    nux_u32_t   id;
    nux_mesh_t *mesh = NUX_NEW_STRUCT(env, NUX_OBJECT_MESH, nux_mesh_t, &id);
    NUX_CHECKM(mesh, "Failed to create cube mesh object", goto cleanup);
    mesh->count = NUX_ARRAY_SIZE(positions);
    mesh->data  = nux_alloc(env, sizeof(nux_f32_t) * 5 * mesh->count);
    NUX_CHECKM(mesh->data, "Failed to allocate cube mesh data", goto cleanup);

    for (nux_u32_t i = 0; i < mesh->count; ++i)
    {
        mesh->data[i * 5 + 0] = positions[i].x;
        mesh->data[i * 5 + 1] = positions[i].y;
        mesh->data[i * 5 + 2] = positions[i].z;
        mesh->data[i * 5 + 3] = uvs[i].x;
        mesh->data[i * 5 + 4] = uvs[i].y;
    }

    NUX_CHECKM(
        nux_graphics_push_vertices(env, mesh->count, mesh->data, &mesh->first),
        "Failed to push cube vertices",
        goto cleanup);

    return NUX_SUCCESS;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_FAILURE;
}

nux_u32_t
nux_new_texture (nux_env_t           *env,
                 nux_texture_format_t format,
                 nux_u32_t            w,
                 nux_u32_t            h)
{
    nux_frame_t frame = nux_begin_frame(env);

    // Create object
    nux_u32_t      id;
    nux_texture_t *tex
        = NUX_NEW_STRUCT(env, NUX_OBJECT_TEXTURE, nux_texture_t, &id);
    NUX_CHECKM(tex, "Failed to create texture object", return NUX_NULL);
    tex->format = format;
    tex->width  = w;
    tex->height = h;

    // Create gpu texture
    nux_gpu_texture_info_t info = {
        .format = format,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = w,
        .height = h,
    };
    nux_u32_t *slot = nux_u32_vec_pop(&env->inst->free_texture_slots);
    NUX_CHECKM(slot, "Out of gpu textures", goto cleanup);
    tex->slot = *slot;
    NUX_CHECKM(nux_os_create_texture(env->inst->userdata, tex->slot, &info),
               "Failed to create texture",
               goto cleanup);

    // Allocate memory
    nux_u32_t sample_size = 0;
    switch (format)
    {
        case NUX_TEXTURE_FORMAT_RGBA:
            sample_size = sizeof(nux_u32_t);
            break;
        case NUX_TEXTURE_FORMAT_INDEX:
            sample_size = sizeof(nux_u8_t);
            break;
    }
    NUX_CHECKM(sample_size, "Invalid texture format", goto cleanup);
    tex->data = nux_alloc(env, sample_size * w * h);
    NUX_CHECKM(tex->data, "Failed to allocate texture data", goto cleanup);
    nux_memset(tex->data, 0, sample_size * w * h);

    return id;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_NULL;
}
nux_u32_t
nux_new_render_target (nux_env_t *env, nux_u32_t w, nux_u32_t h)
{
    nux_frame_t frame = nux_begin_frame(env);

    // Create object
    nux_u32_t            id;
    nux_render_target_t *rt = NUX_NEW_STRUCT(
        env, NUX_OBJECT_RENDER_TARGET, nux_render_target_t, &id);
    NUX_CHECKM(rt, "Failed to create render target", return NUX_NULL);

    // Create texture
    rt->texture = nux_new_texture(env, NUX_TEXTURE_FORMAT_RGBA, w, h);
    NUX_CHECKM(
        rt->texture, "Failed to create texture render target", goto cleanup);

    // Create framebuffer
    nux_u32_t *slot = nux_u32_vec_pop(&env->inst->free_framebuffer_slots);
    NUX_CHECKM(slot, "Out of gpu framebuffer slots", goto cleanup);
    rt->slot = *slot;

    nux_texture_t *tex = nux_get_object(env, rt->texture, NUX_OBJECT_TEXTURE);
    NUX_CHECKM(
        nux_os_create_framebuffer(env->inst->userdata, rt->slot, tex->slot),
        "Failed to create framebuffer",
        goto cleanup);

    return id;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_NULL;
}

void
nux_set_render_target (nux_env_t *env, nux_u32_t id)
{
}
void
nux_blit (nux_env_t *env, nux_u32_t id)
{
}
