#ifndef NU_DS_H
#define NU_DS_H

#define NU_POOL(type)            \
    struct                       \
    {                            \
        type         *data;      \
        nu_size_t     capacity;  \
        nu_size_vec_t _freelist; \
    }

#define NU_POOL_INIT(cap, p) NU_POOL_INIT_A(nu_allocator_core(), cap, p)

#define NU_POOL_INIT_A(alloc, cap, p)                                    \
    do                                                                   \
    {                                                                    \
        (p)->data = NU_VOID_CAST(                                        \
            (p)->data, nu_alloc_a((alloc), sizeof(*(p)->data) * (cap))); \
        nu_memset((p)->data, 0, sizeof(*(p)->data) * (cap));             \
        (p)->capacity = (cap);                                           \
        NU_VEC_INIT_A(alloc, (cap), &(p)->_freelist);                    \
        for (nu_size_t i = 0; i < (cap); ++i)                            \
        {                                                                \
            *NU_VEC_PUSH(&(p)->_freelist) = (cap) - i - 1;               \
        }                                                                \
    } while (0)

#define NU_POOL_FREE(p)                                \
    do                                                 \
    {                                                  \
        nu_free_a((p)->_freelist.allocator,            \
                  (p)->data,                           \
                  sizeof(*(p)->data) * (p)->capacity); \
        NU_VEC_FREE(&(p)->_freelist);                  \
    } while (0)

#define NU_POOL_ADD(p, pindex)                                  \
    (((p)->data = NU_VOID_CAST((p)->data,                       \
                               nu__pool_add(sizeof(*(p)->data), \
                                            (p)->data,          \
                                            &(p)->capacity,     \
                                            &(p)->_freelist,    \
                                            (pindex))))         \
         ? (p)->data + (*(pindex))                              \
         : NU_NULL)

#define NU_POOL_REMOVE(s, index) ((*NU_VEC_PUSH(&(s)->_freelist)) = (index))

#define NU_OBJECT_POOL(type)         \
    struct                           \
    {                                \
        struct                       \
        {                            \
            nu_u8_t version;         \
            union                    \
            {                        \
                type     value;      \
                nu_u32_t free_index; \
            }                        \
        }        *data;              \
        nu_size_t capacity;          \
        nu_u32_t  free_index;        \
    }

#define NU_OBJECT_GET(pool, handle)                           \
    (NU_HANDLE_INDEX(handle) < (pool)->capacity               \
     && NU_HANDLE_VERSION(handle)                             \
            == (pool)->data[NU_HANDLE_INDEX(handle)].version) \
        ? &(pool)->data[NU_HANDLE_INDEX(handle)].value        \
        : NU_NULL;

#define NU_VEC(type)        \
    struct                  \
    {                       \
        type     *data;     \
        nu_size_t capacity; \
        nu_size_t size;     \
    }
#define NU_VEC_ALLOC(v, capa)                                                  \
    {                                                                          \
        (v)->data = (capa) ? nu_malloc(sizeof(*(v)->data) * (capa)) : NU_NULL; \
        (v)->capacity = (capa);                                                \
        (v)->size     = 0;                                                     \
    }
#define NU_VEC_INIT(v, ptr, capa) \
    {                             \
        (v)->data     = (ptr);    \
        (v)->capacity = (capa);   \
        (v)->size     = 0;        \
    }
#define NU_VEC_INIT_A(v, malloc, capa)                       \
    {                                                        \
        (v)->data     = (malloc)(sizeof(*(v)->data) * capa); \
        (v)->capacity = (capa);                              \
        (v)->size     = 0;                                   \
    }
#define NU_VEC_FREE_A(v, free, capa) \
    {                                \
        (free)((v)->data);           \
        (v)->data     = NU_NULL;     \
        (v)->capacity = 0;           \
        (v)->size     = 0;           \
    }
#define NU_VEC_PUSH(v) \
    ((v)->size >= (v)->capacity ? NU_NULL : &(v)->data[(v)->size++])
#define NU_VEC_POP(v)      (v)->size ? &(v)->data[(v)->size--] : NU_NULL
#define NU_VEC_CLEAR(v)    (v)->size = 0
#define NU_VEC_INDEX(v, p) ((p) - (v)->data)
#define NU_VEC_SWAP(v, a, b)                                                 \
    {                                                                        \
        NU_ASSERT((a) < (v)->size && (b) < (v)->size);                       \
        if ((a) != (b))                                                      \
        {                                                                    \
            nu_memswp((v)->data + (a), (v)->data + (b), sizeof(*(v)->data)); \
        }                                                                    \
    }
#define NU_VEC_SWAP_REMOVE(v, i)                  \
    {                                             \
        NU_ASSERT((i) < (v)->size);               \
        if ((i) < (v)->size - 1)                  \
        {                                         \
            NU_VEC_SWAP((v), (i), (v)->size - 1); \
        }                                         \
        NU_VEC_POP((v));                          \
    }
#define NU_VEC_SWAP_REMOVE_PTR(v, p) NU_VEC_SWAP_REMOVE(v, NU_VEC_INDEX(v, p))
#define NU_VEC_APPEND(dst, src)                                  \
    do                                                           \
    {                                                            \
        NU_ASSERT(sizeof(*(dst)->data) == sizeof(*(src)->data)); \
        nu_size_t dst_size = (dst)->size;                        \
        nu_size_t src_size = (src)->size;                        \
        nu_size_t new_size = dst_size + src_size;                \
        if (new_size <= (dst)->capacity)                         \
        {                                                        \
            (dst)->size = new_size;                              \
            nu_memcpy((dst)->data + dst_size,                    \
                      (src)->data,                               \
                      sizeof(*(src)->data) * src_size);          \
        }                                                        \
    } while (0)
#define NU_VEC_LAST(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

#define NU_ARRAY(type)  \
    struct              \
    {                   \
        type     *data; \
        nu_size_t size; \
    }
#define NU_ARRAY_ALLOC(a, ss)                                          \
    {                                                                  \
        (a)->data = ss ? nu_malloc(sizeof(*(a)->data) * ss) : NU_NULL; \
        (a)->size = ss;                                                \
    }

#define NU_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define NU_ARRAY_FILL(arr, size, value)  \
    for (nu_size_t i = 0; i < size; ++i) \
    {                                    \
        (arr)[i] = (value);              \
    }

#endif
