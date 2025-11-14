#include "internal.h"

static void
module_pre_update (void)
{
    nux_gui_t *gui = nullptr;
    while ((gui = nux_resource_next(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_begin(gui);
    }
}
static void
module_post_update (void)
{
    nux_gui_t *gui = nullptr;
    while ((gui = nux_resource_next(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_end(gui);
    }
}
static nux_status_t
module_init (void)
{
    // Register systems
    nux_system_register(NUX_SYSTEM_PRE_UPDATE, module_pre_update);
    nux_system_register(NUX_SYSTEM_POST_UPDATE, module_post_update);

    // Register resources
    nux_resource_register(NUX_RESOURCE_GUI,
                          (nux_resource_info_t) {
                              .name = "gui",
                              .size = sizeof(nux_gui_t),
                          });
    nux_resource_register(
        NUX_RESOURCE_STYLESHEET,
        (nux_resource_info_t) { .name = "stylesheet",
                                .size = sizeof(nux_stylesheet_t) });

    return NUX_SUCCESS;
}
void
nux_ui_module_register (void)
{
    nux_module_register((nux_module_info_t) {
        .name = "ui", .data = nullptr, .size = 0, .init = module_init });
}
