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

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_f32_vec, nux_f32_t)
NUX_VEC_DEFINE(nux_v4_vec, nux_v4_t)
NUX_VEC_DEFINE(nux_ptr_vec, void *);
typedef nux_u32_vec_t nux_rid_vec_t;

typedef void (*nux_resource_cleanup_t)(void *data);
typedef nux_status_t (*nux_resource_reload_t)(void *data, const nux_c8_t *path);

typedef struct nux_resource_finalizer
{
    nux_rid_t rid;
} nux_resource_header_t;

typedef struct
{
    nux_rid_t       rid; // for validity check
    nux_arena_t    *arena;
    nux_u32_t       type_index;
    void           *data;
    const nux_c8_t *path;
    const nux_c8_t *name;
    nux_u32_t       prev_entry_index; // same type
    nux_u32_t       next_entry_index; // same type
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

typedef enum
{
    NUX_SYSTEM_PRE_UPDATE,
    NUX_SYSTEM_UPDATE,
    NUX_SYSTEM_POST_UPDATE
} nux_system_phase_t;

typedef struct
{
    const nux_c8_t    *name;
    nux_system_phase_t phase;
    void (*callback)(void);
} nux_module_system_t;

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       size;
    void           *data;
    nux_u32_t       flags;
    nux_status_t (*init)(void);
    nux_status_t (*free)(void);
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

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v);

nux_resource_type_t *nux_resource_register(nux_u32_t       index,
                                           nux_u32_t       size,
                                           const nux_c8_t *name);
void                *nux_resource_new(nux_arena_t *a, nux_u32_t type);
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
void           *nux_resource_next(nux_u32_t type, void *p);
nux_rid_t       nux_resource_rid(void *data);
nux_arena_t    *nux_resource_arena_rid(nux_rid_t rid);
nux_arena_t    *nux_resource_arena(void *data);
nux_rid_t       nux_resource_find_rid(const nux_c8_t *name);
void           *nux_resource_find(const nux_c8_t *name);

void nux_arena_cleanup(void *data);

void nux_vlog(nux_log_level_t level, const nux_c8_t *fmt, va_list args);
void nux_log(nux_log_level_t level, const nux_c8_t *fmt, ...);

void            nux_error(const nux_c8_t *fmt, ...);
void            nux_error_enable(void);
void            nux_error_disable(void);
void            nux_error_reset(void);
const nux_c8_t *nux_error_get_message(void);
nux_status_t    nux_error_get_status(void);

nux_status_t nux_module_register(const nux_module_info_t *module);
void         nux_module_requires(const nux_c8_t *name);
void         nux_module_init(const nux_c8_t *name);

void nux_config_set_u32(const nux_c8_t *name, nux_u32_t v);

nux_config_t *nux_config(void);
void         *nux_userdata(void);

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
