#include "stdlib.h"

void *
nux_malloc (nux_allocator_t *a, nux_u32_t size)
{
    return a->alloc(a->userdata, NUX_NULL, 0, size);
}
void *
nux_realloc (nux_allocator_t *a, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    return a->alloc(a->userdata, optr, osize, nsize);
}
void
nux_free (nux_allocator_t *a, void *p, nux_u32_t osize)
{
    a->alloc(a->userdata, p, osize, 0);
}

static void *
block_allocator_malloc (nux_block_allocator_t *a, nux_u32_t size)
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
            nux_block_allocator_header_t *header = a->last_header->next;
            a->head                              = (nux_u8_t *)(header + 1);
            a->end                               = a->head + a->block_size;
            a->last_header                       = header;
        }
        else
        {
            // allocate new block
            NUX_ASSERT(a->block_size);
            nux_u32_t block_count = (size / a->block_size) + 1;
            nux_block_allocator_header_t *new_header
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
block_allocator_realloc (nux_block_allocator_t *a,
                         void                  *optr,
                         nux_u32_t              osize,
                         nux_u32_t              nsize)
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
        p = block_allocator_malloc(a, nsize);
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
block_allocator_alloc (void     *userdata,
                       void     *p,
                       nux_u32_t osize,
                       nux_u32_t nsize)
{
    if (p)
    {
        if (nsize)
        {
            return block_allocator_realloc(userdata, p, osize, nsize);
        }
        else
        {
            // nothing to do
            return NUX_NULL;
        }
    }
    else
    {
        return block_allocator_malloc(userdata, nsize);
    }
}

void
nux_block_allocator_init (nux_block_allocator_t *a, nux_allocator_t *allocator)
{
    a->allocator       = allocator;
    a->first_header    = NUX_NULL;
    a->last_header     = NUX_NULL;
    a->block_size      = NUX_MEM_4M;
    a->head            = NUX_NULL;
    a->end             = NUX_NULL;
    a->total_alloc     = 0;
    a->total_waste     = 0;
    a->vtable.userdata = a;
    a->vtable.alloc    = block_allocator_alloc;
}
void
nux_block_allocator_free (nux_block_allocator_t *a)
{
    // free memory blocks
    nux_block_allocator_header_t *block = a->first_header;
    while (block)
    {
        void     *p  = block;
        nux_u32_t bc = block->bc;
        block        = block->next;
        nux_free(a->allocator, p, a->block_size * bc);
    }
}
nux_allocator_t *
nux_block_allocator_interface (nux_block_allocator_t *a)
{
    return &a->vtable;
}
void
nux_block_allocator_clear (nux_block_allocator_t *a)
{
    a->last_header = a->first_header;
    a->head        = NUX_NULL;
    a->end         = NUX_NULL;
    a->total_alloc = 0;
    a->total_waste = 0;
}
nux_u32_t
nux_block_allocator_block_count (const nux_block_allocator_t *a)
{
    nux_block_allocator_header_t *h  = a->first_header;
    nux_u32_t                     bc = 0;
    while (h)
    {
        h = h->next;
        bc += h->bc;
    }
    return bc;
}
nux_u32_t
nux_block_allocator_memory_usage (const nux_block_allocator_t *a)
{
    return a->total_alloc;
}
nux_u32_t
nux_block_allocator_memory_capacity (const nux_block_allocator_t *a)
{
    return 0;
}
