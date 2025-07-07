#include "internal.h"

#include "fonts_data.c.inc"

nux_status_t
nux_font_init_default (nux_ctx_t *ctx, nux_font_t *font)
{
    const nux_u32_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;

    return NUX_SUCCESS;
}
