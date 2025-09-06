#include "internal.h"

nux_rid_t
nux_event_new (nux_ctx_t *ctx, nux_rid_t arena, const nux_c8_t *name)
{
    nux_rid_t    rid;
    nux_event_t *event = nux_resource_new(
        ctx, arena, NUX_RESOURCE_EVENT, sizeof(*event), &rid);
    NUX_CHECK(event, return NUX_NULL);

    return rid;
}
nux_rid_t
nux_event_on_tick (nux_ctx_t *ctx)
{
}

void
nux_event_subscribe (nux_ctx_t           *ctx,
                     nux_rid_t            event,
                     nux_event_callback_t callback)
{
}
void
nux_event_unsubscribe (nux_ctx_t           *ctx,
                       nux_rid_t            event,
                       nux_event_callback_t callback)
{
}
void
nux_event_send (nux_ctx_t *ctx, nux_rid_t event, nux_event_data_t data)
{
}
