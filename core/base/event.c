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
void
nux_event_subscribe (nux_ctx_t           *ctx,
                     nux_rid_t            event,
                     nux_event_callback_t callback)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
    nux_event_subscriber_t sub;
    sub.callback = callback;
    sub.next     = e->first_subscriber;
}
void
nux_event_unsubscribe (nux_ctx_t           *ctx,
                       nux_rid_t            event,
                       nux_event_callback_t callback)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
}
void
nux_event_post (nux_ctx_t *ctx, nux_rid_t event, nux_event_data_t data)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
}

void
nux_event_process (nux_ctx_t *ctx, nux_rid_t event)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
    nux_event_subscriber_t *sub = e->first_subscriber;
    while (sub)
    {
        // sub->callback(ctx, event, &data);
        sub = sub->next;
    }
}
