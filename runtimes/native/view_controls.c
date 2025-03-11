#include "nuklear/src/nuklear.h"
#include "runtime.h"

void
view_controls (struct nk_context *ctx, struct nk_rect bounds)
{
    const nu_size_t row = 30;
    if (nk_begin(ctx, "Controls", bounds, NK_WINDOW_BACKGROUND))
    {
        nk_layout_row_static(ctx, row, 200, 1);
        for (nu_size_t i = 0; i < SYS_BUTTON_COUNT; ++i)
        {
            nu_char_t buf[32];
            nu_int_t  buf_len;
            nk_edit_string(ctx,
                           NK_EDIT_FIELD,
                           buf,
                           &buf_len,
                           sizeof(buf),
                           nk_filter_ascii);
        }
        for (nu_size_t i = 0; i < SYS_AXIS_COUNT; ++i)
        {
        }
    }
    nk_end(ctx);
}
