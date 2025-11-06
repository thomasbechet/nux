#include "internal.h"

static nux_status_t
module_init (void)
{
    // Register resources
    nux_resource_register(NUX_RESOURCE_GUI,
                          (nux_resource_type_info_t) {
                              .name = "gui",
                              .size = sizeof(nux_gui_t),
                          });
    nux_resource_register(
        NUX_RESOURCE_STYLESHEET,
        (nux_resource_type_info_t) { .name = "stylesheet",
                                     .size = sizeof(nux_stylesheet_t) });

    return NUX_SUCCESS;
}
static nux_status_t
module_pre_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_resource_next(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_begin(gui);
    }
    return NUX_SUCCESS;
}
static nux_status_t
module_post_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_resource_next(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_end(gui);
    }
    return NUX_SUCCESS;
}
void
nux_gui_module_register (void)
{
    nux_module_begin("gui", NUX_NULL, 0);
    nux_module_on_init(module_init);
    nux_module_end();
}
