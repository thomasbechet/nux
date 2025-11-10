#include "internal.h"

nux_stylesheet_t *
nux_stylesheet_new (nux_arena_t *arena)
{
    nux_stylesheet_t *s = nux_new_resource(arena, NUX_RESOURCE_STYLESHEET);
    NUX_CHECK(s, return NUX_NULL);
    return s;
}
void
nux_stylesheet_set (nux_stylesheet_t         *style,
                    nux_stylesheet_property_t property,
                    nux_texture_t            *texture,
                    nux_b2i_t                 extent,
                    nux_b2i_t                 inner)
{
    nux_rid_t tex_rid = nux_resource_rid(texture);
    switch (property)
    {
        case NUX_STYLESHEET_BUTTON_PRESSED:
            style->button.pressed.texture = tex_rid;
            style->button.pressed.extent  = extent;
            style->button.pressed.inner   = inner;
            break;
        case NUX_STYLESHEET_BUTTON_RELEASED:
            style->button.released.texture = tex_rid;
            style->button.released.extent  = extent;
            style->button.released.inner   = inner;
            break;
        case NUX_STYLESHEET_BUTTON_HOVERED:
            style->button.hovered.texture = tex_rid;
            style->button.hovered.extent  = extent;
            style->button.hovered.inner   = inner;
            break;
        case NUX_STYLESHEET_CHECKBOX_CHECKED:
            style->checkbox.checked.texture = tex_rid;
            style->checkbox.checked.extent  = extent;
            style->checkbox.checked.inner   = inner;
            break;
        case NUX_STYLESHEET_CHECKBOX_UNCHECKED:
            style->checkbox.unchecked.texture = tex_rid;
            style->checkbox.unchecked.extent  = extent;
            style->checkbox.unchecked.inner   = inner;
            break;
        case NUX_STYLESHEET_CURSOR:
            style->cursor.image.texture = tex_rid;
            style->cursor.image.extent  = extent;
            style->cursor.image.inner   = inner;
            break;
    }
}
