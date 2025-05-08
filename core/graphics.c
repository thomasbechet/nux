#include "internal.h"

#include "fonts_data.c.inc"

void
nux_pal (nux_env_t env, nux_u8_t index, nux_u8_t color)
{
    nux_u8_t *pal = NUX_MEMPTR(env->inst, NUX_RAM_PALETTE, nux_u8_t);
    pal[index]    = color;
}
void
nux_palt (nux_env_t env, nux_u8_t c)
{
}
void
nux_palr (nux_env_t env)
{
    nux_u8_t *pal = NUX_MEMPTR(env->inst, NUX_RAM_PALETTE, nux_u8_t);
    for (nux_u32_t i = 0; i < NUX_PALETTE_SIZE; ++i)
    {
        pal[i] = i;
    }
    nux_palt(env, 0);
}
nux_u8_t
nux_palc (nux_env_t env, nux_u8_t index)
{
    return NUX_MEMPTR(env->inst, NUX_RAM_PALETTE, const nux_u8_t)[index];
}
void
nux_cls (nux_env_t env, nux_u32_t color)
{
    nux_rectfill(env, 0, 0, NUX_CANVAS_WIDTH - 1, NUX_CANVAS_HEIGHT - 1, color);
}
void
nux_text (
    nux_env_t env, nux_i32_t x, nux_i32_t y, const nux_c8_t *text, nux_u8_t c)
{
    const nu_size_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;

    nu_sv_t  sv = nu_sv(text, nu_strnlen(text, 1024));
    nu_b2i_t extent
        = nu_b2i_xywh(x, y, DEFAULT_FONT_DATA_WIDTH, DEFAULT_FONT_DATA_HEIGHT);
    nu_size_t  it = 0;
    nu_wchar_t wchar;
    while (nu_sv_next(sv, &it, &wchar))
    {
        if (wchar > 256)
        {
            continue;
        }
        if (wchar == '\n')
        {
            extent = nu_b2i_moveto(
                extent, nu_v2i(x, extent.min.y + DEFAULT_FONT_DATA_HEIGHT));
            continue;
        }
        nu_size_t index = default_font_data_chars[wchar];

        for (nu_size_t p = 0; p < pixel_per_glyph; ++p)
        {
            nu_size_t bit_offset = index * pixel_per_glyph + p;
            NU_ASSERT((bit_offset / 8) < sizeof(default_font_data));
            nu_byte_t byte    = default_font_data[bit_offset / 8];
            nu_byte_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nux_i32_t px = extent.min.x + p % DEFAULT_FONT_DATA_WIDTH;
            nux_i32_t py = extent.min.y + p / DEFAULT_FONT_DATA_WIDTH;

            if (bit_set)
            {
                nux_pset(env, px, py, c);

                // Apply shadow
                nux_pset(env, px + 1, py + 1, 0);
            }
        }

        extent = nu_b2i_translate(extent, nu_v2i(DEFAULT_FONT_DATA_WIDTH, 0));
    }
}
void
nux_print (nux_env_t env, const nux_c8_t *text, nux_u8_t c)
{
    nux_i32_t x = nux_cursorx(env);
    nux_i32_t y = nux_cursory(env);
    nux_text(env, x, y, text, c);
    nux_cursor(env, x, y + DEFAULT_FONT_DATA_HEIGHT);
}
#ifdef NUX_BUILD_VARARGS
void
nux_textfmt (nux_env_t       env,
             nux_i32_t       x,
             nux_i32_t       y,
             nux_u8_t        c,
             const nux_c8_t *fmt,
             ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nu_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_text(env, x, y, buf, c);
}
void
nux_printfmt (nux_env_t env, nux_u8_t c, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nu_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_print(env, buf, c);
}
void
nux_tracefmt (nux_env_t env, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nu_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_trace(env, buf);
}
#endif

nux_i32_t
nux_cursorx (nux_env_t env)
{
    return NUX_MEMGET(env->inst, NUX_RAM_CURSORX, nux_i32_t);
}
nux_i32_t
nux_cursory (nux_env_t env)
{
    return NUX_MEMGET(env->inst, NUX_RAM_CURSORY, nux_i32_t);
}
void
nux_cursor (nux_env_t env, nux_i32_t x, nux_i32_t y)
{
    NUX_MEMSET(env->inst, NUX_RAM_CURSORX, nux_i32_t, x);
    NUX_MEMSET(env->inst, NUX_RAM_CURSORY, nux_i32_t, y);
}
nux_u32_t
nux_cget (nux_env_t env, nux_u8_t index)
{
    nux_u8_t *map = NUX_MEMPTR(env->inst, NUX_RAM_COLORMAP, nux_u8_t);
    return (map[index * 3 + 0] << 16 | map[index * 3 + 1] << 8
            | map[index * 3 + 0]);
}
void
nux_cset (nux_env_t env, nux_u8_t index, nux_u32_t color)
{
    nux_u8_t *map      = NUX_MEMPTR(env->inst, NUX_RAM_COLORMAP, nux_u8_t);
    map[index * 3 + 0] = (color & 0xFF0000) >> 16;
    map[index * 3 + 1] = (color & 0xFF00) >> 8;
    map[index * 3 + 2] = color & 0xFF;
}
void
nux_cameye (nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_EYE, nux_f32_t)[0] = x;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_EYE, nux_f32_t)[1] = y;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_EYE, nux_f32_t)[2] = z;
}
void
nux_camcenter (nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_CENTER, nux_f32_t)[0] = x;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_CENTER, nux_f32_t)[1] = y;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_CENTER, nux_f32_t)[2] = z;
}
void
nux_camup (nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z)
{
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_UP, nux_f32_t)[0] = x;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_UP, nux_f32_t)[1] = y;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_UP, nux_f32_t)[2] = z;
}
void
nux_camviewport (
    nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h)
{
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_VIEWPORT, nux_u32_t)[0] = x;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_VIEWPORT, nux_u32_t)[1] = y;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_VIEWPORT, nux_u32_t)[2] = w;
    NUX_MEMPTR(env->inst, NUX_RAM_CAM_VIEWPORT, nux_u32_t)[3] = h;
}
void
nux_camfov (nux_env_t env, nux_f32_t fov)
{
    NUX_MEMSET(env->inst, NUX_RAM_CAM_FOV, nux_f32_t, fov);
}

nux_u8_t *
nux_screen (nux_env_t env)
{
    return NUX_MEMPTR(env->inst, NUX_RAM_CANVAS, nux_u8_t);
}
void
nux_bind_texture (nux_env_t          env,
                  nux_u32_t          x,
                  nux_u32_t          y,
                  nux_u32_t          w,
                  nux_u32_t          h,
                  nux_texture_type_t type)
{
    nu_u32_t *view = NUX_MEMPTR(env->inst, NUX_RAM_TEXTURE_VIEW, nu_u32_t);
    view[0]        = x;
    view[1]        = y;
    view[2]        = w;
    view[3]        = h;
}
void
nux_write_texture (nux_env_t       env,
                   nux_u32_t       x,
                   nux_u32_t       y,
                   nux_u32_t       w,
                   nux_u32_t       h,
                   const nux_u8_t *data)
{
    // Clamp to region
    x = NU_MIN(x, NUX_TEXTURE_WIDTH - 1);
    y = NU_MIN(y, NUX_TEXTURE_HEIGHT - 1);
    w = NU_MIN(w, NUX_TEXTURE_WIDTH - x - 1);
    h = NU_MIN(h, NUX_TEXTURE_HEIGHT - y - 1);

    // Copy row by row
    nux_u8_t *tex = NUX_MEMPTR(env->inst, NUX_RAM_TEXTURE, nux_u8_t);
    for (nux_u32_t i = 0; i < h; ++i)
    {
        nux_u8_t       *dst = tex + ((y + i) * NUX_TEXTURE_WIDTH + x);
        const nux_u8_t *src = data + (i * w);
        nu_memcpy(dst, src, w);
    }
}

nux_u32_t
nux_push_gpu_data (nux_env_t env, const nux_f32_t *data, nux_u32_t count)
{
    if (env->inst->gpu_buffer_size + count >= NUX_GPU_BUFFER_SIZE)
    {
        return 0;
    }
    nux_u32_t index = env->inst->gpu_buffer_size;
    nu_memcpy(env->inst->gpu_buffer + index, data, sizeof(*data) * count);
    env->inst->gpu_buffer_size += count;
    return index;
}
nux_gpu_command_t *
nux_push_gpu_command (nux_env_t env)
{
    if (env->inst->gpu_commands_size >= NUX_GPU_COMMAND_SIZE)
    {
        return NU_NULL;
    }
    nux_gpu_command_t *cmd
        = &env->inst->gpu_commands[env->inst->gpu_commands_size];
    ++env->inst->gpu_commands_size;
    return cmd;
}
