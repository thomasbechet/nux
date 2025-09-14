#include "internal.h"

nux_rid_t
nux_event_new (nux_ctx_t *ctx, nux_rid_t arena, nux_event_type_t type)
{
    nux_rid_t    rid;
    nux_event_t *event = nux_resource_new(ctx, arena, NUX_RESOURCE_EVENT, &rid);
    NUX_CHECK(event, return NUX_NULL);
    event->arena = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_ASSERT(event->arena);
    event->type          = type;
    event->first_handler = NUX_NULL;
    event->first_event   = NUX_NULL;
    return rid;
}
nux_event_handler_t *
nux_event_subscribe (nux_ctx_t           *ctx,
                     nux_rid_t            arena,
                     nux_rid_t            event,
                     void                *userdata,
                     nux_event_callback_t callback)
{
    nux_arena_t *a = nux_resource_check(ctx, NUX_RESOURCE_ARENA, arena);
    NUX_CHECK(a, return NUX_NULL);
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return NUX_NULL);
    nux_event_handler_t *handler = nux_arena_malloc(a, sizeof(*handler));
    handler->event               = event;
    handler->userdata            = userdata;
    handler->callback            = callback;
    handler->next                = NUX_NULL;
    handler->prev                = e->first_handler;
    if (handler->prev)
    {
        handler->prev->next = handler;
    }
    else
    {
        e->first_handler = handler;
    }
    return handler;
}
void
nux_event_unsubscribe (nux_ctx_t *ctx, const nux_event_handler_t *handler)
{
    NUX_ASSERT(handler);
    nux_event_t *e = nux_resource_get(ctx, NUX_RESOURCE_EVENT, handler->event);
    NUX_CHECK(e, return); // event has been deleted
    nux_event_handler_t *h = e->first_handler;
    while (h && h != handler)
    {
        h = h->next;
    }
    NUX_CHECK(h, return); // already unsubscribed ?
    if (h->next)
    {
        h->next->prev = h->prev;
    }
    if (h->prev)
    {
        h->prev->next = h->next;
    }
    if (e->first_handler == h)
    {
        e->first_handler = h->next;
    }
}
void
nux_event_emit (nux_ctx_t  *ctx,
                nux_rid_t   event,
                nux_u32_t   size,
                const void *data)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
    nux_arena_t *a
        = nux_resource_check(ctx, NUX_RESOURCE_ARENA, ctx->frame_arena_rid);
    NUX_CHECK(a, return);
    nux_event_header_t *header = nux_arena_malloc(a, sizeof(*header));
    NUX_CHECK(header, return);
    header->next = NUX_NULL;
    header->data = nux_arena_malloc(a, size);
    NUX_CHECK(header->data, return);
    nux_memcpy(header->data, data, size);
    if (e->first_event)
    {
        e->first_event->next = header;
    }
    else
    {
        e->first_event = header;
    }
}
void
nux_event_process (nux_ctx_t *ctx, nux_rid_t event)
{
    nux_event_t *e = nux_resource_check(ctx, NUX_RESOURCE_EVENT, event);
    NUX_CHECK(e, return);
    nux_event_header_t *header = e->first_event;
    nux_u32_t           count  = 0;
    while (header)
    {
        nux_event_handler_t *handler = e->first_handler;
        while (handler)
        {
            handler->callback(ctx, handler->userdata, event, header->data);
            handler = handler->next;
        }
        header = header->next;
        ++count;
    }
    e->first_event = NUX_NULL;
    if (count)
    {
        NUX_DEBUG("%d events processed for 0x%X", count, event);
    }
}
void
nux_event_process_all (nux_ctx_t *ctx)
{
    nux_rid_t it = NUX_NULL;
    while ((it = nux_resource_next(ctx, NUX_RESOURCE_EVENT, it)))
    {
        nux_event_process(ctx, it);
    }
}
