#include "internal.h"

void
nux_palset (nux_env_t env, nux_u8_t index, nux_u32_t color)
{
    nux_u8_t *pal      = env->inst->memory + NUX_MAP_PALETTE;
    pal[index * 3 + 0] = (color & 0xFF0000) >> 16;
    pal[index * 3 + 1] = (color & 0x00FF00) >> 8;
    pal[index * 3 + 2] = (color & 0x0000FF) >> 0;
}
void
nux_clear (nux_env_t env, nux_u32_t color)
{
    nux_fill(env, 0, 0, NUX_SCREEN_WIDTH - 1, NUX_SCREEN_HEIGHT - 1, color);
}
