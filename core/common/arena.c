#include "common.h"

void
nux_arena_init (nux_arena_t               *arena,
                void                      *userdata,
                nux_arena_alloc_callback_t alloc,
                nux_arena_panic_callback_t panic)
{
    arena->userdata = userdata;
    arena->alloc    = alloc;
    arena->panic    = panic;

    arena->last_object          = nullptr;
    arena->first_header         = nullptr;
    arena->last_header          = nullptr;
    arena->head                 = nullptr;
    arena->end                  = nullptr;
    arena->info.block_size      = NUX_MEM_4M;
    arena->info.block_count     = 0;
    arena->info.memory_usage    = 0;
    arena->info.memory_waste    = 0;
    arena->info.memory_capacity = 0;
}
void
nux_arena_free (nux_arena_t *a)
{
    // clear all blocks
    nux_arena_clear(a);
    // free memory blocks
    nux_arena_header_t *block = a->first_header;
    while (block)
    {
        void     *p  = block;
        nux_u32_t bc = block->bc;
        block        = block->next;
        a->alloc(a->userdata, p, a->info.block_size * bc, 0);
    }
}
nux_arena_info_t
nux_arena_info (nux_arena_t *a)
{
    return a->info;
}
void *
nux_arena_malloc (nux_arena_t *a, nux_u32_t size)
{
    if (!size)
    {
        return nullptr;
    }
    a->info.memory_usage += size;
    // check available space in current block
    if (!a->head || (a->head + size) > a->end)
    {
        // check next block
        if (a->last_header && a->last_header->next)
        {
            // reuse blocks
            nux_arena_header_t *header = a->last_header->next;
            a->head                    = (nux_u8_t *)(header + 1);
            a->end                     = a->head + a->info.block_size;
            a->last_header             = header;
        }
        else
        {
            // allocate new block
            nux_assert(a->info.block_size);
            nux_u32_t           block_count = (size / a->info.block_size) + 1;
            nux_arena_header_t *new_header  = a->alloc(
                a->userdata, nullptr, 0, a->info.block_size * block_count);
            if (!new_header)
            {
                a->panic(a->userdata);
            }
            a->info.block_count += block_count;
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
            new_header->next = nullptr;
            a->last_header   = new_header;
            a->head          = (nux_u8_t *)(new_header + 1);
            a->end           = a->head + a->info.block_size * block_count;
        }
        nux_assert(a->head < a->end);
    }
    void *p = a->head;
    nux_memset(p, 0, size);
    a->head += size;
    return p;
}
void *
nux_arena_realloc (nux_arena_t *a, void *optr, nux_u32_t osize, nux_u32_t nsize)
{
    void *p;
    if (nsize <= osize) // shrink
    {
        a->info.memory_waste += (osize - nsize);
        p = optr; // nothing to do
    }
    else // grow
    {
        nux_assert(nsize);
        p = nux_arena_malloc(a, nsize);
        a->info.memory_waste += osize;
        nux_memset(p, 0, nsize);
        if (optr) // copy previous memory
        {
            nux_memcpy(p, optr, osize);
        }
    }
    return p;
}
void *
nux_arena_new_object (nux_arena_t          *a,
                      nux_u32_t             size,
                      nux_arena_finalizer_t finalizer)
{
    // TODO: handle proper alignment
    nux_arena_object_t *obj
        = nux_arena_malloc(a, sizeof(nux_arena_object_t) + size);
    obj->finalize  = finalizer;
    obj->prev      = a->last_object;
    a->last_object = obj;
    return obj + 1;
}
void
nux_arena_clear (nux_arena_t *a)
{
    // Finalize objects
    nux_arena_object_t *obj = a->last_object;
    while (obj)
    {
        obj->finalize(obj + 1); // TODO: handle proper alignment
        obj = obj->prev;
    }
    // Clear arena
    a->last_header       = a->first_header;
    a->head              = nullptr;
    a->end               = nullptr;
    a->info.memory_usage = 0;
    a->info.memory_waste = 0;
    a->last_object       = nullptr;
}
