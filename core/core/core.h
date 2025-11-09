#ifndef NUX_CORE_H
#define NUX_CORE_H

#include <stdlib/stdlib.h>

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

#define NUX_REGISTER_MODULE(mname, mdata, minit, mfree)               \
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

NUX_VEC_DEFINE(nux_u32_vec, nux_u32_t)
NUX_VEC_DEFINE(nux_f32_vec, nux_f32_t)
NUX_VEC_DEFINE(nux_v4_vec, nux_v4_t)
NUX_VEC_DEFINE(nux_ptr_vec, void *);
typedef nux_u32_vec_t nux_rid_vec_t;

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
typedef struct nux_event_handler_t nux_event_handler_t;

typedef void (*nux_resource_cleanup_t)(void *data);
typedef nux_status_t (*nux_resource_reload_t)(void *data, const nux_c8_t *path);

typedef struct
{
    const nux_c8_t        *name;
    nux_u32_t              size;
    nux_resource_cleanup_t cleanup;
    nux_resource_reload_t  reload;
} nux_resource_type_info_t;

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       size;
    void           *data;
    nux_status_t (*init)(void);
    void (*free)(void);
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
    NUX_OS_EVENT_INPUT,
} nux_os_event_type_t;

typedef enum
{
    NUX_SYSTEM_PRE_UPDATE,
    NUX_SYSTEM_UPDATE,
    NUX_SYSTEM_POST_UPDATE
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

typedef struct nux_file_t nux_file_t;

typedef struct
{
    nux_u32_t size;
} nux_file_stat_t;

typedef enum
{
    NUX_SERDE_OBJECT,
    NUX_SERDE_ARRAY,
    NUX_SERDE_END,
    NUX_SERDE_U32,
    NUX_SERDE_F32,
    NUX_SERDE_V3,
    NUX_SERDE_Q4,
    NUX_SERDE_BYTES,
    NUX_SERDE_STRING,
    NUX_SERDE_RID,
    NUX_SERDE_NID,
} nux_serde_type_t;

typedef struct
{
    const nux_c8_t  *key;
    nux_serde_type_t type;
    union
    {
        nux_u32_t *u32;
        nux_f32_t *f32;
        nux_v3_t  *v3;
        nux_q4_t  *q4;
        struct
        {
            const nux_u8_t **data;
            nux_u32_t        n;
        } bytes;
        struct
        {
            const nux_c8_t **s;
            nux_u32_t       *n;
        } str;
        nux_u32_t *size;
    };
} nux_serde_value_t;

struct nux_serde_writer;
struct nux_serde_reader;

typedef nux_status_t (*nux_serde_writer_callback_t)(void *userdata,
                                                    const nux_serde_value_t *v);
typedef nux_status_t (*nux_serde_reader_callback_t)(void              *userdata,
                                                    nux_serde_value_t *v);

typedef struct nux_serde_writer
{
    void                       *userdata;
    nux_serde_writer_callback_t callback;
    nux_status_t                status;
} nux_serde_writer_t;

typedef struct nux_serde_reader
{
    void                       *userdata;
    nux_serde_reader_callback_t callback;
    nux_status_t                status;
} nux_serde_reader_t;

typedef struct
{
    nux_file_t        *file;
    nux_b32_t          has_previous_value;
    nux_serde_writer_t writer;
    nux_serde_type_t   stack[256];
    nux_u32_t          depth;
} nux_json_writer_t;

struct jsmntok;

typedef struct
{
    nux_serde_reader_t reader;
    const nux_c8_t    *json;
    nux_u32_t          json_size;
    void              *tokens;
    nux_u32_t          tokens_capa;
    nux_u32_t          tokens_count;
    struct jsmntok    *it;
    nux_u32_t          depth;
    struct jsmntok    *iters[256];
} nux_json_reader_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

nux_u32_t nux_stat(nux_stat_t info);
nux_f32_t nux_time_elapsed();
nux_f32_t nux_time_delta();
nux_u32_t nux_time_frame();
nux_u64_t nux_time_timestamp();

nux_u32_t nux_random();
nux_f32_t nux_random01();

nux_arena_t *nux_arena_new(nux_arena_t *arena);
void         nux_arena_clear(nux_arena_t *arena);
nux_u32_t    nux_arena_block_count(nux_arena_t *arena);
nux_u32_t    nux_arena_memory_usage(nux_arena_t *arena);

nux_arena_t *nux_arena_core(void);
nux_arena_t *nux_arena_frame(void);

void            nux_log_set_level(nux_log_level_t level);
nux_log_level_t nux_log_level(void);

void nux_u32_vec_fill_reversed(nux_u32_vec_t *v);

void  nux_resource_register(nux_u32_t index, nux_resource_type_info_t info);
void *nux_resource_new(nux_arena_t *a, nux_u32_t type);
void *nux_resource_get(nux_u32_t type, nux_rid_t rid);
void *nux_resource_check(nux_u32_t type, nux_rid_t rid);
nux_status_t nux_resource_reload(nux_rid_t rid);

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

void nux_vlog(nux_log_level_t level, const nux_c8_t *fmt, va_list args);
void nux_log(nux_log_level_t level, const nux_c8_t *fmt, ...);

void            nux_error(const nux_c8_t *fmt, ...);
void            nux_error_enable(void);
void            nux_error_disable(void);
void            nux_error_reset(void);
const nux_c8_t *nux_error_get_message(void);
nux_status_t    nux_error_get_status(void);

void         nux_module_register(nux_module_info_t info);
nux_status_t nux_module_requires(const nux_c8_t *name);

void nux_config_set_u32(const nux_c8_t *name, nux_u32_t v);

nux_config_t *nux_config(void);
void         *nux_userdata(void);

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

nux_status_t nux_disk_mount(const nux_c8_t *path);

nux_b32_t    nux_file_exists(const nux_c8_t *path);
nux_file_t  *nux_file_open(nux_arena_t    *arena,
                           const nux_c8_t *path,
                           nux_io_mode_t   mode);
void         nux_file_close(nux_file_t *file);
nux_u32_t    nux_file_read(nux_file_t *file, void *data, nux_u32_t n);
nux_u32_t    nux_file_write(nux_file_t *file, const void *data, nux_u32_t n);
nux_status_t nux_file_seek(nux_file_t *file, nux_u32_t cursor);
nux_status_t nux_file_stat(nux_file_t *file, nux_file_stat_t *stat);
void *nux_file_load(nux_arena_t *a, const nux_c8_t *path, nux_u32_t *size);

nux_status_t nux_io_write_cart_data(const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

void nux_serde_writer_init(nux_serde_writer_t         *s,
                           void                       *userdata,
                           nux_serde_writer_callback_t callback);
void nux_serde_reader_init(nux_serde_reader_t         *s,
                           void                       *userdata,
                           nux_serde_reader_callback_t callback);

nux_status_t nux_json_writer_init(nux_json_writer_t *j, const nux_c8_t *path);
void         nux_json_writer_close(nux_json_writer_t *j);
nux_status_t nux_json_reader_init(nux_json_reader_t *j, const nux_c8_t *path);

void nux_serde_write(nux_serde_writer_t *s, const nux_serde_value_t *value);
void nux_serde_write_object(nux_serde_writer_t *s, const nux_c8_t *key);
void nux_serde_write_array(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           nux_u32_t           size);
void nux_serde_write_end(nux_serde_writer_t *s);
void nux_serde_write_u32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_u32_t           v);
void nux_serde_write_f32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_f32_t           v);
void nux_serde_write_v3(nux_serde_writer_t *s, const nux_c8_t *key, nux_v3_t v);
void nux_serde_write_q4(nux_serde_writer_t *s, const nux_c8_t *key, nux_q4_t v);
void nux_serde_write_bytes(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           const nux_u8_t     *bytes,
                           nux_u32_t           size);
void nux_serde_write_string(nux_serde_writer_t *s,
                            const nux_c8_t     *key,
                            const nux_c8_t     *v);
void nux_serde_write_rid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_rid_t           rid);
void nux_serde_write_eid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_nid_t           v);

void nux_serde_read(nux_serde_reader_t *s, nux_serde_value_t *value);
void nux_serde_read_object(nux_serde_reader_t *s, const nux_c8_t *key);
void nux_serde_read_array(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          nux_u32_t          *size);
void nux_serde_read_end(nux_serde_reader_t *s);
void nux_serde_read_u32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_u32_t          *v);
void nux_serde_read_f32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_f32_t          *v);
void nux_serde_read_v3(nux_serde_reader_t *s, const nux_c8_t *key, nux_v3_t *v);
void nux_serde_read_q4(nux_serde_reader_t *s, const nux_c8_t *key, nux_q4_t *v);
void nux_serde_read_bytes(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          const nux_u8_t    **v,
                          nux_u32_t          *n);
void nux_serde_read_string(nux_serde_reader_t *s,
                           const nux_c8_t     *key,
                           const nux_c8_t    **v,
                           nux_u32_t          *n);
void nux_serde_read_rid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_rid_t          *rid);
void nux_serde_read_nid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_nid_t          *v);

#endif
