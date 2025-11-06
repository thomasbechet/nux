#include "internal.h"

nux_event_t *
nux_event_new (nux_arena_t        *arena,
               nux_event_type_t    type,
               nux_event_cleanup_t cleanup)
{
    nux_event_t *event = nux_resource_new(arena, NUX_RESOURCE_EVENT);
    NUX_CHECK(event, return NUX_NULL);
    event->arena         = arena;
    event->type          = type;
    event->first_handler = NUX_NULL;
    event->first_event   = NUX_NULL;
    event->cleanup       = cleanup;
    return event;
}
nux_event_type_t
nux_event_type (nux_event_t *event)
{
    return event->type;
}
nux_event_handler_t *
nux_event_subscribe (nux_arena_t         *arena,
                     nux_event_t         *event,
                     void                *userdata,
                     nux_event_callback_t callback)
{
    nux_event_handler_t *handler = nux_arena_malloc(arena, sizeof(*handler));
    handler->event               = nux_resource_rid(event);
    handler->userdata            = userdata;
    handler->callback            = callback;
    handler->next                = NUX_NULL;
    handler->prev                = event->first_handler;
    if (handler->prev)
    {
        handler->prev->next = handler;
    }
    else
    {
        event->first_handler = handler;
    }
    return handler;
}
void
nux_event_unsubscribe (const nux_event_handler_t *handler)
{
    NUX_ASSERT(handler);
    nux_event_t *e = nux_resource_get(NUX_RESOURCE_EVENT, handler->event);
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
nux_rid_t
nux_event_handler_event (nux_event_handler_t *handler)
{
    return handler->event;
}
void
nux_event_emit (nux_event_t *event, nux_u32_t size, const void *data)
{
    nux_arena_t        *a      = nux_arena_frame();
    nux_event_header_t *header = nux_arena_malloc(a, sizeof(*header));
    NUX_CHECK(header, return);
    header->next = NUX_NULL;
    header->data = nux_arena_malloc(a, size);
    NUX_CHECK(header->data, return);
    nux_memcpy(header->data, data, size);
    if (event->first_event)
    {
        event->first_event->next = header;
    }
    else
    {
        event->first_event = header;
    }
}
void
nux_event_process (nux_event_t *event)
{
    nux_event_header_t *header = event->first_event;
    nux_u32_t           count  = 0;
    while (header)
    {
        nux_event_handler_t *handler = event->first_handler;
        while (handler)
        {
            handler->callback(
                handler->userdata, nux_resource_rid(event), header->data);
            handler = handler->next;
        }
        if (event->cleanup)
        {
            event->cleanup(header->data);
        }
        header = header->next;
        ++count;
    }
    event->first_event = NUX_NULL;
    if (count)
    {
        NUX_DEBUG("%d events processed for 0x%X", count, event);
    }
}
void
nux_event_process_all (void)
{
    nux_event_t *it = NUX_NULL;
    while ((it = nux_resource_next(NUX_RESOURCE_EVENT, it)))
    {
        nux_event_process(it);
    }
}
