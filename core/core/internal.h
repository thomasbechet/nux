#ifndef NUX_CORE_INTERNAL_H
#define NUX_CORE_INTERNAL_H

#include <core/platform.h>

typedef enum
{
    DEFAULT_MODULE_CAPACITY              = 8,
    DEFAULT_SYSTEM_CAPACITY              = 32,
    DEFAULT_MODULE_DEPENDENCIES_CAPACITY = 64,
} nux_base_defaults_t;

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

typedef struct nux_object_finalizer
{
    nux_id_t id;
} nux_object_header_t;

typedef struct
{
    nux_u32_t       type;
    nux_id_t        id; // for validity check
    nux_arena_t    *arena;
    void           *data;
    const nux_c8_t *path;
    const nux_c8_t *name;
    nux_u32_t       prev_entry_index; // same type
    nux_u32_t       next_entry_index; // same type
} nux_object_entry_t;

typedef struct
{
    nux_module_info_t info;
    nux_b32_t         initialized;
    nux_u32_t         order; // initialization order
} nux_module_t;

typedef struct
{
    nux_system_phase_t    phase;
    nux_system_callback_t callback;
} nux_system_t;

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

struct nux_event_handler_t
{
    void                       *userdata;
    nux_id_t                    event;
    struct nux_event_handler_t *next;
    struct nux_event_handler_t *prev;
    nux_event_callback_t        callback;
};

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
    nux_id_t  file;
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

typedef nux_pool(nux_object_entry_t) nux_object_pool_t;

typedef struct nux_transform_t
{
    nux_v3_t  translation;
    nux_q4_t  rotation;
    nux_v3_t  scale;
    nux_m4_t  global_matrix;
    nux_b32_t dirty;
} nux_transform_t;

typedef struct nux_node_t
{
    nux_scene_t *scene;
    nux_id_t     parent;
    nux_id_t     next;
    nux_id_t     prev;
    nux_id_t     child;
    nux_id_t     components[32];
} nux_node_t;

typedef struct nux_scene_t
{
    nux_node_t *root;
} nux_scene_t;

typedef struct
{
    nux_config_t      config;
    nux_b32_t         running;
    nux_u64_t         frame;
    nux_f32_t         time_elapsed;
    nux_pcg_t         pcg;
    nux_u64_t         stats[NUX_STAT_MAX];
    nux_object_pool_t objects;
    nux_object_type_t object_types[NUX_OBJECT_MAX];
    nux_u32_t         component_count;
    nux_arena_t      *frame_arena;
    nux_c8_t          error_message[256];
    nux_status_t      error_status;
    nux_status_t      error_enable;
    nux_arena_t      *core_arena;

    nux_vec(nux_module_t) modules;
    nux_u32_t modules_init_order;
    nux_vec(nux_system_t) systems;

    nux_cart_writer_t cart_writer;
    nux_u32_vec_t     free_file_slots;

    nux_scene_t *active_scene;
    nux_scene_t *default_scene;
} nux_core_module_t;

nux_core_module_t *nux_core(void);

nux_object_entry_t *nux_object_add(nux_object_pool_t *resources,
                                   nux_u32_t          type);

nux_u32_t nux_object_header_size(nux_u32_t size);
void      nux_object_header_init(nux_object_header_t *header, nux_id_t id);
void     *nux_object_header_to_data(nux_object_header_t *header);
nux_object_header_t *nux_object_header_from_data(void *data);

void nux_arena_init_core(nux_arena_t *arena);
void nux_arena_cleanup(void *data);

nux_status_t nux_io_init(void);
void         nux_io_free(void);

nux_status_t nux_scene_init(void);

void nux_file_cleanup(void *data);
void nux_disk_cleanup(void *data);

nux_status_t nux_io_open_os_file(const nux_c8_t *path,
                                 nux_io_mode_t   mode,
                                 nux_u32_t      *ret_slot);
void         nux_io_close_os_file(nux_u32_t slot);
nux_status_t nux_io_write_cart_data(const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

void nux_module_init_all(void);
void nux_module_free_all(void);

void nux_system_call(nux_system_phase_t phase);

#endif
