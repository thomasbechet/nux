#include "internal.h"

#include "fonts_data.c.inc"
#include "shaders_data.c.inc"

#define PIPELINE_CANVAS             0
#define PIPELINE_MAIN               1
#define TEXTURE_COLORMAP            0
#define TEXTURE_CANVAS              1
#define UNIFORM_BUFFER              0
#define VERTEX_STORAGE_BUFFER       1
#define VERTEX_SIZE                 5
#define VERTEX_STORAGE_DEFAULT_SIZE 1024

nux_status_t
nux_graphics_init (nux_instance_t *inst)
{
    // Create pipelines
    NUX_CHECK(nux_os_create_pipeline(inst->userdata,
                                     PIPELINE_MAIN,
                                     NUX_GPU_SHADER_GLSL,
                                     shader_main_vert,
                                     NUX_ARRAY_SIZE(shader_main_vert),
                                     shader_main_frag,
                                     NUX_ARRAY_SIZE(shader_main_frag)),
              return NUX_FAILURE);
    NUX_CHECK(nux_os_create_pipeline(inst->userdata,
                                     PIPELINE_CANVAS,
                                     NUX_GPU_SHADER_GLSL,
                                     shader_canvas_vert,
                                     NUX_ARRAY_SIZE(shader_canvas_vert),
                                     shader_canvas_frag,
                                     NUX_ARRAY_SIZE(shader_canvas_frag)),
              return NUX_FAILURE);

    // Create textures
    inst->colormap_info
        = (nux_gpu_texture_info_t) { .format = NUX_GPU_TEXTURE_FORMAT_RGBA,
                                     .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
                                     .width  = NUX_COLORMAP_SIZE,
                                     .height = 1 };
    NUX_CHECK(nux_os_create_texture(
                  inst->userdata, TEXTURE_COLORMAP, &inst->colormap_info),
              return NUX_FAILURE);

    inst->canvas_info
        = (nux_gpu_texture_info_t) { .format = NUX_GPU_TEXTURE_FORMAT_INDEX,
                                     .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
                                     .width  = NUX_CANVAS_WIDTH,
                                     .height = NUX_CANVAS_HEIGHT };
    NUX_CHECK(nux_os_create_texture(
                  inst->userdata, TEXTURE_CANVAS, &inst->canvas_info),
              return NUX_FAILURE);

    // Create buffers
    NUX_CHECK(nux_os_create_buffer(inst->userdata,
                                   UNIFORM_BUFFER,
                                   NUX_GPU_BUFFER_UNIFORM,
                                   sizeof(nux_gpu_uniform_buffer_t)),
              return NUX_FAILURE);

    NUX_CHECK(nux_os_create_buffer(inst->userdata,
                                   VERTEX_STORAGE_BUFFER,
                                   NUX_GPU_BUFFER_STORAGE,
                                   VERTEX_SIZE * VERTEX_STORAGE_DEFAULT_SIZE),
              return NUX_FAILURE);
    inst->vertex_storage_head = 0;

    inst->test_cube = nux_generate_cube(&inst->env, 1, 1, 1);
    NUX_ASSERT(inst->test_cube);

    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_free (nux_instance_t *inst)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_render (nux_instance_t *inst)
{
    // Update colormap
    NUX_CHECK(nux_os_update_texture(inst->userdata,
                                    TEXTURE_COLORMAP,
                                    0,
                                    0,
                                    inst->colormap_info.width,
                                    inst->colormap_info.height,
                                    inst->colormap),
              return NUX_FAILURE);

    // Update canvas
    NUX_CHECK(nux_os_update_texture(inst->userdata,
                                    TEXTURE_CANVAS,
                                    0,
                                    0,
                                    inst->canvas_info.width,
                                    inst->canvas_info.height,
                                    inst->canvas),
              return NUX_FAILURE);

    // Update storage buffer
    nux_gpu_uniform_buffer_t uniform_buffer;
    uniform_buffer.view  = nux_lookat(nux_v3s(5), nux_v3s(0), nux_v3(0, 1, 0));
    uniform_buffer.proj  = nux_perspective(nux_radian(60), 1, 0.1, 100);
    uniform_buffer.model = nux_m4_identity();
    nux_os_update_buffer(inst->userdata,
                         UNIFORM_BUFFER,
                         0,
                         sizeof(uniform_buffer),
                         &uniform_buffer);

    // Blit canvas
    nux_gpu_command_t cmds[32];
    nux_u32_t         count = 0;

    cmds[count].type                    = NUX_GPU_BIND_PIPELINE;
    cmds[count].data.bind_pipeline.slot = PIPELINE_MAIN;
    ++count;

    cmds[count].type            = NUX_GPU_DRAW;
    cmds[count].data.draw.count = 36;
    ++count;

    cmds[count].type                    = NUX_GPU_BIND_PIPELINE;
    cmds[count].data.bind_pipeline.slot = PIPELINE_CANVAS;
    ++count;

    cmds[count].type                      = NUX_GPU_BIND_TEXTURE;
    cmds[count].data.bind_texture.slot    = TEXTURE_CANVAS;
    cmds[count].data.bind_texture.binding = 0;
    ++count;

    cmds[count].type                      = NUX_GPU_BIND_TEXTURE;
    cmds[count].data.bind_texture.slot    = TEXTURE_COLORMAP;
    cmds[count].data.bind_texture.binding = 1;
    ++count;

    cmds[count].type            = NUX_GPU_DRAW;
    cmds[count].data.draw.count = 3;
    ++count;

    nux_os_submit_commands(inst->userdata, cmds, count);

    return NUX_SUCCESS;
}

nux_status_t
nux_graphics_push_vertices (nux_instance_t  *inst,
                            nux_u32_t        vcount,
                            const nux_f32_t *data,
                            nux_u32_t       *first)
{
    NUX_CHECK(inst->vertex_storage_head + vcount < VERTEX_STORAGE_DEFAULT_SIZE,
              return NUX_FAILURE);
    NUX_CHECK(nux_os_update_buffer(inst->userdata,
                                   VERTEX_STORAGE_BUFFER,
                                   inst->vertex_storage_head * VERTEX_SIZE
                                       * sizeof(nux_f32_t),
                                   vcount * VERTEX_SIZE * sizeof(nux_f32_t),
                                   data),
              return NUX_FAILURE);
    *first = inst->vertex_storage_head;
    inst->vertex_storage_head += vcount;
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
    nux_mesh_t *mesh = nux_add_object(env, NUX_OBJECT_MESH, &id);
    NUX_CHECK(mesh, return NUX_NULL);
    mesh->count = NUX_ARRAY_SIZE(positions);
    mesh->data  = nux_malloc(env, sizeof(nux_f32_t) * 5 * mesh->count);
    NUX_CHECK(mesh->data, goto cleanup0);

    for (nux_u32_t i = 0; i < mesh->count; ++i)
    {
        mesh->data[i * 5 + 0] = positions[i].x;
        mesh->data[i * 5 + 1] = positions[i].y;
        mesh->data[i * 5 + 2] = positions[i].z;
        mesh->data[i * 5 + 3] = uvs[i].x;
        mesh->data[i * 5 + 4] = uvs[i].y;
    }

    NUX_CHECK(nux_graphics_push_vertices(
                  env->inst, mesh->count, mesh->data, &mesh->first),
              return NUX_FAILURE);

    return NUX_SUCCESS;

cleanup0:
    nux_remove_object(env, id);
    return NUX_FAILURE;
}
