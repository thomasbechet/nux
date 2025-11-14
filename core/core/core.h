#ifndef NUX_CORE_H
#define NUX_CORE_H

#include <common/common.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_ENSURE(check, action, format, ...)                        \
    if (!(check))                                                     \
    {                                                                 \
        nux_error_report(format " at %s", ##__VA_ARGS__, __SOURCE__); \
        action;                                                       \
    }
#define nux_checkn(check) nux_check(check, return nullptr)
#define nux_checkf(check) nux_check(check, return NUX_FAILURE)

#define NUX_DEBUG(format, ...) \
    nux_logger_log(NUX_LOG_DEBUG, format, ##__VA_ARGS__)
#define NUX_INFO(format, ...) \
    nux_logger_log(NUX_LOG_INFO, format, ##__VA_ARGS__)
#define NUX_WARNING(format, ...) \
    nux_logger_log(NUX_LOG_WARNING, format, ##__VA_ARGS__)
#define NUX_ERROR(format, ...) \
    nux_logger_log(NUX_LOG_ERROR, format, ##__VA_ARGS__)

#define NUX_MODULE_REGISTER(mname, mdata, minit, mfree)               \
    nux_module_register((nux_module_info_t) { .name = mname,          \
                                              .data = mdata,          \
                                              .size = sizeof(*mdata), \
                                              .init = minit,          \
                                              .free = mfree })

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef nux_u32_t nux_rid_t;
typedef nux_u32_t nux_nid_t;

typedef enum
{
    NUX_ERROR_NONE                 = 0,
    NUX_ERROR_OUT_OF_MEMORY        = 1,
    NUX_ERROR_INVALID_TEXTURE_SIZE = 4,
    NUX_ERROR_WASM_RUNTIME         = 8,
    NUX_ERROR_CART_EOF             = 10,
    NUX_ERROR_CART_MOUNT           = 11,
} nux_error_t;

typedef enum
{
    NUX_STAT_FPS           = 0,
    NUX_STAT_SCREEN_WIDTH  = 1,
    NUX_STAT_SCREEN_HEIGHT = 2,
    NUX_STAT_TIMESTAMP     = 3,
    NUX_STAT_MAX           = 4
} nux_stat_t;

typedef enum
{
    NUX_LOG_DEBUG   = 4,
    NUX_LOG_INFO    = 3,
    NUX_LOG_WARNING = 2,
    NUX_LOG_ERROR   = 1,
} nux_log_level_t;

typedef enum
{
    NUX_RESOURCE_NULL       = 0,
    NUX_RESOURCE_ANY        = 1,
    NUX_RESOURCE_ARENA      = 2,
    NUX_RESOURCE_LUA_MODULE = 3,
    NUX_RESOURCE_TEXTURE    = 4,
    NUX_RESOURCE_MESH       = 5,
    NUX_RESOURCE_PALETTE    = 6,
    NUX_RESOURCE_VIEWPORT   = 7,
    NUX_RESOURCE_CANVAS     = 8,
    NUX_RESOURCE_FONT       = 9,
    NUX_RESOURCE_FILE       = 10,
    NUX_RESOURCE_DISK       = 11,
    NUX_RESOURCE_SCENE      = 12,
    NUX_RESOURCE_QUERY      = 13,
    NUX_RESOURCE_EVENT      = 14,
    NUX_RESOURCE_INPUTMAP   = 15,
    NUX_RESOURCE_GUI        = 16,
    NUX_RESOURCE_STYLESHEET = 17,
    NUX_RESOURCE_WORLD      = 18,

    NUX_RESOURCE_MAX = 256,
} nux_resource_base_t;

typedef enum
{
    NUX_MODULE_NO_DATA_INITIALIZATION = 1 << 0,
} nux_module_flags_t;

typedef enum
{
    NUX_EVENT_LUA = 0,
} nux_event_type_t;

typedef enum
{
    NUX_NAME_MAX = 64,
} nux_base_constants_t;

typedef void (*nux_event_callback_t)(void       *userdata,
                                     nux_rid_t   event,
                                     const void *data);
typedef void (*nux_event_cleanup_t)(void *data);

typedef struct nux_event_t         nux_event_t;
typedef struct nux_file_t          nux_file_t;
typedef struct nux_event_handler_t nux_event_handler_t;

typedef void (*nux_resource_cleanup_t)(void *data);
typedef nux_status_t (*nux_resource_reload_t)(void *data, const nux_c8_t *path);

typedef nux_status_t (*nux_module_init_callback_t)(void);
typedef void (*nux_module_free_callback_t)(void);

typedef struct
{
    const nux_c8_t        *name;
    nux_u32_t              size;
    nux_resource_cleanup_t cleanup;
    nux_resource_reload_t  reload;
} nux_resource_info_t;

typedef struct
{
    const nux_c8_t            *name;
    nux_u32_t                  size;
    void                      *data;
    nux_module_init_callback_t init;
    nux_module_free_callback_t free;
} nux_module_info_t;

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
    NUX_SYSTEM_PRE_UPDATE  = 0,
    NUX_SYSTEM_UPDATE      = 1,
    NUX_SYSTEM_POST_UPDATE = 2
} nux_system_phase_t;

typedef void (*nux_system_callback_t)(void);

typedef enum
{
    NUX_FILE_MAX = 64,
    NUX_DISK_MAX = 4,
} nux_io_constants_t;

typedef enum
{
    NUX_IO_READ       = 0,
    NUX_IO_READ_WRITE = 1,
} nux_io_mode_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

nux_u32_t nux_stat_get(nux_stat_t info);
nux_f32_t nux_time_elapsed();
nux_f32_t nux_time_delta();
nux_u32_t nux_time_frame();
nux_u64_t nux_time_epoch();

nux_u32_t nux_random_next();
nux_f32_t nux_random_nextf();

nux_arena_t *nux_arena_new(nux_arena_t *arena);
void         nux_arena_clear(nux_arena_t *arena);
nux_u32_t    nux_arena_block_count(nux_arena_t *arena);
nux_u32_t    nux_arena_memory_usage(nux_arena_t *arena);

nux_arena_t *nux_arena_core(void);
nux_arena_t *nux_arena_frame(void);

void            nux_logger_set_level(nux_log_level_t level);
nux_log_level_t nux_logger_level(void);

void         nux_resource_register(nux_u32_t index, nux_resource_info_t info);
void        *nux_resource_new(nux_arena_t *a, nux_u32_t type);
void        *nux_resource_get(nux_u32_t type, nux_rid_t rid);
void        *nux_resource_check(nux_u32_t type, nux_rid_t rid);
nux_status_t nux_resource_reload(nux_rid_t rid);

void            nux_resource_set_path(void *data, const nux_c8_t *path);
const nux_c8_t *nux_resource_path(void *data);
void            nux_resource_set_name(void *data, const nux_c8_t *name);
const nux_c8_t *nux_resource_name(void *data);
void           *nux_resource_next(nux_u32_t type, void *p);
nux_rid_t       nux_resource_rid(const void *data);
nux_arena_t    *nux_resource_arena(void *data);
void           *nux_resource_find(const nux_c8_t *name);

void nux_logger_vlog(nux_log_level_t level, const nux_c8_t *fmt, va_list args);
void nux_logger_log(nux_log_level_t level, const nux_c8_t *fmt, ...);

void            nux_error_report(const nux_c8_t *fmt, ...);
void            nux_error_enable(void);
void            nux_error_disable(void);
void            nux_error_reset(void);
const nux_c8_t *nux_error_message(void);
nux_status_t    nux_error_status(void);

void         nux_module_register(nux_module_info_t info);
nux_status_t nux_module_requires(const nux_c8_t *name);

nux_config_t *nux_config_get(void);

nux_event_t         *nux_event_new(nux_arena_t        *arena,
                                   nux_event_type_t    type,
                                   nux_event_cleanup_t cleanup);
nux_event_type_t     nux_event_type(nux_event_t *event);
nux_event_handler_t *nux_event_subscribe(nux_arena_t         *arena,
                                         nux_event_t         *event,
                                         void                *userdata,
                                         nux_event_callback_t callback);
void                 nux_event_unsubscribe(const nux_event_handler_t *handler);
nux_rid_t            nux_event_handler_event(nux_event_handler_t *handler);
void nux_event_emit(nux_event_t *event, nux_u32_t size, const void *data);
void nux_event_process(nux_event_t *event);
void nux_event_process_all(void);

void nux_system_register(nux_system_phase_t    phase,
                         nux_system_callback_t callback);

nux_status_t nux_io_cart_begin(const nux_c8_t *path, nux_u32_t entry_count);
nux_status_t nux_io_cart_end(void);
nux_status_t nux_io_write_cart_file(const nux_c8_t *path);

nux_status_t nux_mount_disk(const nux_c8_t *path);

nux_b32_t    nux_file_exists(const nux_c8_t *path);
nux_file_t  *nux_file_open(nux_arena_t    *arena,
                           const nux_c8_t *path,
                           nux_io_mode_t   mode);
void         nux_file_close(nux_file_t *file);
nux_u32_t    nux_file_read(nux_file_t *file, void *data, nux_u32_t n);
nux_u32_t    nux_file_write(nux_file_t *file, const void *data, nux_u32_t n);
nux_status_t nux_file_seek(nux_file_t *file, nux_u32_t cursor);
nux_u32_t    nux_file_size(nux_file_t *file);
void *nux_file_load(nux_arena_t *a, const nux_c8_t *path, nux_u32_t *size);

#endif
