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
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_text(env, x, y, buf, c);
}
void
nux_printfmt (nux_env_t env, nux_u8_t c, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_print(env, buf, c);
}
void
nux_tracefmt (nux_env_t env, const nux_c8_t *fmt, ...)
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
    nux_u32_t *view = NUX_MEMPTR(env->inst, NUX_RAM_TEXTURE_VIEW, nux_u32_t);
    view[0]         = x;
    view[1]         = y;
    view[2]         = w;
    view[3]         = h;
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
    x = NUX_MIN(x, NUX_TEXTURE_WIDTH - 1);
    y = NUX_MIN(y, NUX_TEXTURE_HEIGHT - 1);
    w = NUX_MIN(w, NUX_TEXTURE_WIDTH - x - 1);
    h = NUX_MIN(h, NUX_TEXTURE_HEIGHT - y - 1);

    // Copy row by row
    nux_u8_t *tex = NUX_MEMPTR(env->inst, NUX_RAM_TEXTURE, nux_u8_t);
    for (nux_u32_t i = 0; i < h; ++i)
    {
        nux_u8_t       *dst = tex + ((y + i) * NUX_TEXTURE_WIDTH + x);
        const nux_u8_t *src = data + (i * w);
        nux_memcpy(dst, src, w);
    }
}
