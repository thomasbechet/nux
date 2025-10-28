#include "internal.h"

// void
// nux_gui_register (void)
// {
//     nux_module_t module;
//     module.name   = "name";
//     module.size   = sizeof(nux_gui_t);
//     module.load   = nux_gui_init;
//     module.unload = nux_gui_free;
//     nux_module_register_at(NUX_MODULE_GUI, &module);
// }

nux_status_t
nux_gui_init (void)
{
    // Register resources
    nux_resource_type_t *type;
    type = nux_resource_register(NUX_RESOURCE_GUI, sizeof(nux_gui_t), "gui");
    type = nux_resource_register(
        NUX_RESOURCE_STYLESHEET, sizeof(nux_stylesheet_t), "stylesheet");

    return NUX_SUCCESS;
}
void
nux_gui_free (void)
{
}
void
nux_gui_pre_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_resource_nextp(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_begin(gui);
    }
}
void
nux_gui_post_update (void)
{
    nux_gui_t *gui = NUX_NULL;
    while ((gui = nux_resource_nextp(NUX_RESOURCE_GUI, gui)))
    {
        nux_gui_end(gui);
    }
}
