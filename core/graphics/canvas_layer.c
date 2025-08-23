#include <nux_internal.h>

void
nux_canvaslayer_add (nux_ctx_t *ctx, nux_ent_t e)
{
    nux_canvaslayer_t *c = nux_ecs_set(ctx, e, NUX_COMPONENT_CANVASLAYER);
    NUX_CHECK(c, return);
    c->canvas = NUX_NULL;
}
void
nux_canvaslayer_remove (nux_ctx_t *ctx, nux_ent_t e)
{
    nux_ecs_unset(ctx, e, NUX_COMPONENT_CANVASLAYER);
}
void
nux_canvaslayer_set_canvas (nux_ctx_t *ctx, nux_ent_t e, nux_res_t canvas)
{
    nux_canvaslayer_t *c = nux_ecs_get(ctx, e, NUX_COMPONENT_CANVASLAYER);
    NUX_CHECK(c, return);
    NUX_CHECK(nux_res_check(ctx, NUX_RES_CANVAS, canvas), return);
    c->canvas = canvas;
}
nux_res_t
nux_canvaslayer_get_canvas (nux_ctx_t *ctx, nux_ent_t e, nux_res_t canvas)
{
    nux_canvaslayer_t *c = nux_ecs_get(ctx, e, NUX_COMPONENT_CANVASLAYER);
    NUX_CHECK(c, return NUX_NULL);
    return c->canvas;
}
