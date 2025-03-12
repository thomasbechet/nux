#include "runtime.h"

void
view_settings (struct nk_context *ctx, struct nk_rect bounds)
{
    if (nk_begin(ctx, "Settings", bounds, NK_WINDOW_BACKGROUND))
    {
    }
    nk_end(ctx);
}
