#ifndef NUX_GUI_INTERNAL_H
#define NUX_GUI_INTERNAL_H

#include <base/module.h>
#include <gui/api.h>

struct nux_gui_t
{
    nux_rid_t canvas;
    nux_u32_t next_id;
    nux_u32_t active_id;
    nux_u32_t hot_id;
};

#endif
