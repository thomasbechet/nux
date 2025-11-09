#include "common.h"

static float
color_to_linear (float x)
{
    if (x > 0.04045)
    {
        return nux_pow((x + 0.055) / 1.055, 2.4);
    }
    else
    {
        return x / 12.92;
    }
}

nux_v4_t
nux_color_rgba (nux_u8_t r, nux_u8_t g, nux_u8_t b, nux_u8_t a)
{
    return NUX_COLOR_RGBA(r, g, b, a);
}
nux_v4_t
nux_color_hex (nux_u32_t hex)
{
    return NUX_COLOR_HEX(hex);
}
nux_u32_t
nux_color_to_hex (nux_v4_t color)
{
    return ((nux_u8_t)(color.x * 255) << 16) | ((nux_u8_t)(color.y * 255) << 8)
           | ((nux_u8_t)(color.z * 255) << 0)
           | ((nux_u8_t)(color.w * 255) << 24);
}
nux_v4_t
nux_color_to_srgb (nux_v4_t color)
{
    nux_v4_t c;
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(color.data); ++i)
    {
        c.data[i] = color_to_linear(color.data[i]);
    }
    return c;
}
