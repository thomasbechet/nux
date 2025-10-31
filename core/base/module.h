#ifndef NUX_BASE_MODULE_H
#define NUX_BASE_MODULE_H

#include <nux.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_ENSURE(check, action, format, ...)                 \
    if (!(check))                                              \
    {                                                          \
        nux_error(format " at %s", ##__VA_ARGS__, __SOURCE__); \
        action;                                                \
    }

#define NUX_DEBUG(format, ...)   nux_log(NUX_LOG_DEBUG, format, ##__VA_ARGS__)
#define NUX_INFO(format, ...)    nux_log(NUX_LOG_INFO, format, ##__VA_ARGS__)
#define NUX_WARNING(format, ...) nux_log(NUX_LOG_WARNING, format, ##__VA_ARGS__)
#define NUX_ERROR(format, ...)   nux_log(NUX_LOG_ERROR, format, ##__VA_ARGS__)

#define NUX_VEC_DEFINE(name, T)                                      \
    typedef struct                                                   \
    {                                                                \
        nux_arena_t *arena;                                          \
        nux_u32_t    size;                                           \
        nux_u32_t    capa;                                           \
        T           *data;                                           \
    } name##_t;                                                      \
    nux_status_t name##_init(nux_arena_t *a, name##_t *v);           \
    nux_status_t name##_init_capa(                                   \
        nux_arena_t *a, nux_u32_t capa, name##_t *v);                \
    T           *name##_push(name##_t *v);                           \
    nux_status_t name##_pushv(name##_t *v, T val);                   \
    nux_status_t name##_reserve(name##_t *v, nux_u32_t capa);        \
    nux_status_t name##_resize(name##_t *v, nux_u32_t size);         \
    T           *name##_pop(name##_t *v);                            \
    void         name##_clear(name##_t *v);                          \
    T           *name##_get(name##_t *v, nux_u32_t i);               \
    T           *name##_last(name##_t *v);                           \
    void         name##_swap(name##_t *v, nux_u32_t a, nux_u32_t b); \
    T           *name##_swap_pop(name##_t *v, nux_u32_t i);
#define NUX_VEC_IMPL(name, T)                                                   \
    nux_status_t name##_init(nux_arena_t *a, name##_t *v)                       \
    {                                                                           \
        return name##_init_capa(a, 0, v);                                       \
    }                                                                           \
    nux_status_t name##_init_capa(nux_arena_t *a, nux_u32_t capa, name##_t *v)  \
    {                                                                           \
        (v)->arena = a;                                                         \
        (v)->capa  = capa;                                                      \
        (v)->size  = 0;                                                         \
        (v)->data                                                               \
            = nux_arena_alloc(a, NUX_NULL, 0, sizeof(*(v)->data) * capa);       \
        if (capa && !v->data)                                                   \
            return NUX_FAILURE;                                                 \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    T *name##_push(name##_t *v)                                                 \
    {                                                                           \
        if ((v)->size >= (v)->capa)                                             \
        {                                                                       \
            NUX_CHECK(name##_reserve(v, v->capa ? v->capa * 2 : 1),             \
                      return NUX_NULL);                                         \
        }                                                                       \
        T *ret = (v)->data + (v)->size;                                         \
        ++(v)->size;                                                            \
        return ret;                                                             \
    }                                                                           \
    nux_status_t name##_pushv(name##_t *v, T val)                               \
    {                                                                           \
        T *a = name##_push(v);                                                  \
        if (a)                                                                  \
        {                                                                       \
            *a = val;                                                           \
            return NUX_SUCCESS;                                                 \
        }                                                                       \
        return NUX_FAILURE;                                                     \
    }                                                                           \
    nux_status_t name##_reserve(name##_t *v, nux_u32_t capa)                    \
    {                                                                           \
        if (capa > (v)->capa)                                                   \
        {                                                                       \
            nux_u32_t old_capa = (v)->capa;                                     \
            T        *old_data = (v)->data;                                     \
            (v)->capa          = capa;                                          \
            (v)->data          = nux_arena_alloc((v)->arena,                    \
                                        old_data,                      \
                                        sizeof(*(v)->data) * old_capa, \
                                        sizeof(*(v)->data) * capa);    \
            NUX_CHECK((v)->data, return NUX_NULL);                              \
        }                                                                       \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    nux_status_t name##_resize(name##_t *v, nux_u32_t size)                     \
    {                                                                           \
        NUX_CHECK(name##_reserve(v, size), return NUX_FAILURE);                 \
        v->size = size;                                                         \
        return NUX_SUCCESS;                                                     \
    }                                                                           \
    T *name##_pop(name##_t *v)                                                  \
    {                                                                           \
        if (!(v)->size)                                                         \
            return NUX_NULL;                                                    \
        T *ret = &(v)->data[(v)->size - 1];                                     \
        --(v)->size;                                                            \
        return ret;                                                             \
    }                                                                           \
    void name##_clear(name##_t *v)                                              \
    {                                                                           \
        (v)->size = 0;                                                          \
    }                                                                           \
    T *name##_get(name##_t *v, nux_u32_t i)                                     \
    {                                                                           \
        return i < (v)->size ? (v)->data + i : NUX_NULL;                        \
    }                                                                           \
    T *name##_last(name##_t *v)                                                 \
    {                                                                           \
        return (v)->size ? (v)->data + (v)->size - 1 : NUX_NULL;                \
    }                                                                           \
    void name##_swap(name##_t *v, nux_u32_t a, nux_u32_t b)                     \
    {                                                                           \
        NUX_CHECK(a < v->size && b < v->size && a != b, return);                \
        T temp     = v->data[a];                                                \
        v->data[a] = v->data[b];                                                \
        v->data[b] = temp;                                                      \
    }                                                                           \
    T *name##_swap_pop(name##_t *v, nux_u32_t i)                                \
    {                                                                           \
        NUX_CHECK(i < v->size, return NUX_NULL);                                \
        name##_swap(v, i, v->size - 1);                                         \
        return name##_pop(v);                                                   \
    }

#define NUX_POOL_DEFINE(name, T)                           \
    typedef struct                                         \
    {                                                      \
        nux_u32_vec_t freelist;                            \
        nux_u32_t     capa;                                \
        nux_u32_t     size;                                \
        T            *data;                                \
    } name##_t;                                            \
    nux_status_t name##_init(nux_arena_t *a, name##_t *p); \
    nux_status_t name##_init_capa(                         \
        nux_arena_t *a, nux_u32_t capa, name##_t *p);      \
    T   *name##_add(name##_t *p);                          \
    void name##_remove(name##_t *p, T *i);
#define NUX_POOL_IMPL(name, T)                                                 \
    nux_status_t name##_init(nux_arena_t *a, name##_t *p)                      \
    {                                                                          \
        return name##_init_capa(a, 0, p);                                      \
    }                                                                          \
    nux_status_t name##_init_capa(nux_arena_t *a, nux_u32_t capa, name##_t *p) \
    {                                                                          \
        p->capa = capa;                                                        \
        p->size = 0;                                                           \
        NUX_CHECK(nux_u32_vec_init_capa(a, capa, &p->freelist),                \
                  return NUX_FAILURE);                                         \
        p->data = nux_arena_alloc(a, NUX_NULL, 0, sizeof(*p->data) * capa);    \
        if (capa && !p->data)                                                  \
            return NUX_FAILURE;                                                \
        return NUX_SUCCESS;                                                    \
    }                                                                          \
    T *name##_add(name##_t *p)                                                 \
    {                                                                          \
        if (p->freelist.size)                                                  \
        {                                                                      \
            nux_u32_t free = *nux_u32_vec_pop(&p->freelist);                   \
            return &p->data[free];                                             \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            if (p->size >= p->capa)                                            \
            {                                                                  \
                nux_u32_t old_capa = (p)->capa;                                \
                T        *old_data = (p)->data;                                \
                nux_u32_t new_capa = old_capa * 2;                             \
                if (!new_capa)                                                 \
                {                                                              \
                    new_capa = 1;                                              \
                }                                                              \
                (p)->capa = new_capa;                                          \
                (p)->data = nux_arena_alloc((p)->freelist.arena,               \
                                            old_data,                          \
                                            sizeof(*(p)->data) * old_capa,     \
                                            sizeof(*(p)->data) * new_capa);    \
                NUX_CHECK((p)->data, return NUX_NULL);                         \
            }                                                                  \
            T *data = p->data + p->size;                                       \
            ++p->size;                                                         \
            return data;                                                       \
        }                                                                      \
    }                                                                          \
    void name##_remove(name##_t *p, T *i)                                      \
    {                                                                          \
        nux_u32_t index = i - p->data;                                         \
        NUX_ASSERT(index < p->size);                                           \
        nux_u32_vec_pushv(&p->freelist, index);                                \
    }

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_RESOURCE_NULL       = 0,
    NUX_RESOURCE_ARENA      = 1,
    NUX_RESOURCE_LUA_MODULE = 2,
    NUX_RESOURCE_TEXTURE    = 3,
    NUX_RESOURCE_MESH       = 4,
    NUX_RESOURCE_PALETTE    = 5,
    NUX_RESOURCE_VIEWPORT   = 6,
    NUX_RESOURCE_CANVAS     = 7,
    NUX_RESOURCE_FONT       = 8,
    NUX_RESOURCE_FILE       = 9,
    NUX_RESOURCE_DISK       = 10,
    NUX_RESOURCE_SCENE      = 11,
    NUX_RESOURCE_QUERY      = 12,
    NUX_RESOURCE_EVENT      = 13,
    NUX_RESOURCE_INPUTMAP   = 14,
    NUX_RESOURCE_GUI        = 15,
    NUX_RESOURCE_STYLESHEET = 16,

    NUX_RESOURCE_MAX = 256,
} nux_resource_base_t;

typedef struct nux_resource_finalizer
{
    nux_rid_t                      rid;
    struct nux_resource_finalizer *prev;
    struct nux_resource_finalizer *next;
} nux_resource_header_t;

typedef struct nux_arena_block
{
    struct nux_arena_block *prev;
    struct nux_arena_block *next;
} nux_arena_block_t;

struct nux_arena_t
{
    nux_resource_header_t *first_header;
    nux_resource_header_t *last_header;
    nux_arena_block_t     *first_block;
    nux_arena_block_t     *last_block;
    nux_u32_t              block_size;
    nux_u8_t              *head;
    nux_u8_t              *end;
    void                  *stack;
    nux_u32_t              total_alloc;
    nux_u32_t              total_waste;
};

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_f32_vec, nux_f32_t)
NUX_VEC_DEFINE(nux_v4_vec, nux_v4_t)
NUX_VEC_DEFINE(nux_ptr_vec, void *);
typedef nux_u32_vec_t nux_rid_vec_t;

typedef void (*nux_resource_cleanup_t)(void *data);
typedef nux_status_t (*nux_resource_reload_t)(void *data, const nux_c8_t *path);

typedef struct
{
    nux_rid_t       self; // for validity check
    nux_arena_t    *arena;
    nux_u32_t       type;
    void           *data;
    const nux_c8_t *path;
    const nux_c8_t *name;
    nux_u32_t       prev_entry_index;
    nux_u32_t       next_entry_index;
} nux_resource_entry_t;

typedef struct
{
    const nux_c8_t        *name;
    nux_u32_t              size;
    nux_resource_cleanup_t cleanup;
    nux_resource_reload_t  reload;
    nux_u32_t              first_entry_index;
    nux_u32_t              last_entry_index;
} nux_resource_type_t;

NUX_POOL_DEFINE(nux_resource_pool, nux_resource_entry_t);

typedef struct
{
    struct
    {
        nux_b32_t enable;
        nux_u32_t width;
        nux_u32_t height;
    } window;

    struct
    {
        nux_b32_t enable;
    } scene;

    struct
    {
        nux_b32_t enable;
    } physics;

    struct
    {
        nux_log_level_t level;
    } log;

    struct
    {
        nux_u32_t transforms_buffer_size;
        nux_u32_t batches_buffer_size;
        nux_u32_t vertices_buffer_size;
    } graphics;

    struct
    {
        nux_b32_t enable;
        nux_b32_t console;
    } debug;

    nux_b32_t hotreload;

} nux_config_t;

typedef enum
{
    NUX_MODULE_NO_DATA_INITIALIZATION = 1 << 0,
} nux_module_flags_t;

typedef enum
{
    NUX_MODULE_UNINITIALIZED,
    NUX_MODULE_INITIALIZED,
    NUX_MODULE_STARTED,
} nux_module_status_t;

typedef struct
{
    const nux_c8_t  *name;
    nux_u32_t        size;
    void            *data;
    nux_u32_t        flags;
    const nux_c8_t **deps;
    nux_status_t (*init)(void);
    nux_status_t (*free)(void);
    nux_status_t (*pre_update)(void);
    nux_status_t (*update)(void);
    nux_status_t (*post_update)(void);
    nux_status_t (*on_event)(nux_os_event_t *event);
} nux_module_info_t;

typedef enum
{
    NUX_EVENT_LUA = 0,
} nux_event_type_t;

typedef void (*nux_event_callback_t)(void       *userdata,
                                     nux_rid_t   event,
                                     const void *data);
typedef void (*nux_event_cleanup_t)(void *data);

typedef struct nux_event_handler
{
    nux_rid_t                 event;
    struct nux_event_handler *next;
    struct nux_event_handler *prev;
    nux_event_callback_t      callback;
    void                     *userdata;
} nux_event_handler_t;

typedef struct nux_event_header
{
    struct nux_event_header *next;
    void                    *data;
} nux_event_header_t;

typedef struct
{
    nux_event_type_t     type;
    nux_arena_t         *arena;
    nux_event_handler_t *first_handler;
    nux_event_header_t  *first_event;
    nux_event_cleanup_t  cleanup;
} nux_event_t;

NUX_VEC_DEFINE(nux_os_event_vec, nux_os_event_t);

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// ds.c

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v);

// resource.c

nux_resource_type_t *nux_resource_register(nux_u32_t       index,
                                           nux_u32_t       size,
                                           const nux_c8_t *name);
void                *nux_resource_new(nux_arena_t *arena, nux_u32_t type);
void                *nux_resource_get(nux_u32_t type, nux_rid_t rid);
void                *nux_resource_check(nux_u32_t type, nux_rid_t rid);
nux_status_t         nux_resource_reload(nux_rid_t rid);

void            nux_resource_set_path_rid(nux_rid_t rid, const nux_c8_t *path);
void            nux_resource_set_path(void *data, const nux_c8_t *path);
const nux_c8_t *nux_resource_path_rid(nux_rid_t rid);
const nux_c8_t *nux_resource_path(void *data);
void            nux_resource_set_name_rid(nux_rid_t rid, const nux_c8_t *name);
void            nux_resource_set_name(void *data, const nux_c8_t *name);
const nux_c8_t *nux_resource_name_rid(nux_rid_t rid);
const nux_c8_t *nux_resource_name(void *data);
nux_rid_t       nux_resource_next_rid(nux_u32_t type, nux_rid_t rid);
void           *nux_resource_next(nux_u32_t type, const void *p);
nux_rid_t       nux_resource_rid(const void *data);
nux_arena_t    *nux_resource_arena_rid(nux_rid_t rid);
nux_arena_t    *nux_resource_arena(void *data);
nux_rid_t       nux_resource_find_rid(const nux_c8_t *name);
void           *nux_resource_find(const nux_c8_t *name);

// arena.c

void      nux_arena_init(nux_arena_t *arena);
void      nux_arena_init_stack(nux_arena_t *arena, void *data, nux_u32_t capa);
void      nux_arena_free(nux_arena_t *arena);
void     *nux_arena_alloc(nux_arena_t *arena,
                          void        *optr,
                          nux_u32_t    osize,
                          nux_u32_t    nsize);
nux_c8_t *nux_arena_alloc_string(nux_arena_t *arena, const nux_c8_t *s);
void     *nux_arena_malloc(nux_arena_t *arena, nux_u32_t size);
void      nux_arena_cleanup(void *data);

// logger.c

void nux_vlog(nux_log_level_t level, const nux_c8_t *fmt, va_list args);
void nux_log(nux_log_level_t level, const nux_c8_t *fmt, ...);

// error.c

void            nux_error(const nux_c8_t *fmt, ...);
void            nux_error_enable(void);
void            nux_error_disable(void);
void            nux_error_reset(void);
const nux_c8_t *nux_error_get_message(void);
nux_status_t    nux_error_get_status(void);

// module.c

nux_status_t  nux_modules_register(const nux_module_info_t *module);
nux_status_t  nux_modules_init(void);
nux_status_t  nux_modules_free(void);
nux_status_t  nux_modules_pre_update(void);
nux_status_t  nux_modules_update(void);
nux_status_t  nux_modules_post_update(void);
nux_status_t  nux_modules_on_event(nux_os_event_t *event);
nux_config_t *nux_config(void);
void         *nux_userdata(void);

// event.c

nux_event_t         *nux_event_new(nux_arena_t        *arena,
                                   nux_event_type_t    type,
                                   nux_event_cleanup_t cleanup);
nux_event_handler_t *nux_event_subscribe(nux_arena_t         *arena,
                                         nux_event_t         *event,
                                         void                *userdata,
                                         nux_event_callback_t callback);
void                 nux_event_unsubscribe(const nux_event_handler_t *handler);
void nux_event_emit(nux_event_t *event, nux_u32_t size, const void *data);
void nux_event_process(nux_event_t *event);
void nux_event_process_all(void);

#endif
