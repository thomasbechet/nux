#include "internal.h"

#include "fonts_data.c.inc"

void
nux_pal (nux_env_t env, nux_u8_t index, nux_u32_t color)
{
    nux_u8_t *pal      = NUX_MEMPTR(env->inst, NUX_RAM_PALETTE, nux_u8_t);
    pal[index * 3 + 0] = (color & 0xFF0000) >> 16;
    pal[index * 3 + 1] = (color & 0x00FF00) >> 8;
    pal[index * 3 + 2] = (color & 0x0000FF) >> 0;
}
void
nux_cls (nux_env_t env, nux_u32_t color)
{
    nux_fill(env, 0, 0, NUX_SCREEN_WIDTH - 1, NUX_SCREEN_HEIGHT - 1, color);
}
void
nux_clsz (nux_env_t env)
{
    nux_f32_t *z = NUX_MEMPTR(env->inst, NUX_RAM_ZBUFFER, nux_f32_t);
    for (nux_u32_t p = 0; p < NUX_SCREEN_WIDTH * NUX_SCREEN_HEIGHT; ++p)
    {
        z[p] = NU_FLT_MAX;
    }
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
