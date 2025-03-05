#include "runtime.h"

static struct
{
    nu_char_t       path[NU_PATH_MAX];
    viewport_mode_t viewport_mode;
    union
    {
        nu_f32_t edit_float[32];
    };
    nu_f32_t  edit_step[32];
    nu_f32_t  edit_step_pixel[32];
    nu_bool_t edit_override[32];
} app;

static void
init (void)
{
    app.viewport_mode = VIEWPORT_FIXED_BEST_FIT;
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(app.edit_override); ++i)
    {
        app.edit_override[i]   = NU_FALSE;
        app.edit_float[i]      = 0;
        app.edit_step[i]       = 1;
        app.edit_step_pixel[i] = 0.5;
    }
    runtime_init_instance(0, nu_sv_cstr(app.path));
}
static void
update (struct nk_context *ctx)
{
    {
        nu_v2u_t size = window_get_size();

        const nu_u32_t       width         = 300;
        const nu_u32_t       row           = 25;
        const struct nk_rect window_bounds = nk_rect(0, 0, width, size.y);

        // // Inspector
        // {
        //     if (nk_begin(ctx, "Inspector", window_bounds, NK_WINDOW_TITLE))
        //     {
        //         nk_layout_row_dynamic(ctx, row, 1);
        //         // Viewport mode
        //         {
        //             const nu_char_t *modes[] = { "FIXED",
        //                                          "FIXED BEST FIT",
        //                                          "STRETCH KEEP ASPECT",
        //                                          "STRETCH" };
        //             app.viewport_mode        = nk_combo(ctx,
        //                                          modes,
        //                                          NK_LEN(modes),
        //                                          app.viewport_mode,
        //                                          row,
        //                                          nk_vec2(200, 200));
        //         }
        //         // Inspect values
        //         {
        //             nu_size_t        count;
        //             inspect_value_t *values = runtime_inspect_values(0,
        //             &count); for (nu_size_t i = 0; i < count; ++i)
        //             {
        //                 inspect_value_t *value = values + i;
        //                 if (nk_tree_push_hashed(ctx,
        //                                         NK_TREE_TAB,
        //                                         value->name,
        //                                         NK_MINIMIZED,
        //                                         value->name,
        //                                         nu_strlen(value->name),
        //                                         0))
        //                 {
        //                     nk_layout_row_dynamic(ctx, row, 2);
        //                     nk_labelf(ctx, NK_TEXT_LEFT, "Address");
        //                     nk_labelf(ctx, NK_TEXT_LEFT, "0x%X",
        //                     value->addr); switch (value->type)
        //                     {
        //                         case SYS_INSPECT_I32:
        //                             nk_labelf(ctx,
        //                                       NK_TEXT_LEFT,
        //                                       "Value: %d",
        //                                       value->value.i32);
        //                             break;
        //                         case SYS_INSPECT_F32: {
        //                             nk_labelf(ctx, NK_TEXT_LEFT, "Value");
        //                             nk_labelf(ctx,
        //                                       NK_TEXT_LEFT,
        //                                       "%lf",
        //                                       value->value.f32);
        //                             nk_layout_row_template_begin(ctx, row);
        //                             nk_layout_row_template_push_dynamic(ctx);
        //                             nk_layout_row_template_push_static(ctx,
        //                                                                row);
        //                             nk_layout_row_template_push_static(ctx,
        //                                                                row);
        //                             nk_layout_row_template_end(ctx);
        //                             nk_property_float(ctx,
        //                                               value->name,
        //                                               NU_FLT_MIN,
        //                                               &app.edit_float[i],
        //                                               NU_FLT_MAX,
        //                                               app.edit_step[i],
        //                                               app.edit_step_pixel[i]);
        //                             nk_button_label(ctx, "+");
        //                             nk_button_label(ctx, "-");
        //                         }
        //                         break;
        //                     }
        //                     nk_layout_row_dynamic(ctx, row, 3);
        //                     if (nk_button_label(ctx, "Read"))
        //                     {
        //                         app.edit_float[i] = value->value.f32;
        //                     }
        //                     nu_bool_t override_button
        //                         = nk_button_label(ctx, "Write");
        //                     nk_checkbox_label(
        //                         ctx, "Override", &app.edit_override[i]);
        //                     if (app.edit_override[i] || override_button)
        //                     {
        //                         value->value.f32 = app.edit_float[i];
        //                         value->override  = NU_TRUE;
        //                     }
        //
        //                     nk_tree_pop(ctx);
        //                 }
        //             }
        //         }
        //         nk_end(ctx);
        //     }
        // }

        // Tabs
        {
            const struct nk_rect window_bounds = nk_rect(0, 0, size.x, size.y);
            if (nk_begin(ctx, "Tabs", window_bounds, NK_WINDOW_NO_SCROLLBAR))
            {
                nk_layout_row_static(ctx, row, 100, 2);
                if (nk_menu_begin_label(
                        ctx, "Cartridge", NK_TEXT_LEFT, nk_vec2(120, 200)))
                {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT))
                    {
                    }
                    if (nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT))
                    {
                    }
                    nk_menu_end(ctx);
                }
                if (nk_menu_begin_label(
                        ctx, "Project", NK_TEXT_LEFT, nk_vec2(120, 100)))
                {
                    nk_menu_end(ctx);
                }
                nk_end(ctx);
            }
        }

        // Main view
        {
            // nu_b2i_t viewport = nu_b2i_xywh(width, 0, (size.x - width),
            // size.y); runtime_set_instance_viewport(0, viewport,
            // app.viewport_mode);
        }
    }
}

runtime_app_t
runtime_app_default (nu_sv_t path)
{
    nu_sv_to_cstr(path, app.path, sizeof(app.path));
    return (runtime_app_t) { .init = init, .update = update };
}
