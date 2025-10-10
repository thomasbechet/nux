#include "internal.h"

static nux_v4_t default_palette[]
    = { { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 1, 0, 0, 1 },
        { 0, 1, 0, 1 }, { 0, 0, 1, 1 }, NUX_COLOR_HEX(0x99ccff) };

nux_status_t
nux_palette_register_default (void)
{
    nux_graphics_module_t *module = nux_graphics_module();
    module->default_palette
        = nux_palette_new(nux_arena_core(), NUX_ARRAY_SIZE(default_palette));
    NUX_CHECK(module->default_palette, return NUX_FAILURE);
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(default_palette); ++i)
    {
        nux_palette_set_color(module->default_palette, i, default_palette[i]);
    }
    nux_palette_set_active(module->default_palette);
    return NUX_SUCCESS;
}

nux_palette_t *
nux_palette_new (nux_arena_t *arena, nux_u32_t size)
{
    nux_palette_t *pal = nux_resource_new(arena, NUX_RESOURCE_PALETTE);
    NUX_CHECK(pal, return NUX_NULL);
    pal->size   = size;
    pal->colors = nux_arena_malloc(arena, sizeof(*pal->colors) * size);
    NUX_CHECK(pal->colors, return NUX_NULL);
    return pal;
}
nux_palette_t *
nux_palette_default (void)
{
    return nux_graphics_module()->default_palette;
}
void
nux_palette_set_active (nux_palette_t *palette)
{
    nux_graphics_module_t *module = nux_graphics_module();
    module->active_palette        = palette;
}
void
nux_palette_set_color (nux_palette_t *palette, nux_u32_t index, nux_v4_t color)
{
    NUX_CHECK(index < palette->size, return);
    palette->colors[index] = color;
}
nux_v4_t
nux_palette_get_color (const nux_palette_t *palette, nux_u32_t index)
{
    NUX_CHECK(palette, return nux_color_rgba(255, 255, 255, 255));
    NUX_CHECK(index < palette->size, return NUX_V4_ZEROS);
    return palette->colors[index];
}
