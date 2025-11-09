#ifndef NUX_BASE_INTERNAL_H
#define NUX_BASE_INTERNAL_H

#include <core/platform.h>

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
    nux_resource_info_t info;
    nux_u32_t                first_entry_index;
    nux_u32_t                last_entry_index;
} nux_resource_type_t;

NUX_POOL_DEFINE(nux_resource_pool, nux_resource_entry_t);

typedef struct
{
    nux_module_info_t info;
    nux_b32_t         initialized;
} nux_module_t;

NUX_VEC_DEFINE(nux_module_vec, nux_module_t);

typedef enum
{
    DEFAULT_MODULE_CAPACITY              = 8,
    DEFAULT_MODULE_DEPENDENCIES_CAPACITY = 64,
    ARENA_ALLOCATOR_TYPE                 = 12345,
} nux_base_defaults_t;

struct nux_event_handler_t
{
    void                       *userdata;
    nux_rid_t                   event;
    struct nux_event_handler_t *next;
    struct nux_event_handler_t *prev;
    nux_event_callback_t        callback;
};

typedef struct nux_event_header
{
    struct nux_event_header *next;
    void                    *data;
} nux_event_header_t;

struct nux_event_t
{
    nux_event_type_t     type;
    nux_arena_t         *arena;
    nux_event_handler_t *first_handler;
    nux_event_header_t  *first_event;
    nux_event_cleanup_t  cleanup;
};

typedef enum
{
    NUX_DISK_OS,
    NUX_DISK_CART,
} nux_disk_type_t;

typedef enum
{
    NUX_CART_HEADER_SIZE = 4 * 3,
    NUX_CART_ENTRY_SIZE  = 4 * 6,
} nux_cart_layout_t;

struct nux_file_t
{
    nux_disk_type_t type;
    nux_io_mode_t   mode;
    nux_b32_t       is_open;
    union
    {
        struct
        {
            nux_u32_t slot;
            nux_u32_t offset;
            nux_u32_t length;
            nux_u32_t cursor;
        } cart;
        struct
        {
            nux_u32_t slot;
        } os;
    };
};

typedef struct
{
    nux_b32_t compressed;
    nux_u32_t data_type;
    nux_u32_t data_offset;
    nux_u32_t data_length;
    nux_u32_t path_hash;
    nux_u32_t path_offset;
    nux_u32_t path_length;
} nux_cart_entry_t;

typedef struct
{
    const nux_c8_t   *path;
    nux_file_t       *file;
    nux_cart_entry_t *entries;
    nux_u32_t         entries_count;
} nux_cart_t;

typedef struct
{
    nux_b32_t started;
    nux_rid_t file;
    nux_u32_t entry_count;
    nux_u32_t entry_index;
    nux_u32_t cursor;
} nux_cart_writer_t;

typedef struct nux_disk
{
    nux_disk_type_t type;
    union
    {
        nux_cart_t cart;
    };
} nux_disk_t;

typedef struct
{
    nux_config_t        config;
    nux_b32_t           running;
    nux_u64_t           frame;
    nux_f32_t           time_elapsed;
    nux_pcg_t           pcg;
    nux_u64_t           stats[NUX_STAT_MAX];
    nux_resource_pool_t resources;
    nux_resource_type_t resources_types[NUX_RESOURCE_MAX];
    nux_arena_t        *frame_arena;
    nux_c8_t            error_message[256];
    nux_status_t        error_status;
    nux_status_t        error_enable;
    nux_allocator_t     os_allocator;
    nux_block_arena_t   core_block_arena;
    nux_arena_t        *core_arena;

    nux_module_vec_t modules;

    nux_cart_writer_t cart_writer;
    nux_u32_vec_t     free_file_slots;
} nux_core_module_t;

nux_core_module_t *nux_core(void);

nux_pcg_t           *nux_base_pcg(void);
nux_resource_pool_t *nux_base_resources(void);
nux_resource_type_t *nux_base_resource_types(void);
nux_allocator_t     *nux_os_allocator(void);

nux_resource_entry_t *nux_resource_add(nux_resource_pool_t *resources,
                                       nux_u32_t            type);

nux_u32_t nux_resource_header_size(nux_u32_t size);
void  nux_resource_header_init(nux_resource_header_t *header, nux_rid_t rid);
void *nux_resource_header_to_data(nux_resource_header_t *header);
nux_resource_header_t *nux_resource_header_from_data(void *data);

void nux_arena_cleanup(void *data);

nux_status_t nux_io_init(void);
void         nux_io_free(void);

void nux_file_cleanup(void *data);
void nux_disk_cleanup(void *data);

nux_status_t nux_io_open_os_file(const nux_c8_t *path,
                                 nux_io_mode_t   mode,
                                 nux_u32_t      *ret_slot);
void         nux_io_close_os_file(nux_u32_t slot);

void nux_module_free_all(void);

#endif
