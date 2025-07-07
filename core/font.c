#include "internal.h"

#include "fonts_data.c.inc"

nux_status_t
nux_font_init_default (nux_ctx_t *ctx, nux_font_t *font)
{
    const nux_u32_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;
    const nux_u32_t glyph_count
        = (NUX_ARRAY_SIZE(default_font_data) * 8) / pixel_per_glyph;

    font->texture.width       = glyph_count * DEFAULT_FONT_DATA_WIDTH;
    font->texture.height      = DEFAULT_FONT_DATA_HEIGHT;
    font->texture.type        = NUX_TEXTURE_IMAGE_RGBA;
    font->glyph_height        = DEFAULT_FONT_DATA_HEIGHT;
    font->glyph_width         = DEFAULT_FONT_DATA_WIDTH;
    font->char_to_glyph_index = default_font_data_chars;
    NUX_CHECK(nux_gpu_texture_init(ctx, &font->texture), return NUX_FAILURE);

    // Rasterize glyphs
    for (nux_u32_t g = 0; g < glyph_count; ++g)
    {
        nux_u32_t pixels[128];
        NUX_ASSERT(NUX_ARRAY_SIZE(pixels) >= pixel_per_glyph);
        for (nux_u32_t p = 0; p < pixel_per_glyph; ++p)
        {
            nux_u32_t bit_offset = g * pixel_per_glyph + p;
            NUX_ASSERT((bit_offset / 8) < sizeof(default_font_data));
            nux_u8_t  byte    = default_font_data[bit_offset / 8];
            nux_b32_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            if (bit_set)
            {
                pixels[p] = 0xFFFFFFFF;
            }
            else
            {
                pixels[p] = 0x0;
            }

            // Write glyph to texture
            NUX_CHECK(nux_os_update_texture(ctx->userdata,
                                            font->texture.slot,
                                            g * font->glyph_width,
                                            0,
                                            font->glyph_width,
                                            font->glyph_height,
                                            pixels),
                      return NUX_FAILURE);
        }
    }

    return NUX_SUCCESS;
}
