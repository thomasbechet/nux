#include "internal.h"

static nux_status_t
module_init (void)
{
    // Register resources
    nux_register_resource(NUX_RESOURCE_GUI,
                          (nux_resource_info_t) {
                              .name = "gui",
                              .size = sizeof(nux_gui_t),
                          });
    nux_register_resource(
        NUX_RESOURCE_STYLESHEET,
        (nux_resource_info_t) { .name = "stylesheet",
                                     .size = sizeof(nux_stylesheet_t) });

    return NUX_SUCCESS;
}
static nux_status_t
module_pre_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_next_resource(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_begin(gui);
    }
    return NUX_SUCCESS;
}
static nux_status_t
module_post_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_next_resource(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_end(gui);
    }
    return NUX_SUCCESS;
}
void
nux_ui_module_register (void)
{
    nux_register_module((nux_module_info_t) {
        .name = "ui", .data = NUX_NULL, .size = 0, .init = module_init });
}
