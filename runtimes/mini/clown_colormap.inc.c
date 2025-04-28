#include <nux.h>

static nux_u32_t clown_colormap[]
    = { 0xffffff, 0xffb743, 0xff004d, 0x3bdde3, 0x69788e, 0x342132 };
static void
load_clown_colormap (nux_env_t env)
{
    for (nux_u8_t i = 0; i < sizeof(clown_colormap) / sizeof(clown_colormap[0]);
         ++i)
    {
        nux_cset(env, i, clown_colormap[i]);
    }
}
