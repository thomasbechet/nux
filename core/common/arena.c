#include "common.h"

void
nux_arena_init (nux_arena_t *a,
                void        *userdata,
                void *(*alloc)(void *ud, void *p, nux_u32_t o, nux_u32_t n),
                void (*clear)(void *ud),
                void (*free)(void *ud),
                void (*info)(void *ud, nux_arena_info_t *info))
{
    a->userdata = userdata;
    a->alloc    = alloc;
    a->clear    = clear;
    a->free     = free;
    a->info     = info;
    a->chain    = NUX_NULL;
}
void
nux_arena_free (nux_arena_t *a)
{
    nux_arena_clear(a);
    if (a->free)
    {
        a->free(a->userdata);
    }
}
nux_allocator_t
nux_arena_as_allocator (nux_arena_t *a)
{
    nux_allocator_t allocator;
    allocator.userdata = a->userdata;
    allocator.alloc    = a->alloc;
    return allocator;
}
nux_arena_info_t
nux_arena_info (nux_arena_t *a)
{
    nux_arena_info_t info;
    nux_memset(&info, 0, sizeof(info));
    if (a->info)
    {
        a->info(a->userdata, &info);
    }
    return info;
}
void *
nux_arena_malloc (nux_arena_t *a, nux_u32_t size)
{
    return a->alloc(a->userdata, NUX_NULL, 0, size);
}
void *
nux_arena_realloc (nux_arena_t *a, void *p, nux_u32_t osize, nux_u32_t nsize)
{
    return a->alloc(a->userdata, p, osize, nsize);
}
void *
nux_arena_new_object (nux_arena_t          *a,
                      nux_u32_t             size,
                      nux_arena_finalizer_t finalizer)
{
    // TODO: handle proper alignment
    nux_arena_object_t *obj
        = nux_arena_malloc(a, sizeof(nux_arena_object_t) + size);
    NUX_CHECK(obj, return obj);
    obj->finalize = finalizer;
    obj->prev     = a->chain;
    a->chain      = obj;
    return obj + 1;
}
void
nux_arena_clear (nux_arena_t *a)
{
    // Finalize objects
    nux_arena_object_t *obj = a->chain;
    while (obj)
    {
        obj->finalize(obj + 1); // TODO: handle proper alignment
        obj = obj->prev;
    }
    if (a->clear)
    {
        a->clear(a->userdata);
    }
    a->chain = NUX_NULL;
}

static void *
block_arena_malloc (nux_block_arena_t *a, nux_u32_t size)
{
    if (!size)
    {
        return NUX_NULL;
    }
    a->total_alloc += size;
    // check available space in current block
    if (!a->head || (a->head + size) > a->end)
    {
        // check next block
        if (a->last_header && a->last_header->next)
        {
            // reuse blocks
            nux_block_arena_header_t *header = a->last_header->next;
            a->head                          = (nux_u8_t *)(header + 1);
            a->end                           = a->head + a->block_size;
            a->last_header                   = header;
        }
        else
        {
            // allocate new block
            NUX_ASSERT(a->block_size);
            nux_u32_t                 block_count = (size / a->block_size) + 1;
            nux_block_arena_header_t *new_header
                = nux_malloc(a->allocator, a->block_size * block_count);
            NUX_CHECK(new_header, return NUX_NULL);
            new_header->bc = block_count;
            if (!a->first_header)
            {
                a->first_header = new_header;
            }
            if (a->last_header)
            {
                a->last_header->next = new_header;
            }
            new_header->prev = a->last_header;
            new_header->next = NUX_NULL;
            a->last_header   = new_header;
            a->head          = (nux_u8_t *)(new_header + 1);
            a->end           = a->head + a->block_size * block_count;
        }
        NUX_ASSERT(a->head < a->end);
    }
    void *p = a->head;
    nux_memset(p, 0, size);
    a->head += size;
    return p;
}
static void *
block_arena_realloc (nux_block_arena_t *a,
                     void              *optr,
                     nux_u32_t          osize,
                     nux_u32_t          nsize)
{
    void *p;
    if (nsize <= osize) // shrink
    {
        a->total_waste += (osize - nsize);
        p = optr; // nothing to do
    }
    else // grow
    {
        NUX_ASSERT(nsize);
        p = block_arena_malloc(a, nsize);
        NUX_CHECK(p, return NUX_NULL);
        a->total_waste += osize;
        nux_memset(p, 0, nsize);
        if (optr) // copy previous memory
        {
            nux_memcpy(p, optr, osize);
        }
    }
    return p;
}
static void *
block_arena_alloc (void *userdata, void *p, nux_u32_t osize, nux_u32_t nsize)
{
    if (p)
    {
        if (nsize)
        {
            return block_arena_realloc(userdata, p, osize, nsize);
        }
        else
        {
            // nothing to do
            return NUX_NULL;
        }
    }
    else
    {
        return block_arena_malloc(userdata, nsize);
    }
}
static void
block_arena_clear (void *userdata)
{
    nux_block_arena_t *a = userdata;
    a->last_header       = a->first_header;
    a->head              = NUX_NULL;
    a->end               = NUX_NULL;
    a->total_alloc       = 0;
    a->total_waste       = 0;
}
static void
block_arena_free (void *userdata)
{
    nux_block_arena_t *a = userdata;
    // free memory blocks
    nux_block_arena_header_t *block = a->first_header;
    while (block)
    {
        void     *p  = block;
        nux_u32_t bc = block->bc;
        block        = block->next;
        nux_free(a->allocator, p, a->block_size * bc);
    }
}
static void
block_arena_info (void *userdata, nux_arena_info_t *info)
{
    nux_block_arena_t *a = userdata;

    nux_block_arena_header_t *h = a->first_header;
    info->block_count           = 0;
    while (h)
    {
        info->block_count += h->bc;
        h = h->next;
    }
    info->memory_usage = a->total_alloc;
    info->memory_waste = a->total_waste;
}

void
nux_block_arena_init (nux_arena_t       *a,
                      nux_block_arena_t *ba,
                      nux_allocator_t   *allocator)
{
    ba->allocator    = allocator;
    ba->first_header = NUX_NULL;
    ba->last_header  = NUX_NULL;
    ba->block_size   = NUX_MEM_4M;
    ba->head         = NUX_NULL;
    ba->end          = NUX_NULL;
    ba->total_alloc  = 0;
    ba->total_waste  = 0;
    nux_arena_init(a,
                   ba,
                   block_arena_alloc,
                   block_arena_clear,
                   block_arena_free,
                   block_arena_info);
}
