#include "runtime.h"

static struct
{
    nu_char_t path[NU_PATH_MAX];
} app;

static void
init (void)
{
    runtime_init_instance(0, nu_sv_cstr(app.path));
}
static void
update (struct nk_context *ctx)
{
    {
        nu_v2u_t size = window_get_size();

        const nu_f32_t ratio = 0.15;

        // Inspector
        {
            struct nk_rect bounds = nk_rect(0, 0, size.x * ratio, size.y);
            if (nk_begin(ctx, "Inspector", bounds, NK_WINDOW_TITLE))
            {
                nk_layout_row_static(ctx, 30, 80, 1);
                nk_button_label(ctx, "Button");
                nk_end(ctx);
            }
        }

        // Main view
        {
            nu_b2i_t viewport
                = nu_b2i_xywh(size.x * ratio, 0, size.x * (1 - ratio), size.y);
            runtime_set_instance_viewport(0, viewport);
        }
    }
}

runtime_app_t
runtime_app_default (nu_sv_t path)
{
    nu_sv_to_cstr(path, app.path, sizeof(app.path));
    return (runtime_app_t) { .init = init, .update = update };
}
