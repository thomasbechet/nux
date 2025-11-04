#include "internal.h"

static nux_base_module_t _module;

static nux_status_t
bootstrap_core_arena (void)
{
    // Create core arena
    // NOTE: The core arena is a block arena using the os allocator
    // NOTE: The core arena resource entry doesn't have arena

    // 1. Allocate arena + header
    nux_resource_header_t *core_arena_header = nux_malloc(
        &_module.os_allocator, nux_resource_header_size(sizeof(nux_arena_t)));
    NUX_CHECK(core_arena_header, return NUX_FAILURE);
    // 2. Get arena from the header
    _module.core_arena = nux_resource_header_to_data(core_arena_header);
    // 3. Initialize core arena with block arena
    nux_block_arena_init(
        _module.core_arena, &_module.core_block_arena, &_module.os_allocator);
    // 4. Allocate resource table
    NUX_CHECK(nux_resource_pool_init(_module.core_arena, &_module.resources),
              return NUX_FAILURE);
    // 5. Reserve index 0 for null id
    nux_resource_pool_add(&_module.resources);
    // 6. Create core arena resource entry
    nux_resource_entry_t *entry
        = nux_resource_add(&_module.resources, NUX_RESOURCE_ARENA);
    NUX_CHECK(entry, return NUX_FAILURE);
    entry->data = _module.core_arena;
    // 7. Initialize core arena header
    nux_resource_header_init(core_arena_header, entry->rid);

    return NUX_SUCCESS;
}

static nux_status_t
module_pre_update (void)
{
    nux_os_stats_update(nux_userdata(), _module.stats);
    return NUX_SUCCESS;
}
static nux_status_t
module_post_update (void)
{
    nux_arena_clear(nux_arena_frame());
    _module.time_elapsed += nux_time_delta();
    ++_module.frame;
    nux_os_stats_update(nux_userdata(), _module.stats);
    return NUX_SUCCESS;
}

nux_status_t
nux_base_init (void *userdata)
{
    nux_memset(&_module, 0, sizeof(_module));

    // Initialize os allocator
    _module.os_allocator.userdata = userdata;
    _module.os_allocator.alloc    = nux_os_alloc;

    // Bootstrap core arena
    bootstrap_core_arena();

    // Initialize modules
    NUX_CHECK(nux_module_vec_init_capa(_module.core_arena,
                                       DEFAULT_MODULE_CAPACITY,
                                       &_module.modules),
              return NUX_FAILURE);
    NUX_CHECK(nux_ptr_vec_init_capa(_module.core_arena,
                                    DEFAULT_MODULE_DEPENDENCIES_CAPACITY,
                                    &_module.modules_dependencies),
              return NUX_FAILURE);
    _module.active_module = NUX_NULL;

    // Initialize system state
    _module.userdata = userdata;
    _module.config.log.level
        = NUX_LOG_DEBUG; // see errors during initialization
    _module.running      = NUX_TRUE;
    _module.pcg          = nux_pcg(10243124, 1823719241);
    _module.error_enable = NUX_TRUE;
    nux_error_reset();

    // Initialize configuration
    _module.config.hotreload                       = NUX_FALSE;
    _module.config.log.level                       = NUX_LOG_INFO;
    _module.config.window.enable                   = NUX_TRUE;
    _module.config.window.width                    = 900;
    _module.config.window.height                   = 400;
    _module.config.scene.enable                    = NUX_TRUE;
    _module.config.physics.enable                  = NUX_TRUE;
    _module.config.graphics.transforms_buffer_size = 8192;
    _module.config.graphics.batches_buffer_size    = 8192;
    _module.config.graphics.vertices_buffer_size   = (1 << 18) * 5;
    _module.config.debug.enable                    = NUX_TRUE;
    _module.config.debug.console                   = NUX_TRUE;

    // Register base types
    nux_resource_type_t *type;
    type = nux_resource_register(NUX_RESOURCE_NULL, 0, "null");
    type = nux_resource_register(
        NUX_RESOURCE_ARENA, sizeof(nux_arena_t), "arena");
    type->cleanup = nux_arena_cleanup;
    type          = nux_resource_register(
        NUX_RESOURCE_EVENT, sizeof(nux_event_t), "event");

    // Create frame arena
    _module.frame_arena = nux_arena_new(_module.core_arena);
    NUX_ASSERT(_module.frame_arena);
    nux_resource_set_name(_module.frame_arena, "frame_arena");

    return NUX_SUCCESS;
}
void
nux_base_free (void)
{
    // Free core memory
    nux_arena_free(_module.core_arena);
    // Free core arena
    nux_resource_header_t *core_arena_header
        = nux_resource_header_from_data(_module.core_arena);
    nux_free(&_module.os_allocator,
             core_arena_header,
             nux_resource_header_size(sizeof(nux_block_arena_t)));
}

nux_pcg_t *
nux_base_pcg (void)
{
    return &_module.pcg;
}
nux_resource_pool_t *
nux_base_resources (void)
{
    return &_module.resources;
}
nux_resource_type_t *
nux_base_resource_types (void)
{
    return _module.resources_types;
}
nux_allocator_t *
nux_os_allocator (void)
{
    return &_module.os_allocator;
}

void
nux_module_begin (const nux_c8_t *name, void *data, nux_u32_t size)
{
    nux_module_t *m = nux_module_vec_push(&_module.modules);
    NUX_ASSERT(_module.active_module);
    nux_memset(m, 0, sizeof(*m));
    m->info.name               = name;
    m->info.data               = data;
    m->info.size               = size;
    m->info.dependencies_first = _module.modules_dependencies.size;
    _module.active_module      = m;
}
void
nux_module_on_init (nux_status_t (*callback)(void))
{
    nux_module_t *m = _module.active_module;
    NUX_ASSERT(m);
    m->info.on_init = callback;
}
void
nux_module_on_free (void (*callback)(void))
{
    nux_module_t *m = _module.active_module;
    NUX_ASSERT(m);
    m->info.on_free = callback;
}
void
nux_module_requires (const nux_c8_t *name)
{
    nux_module_t *m = _module.active_module;
    NUX_ASSERT(m);
    NUX_ASSERT(nux_ptr_vec_pushv(&_module.modules_dependencies, (void *)name));
    ++m->info.dependencies_count;
}
void
nux_module_end (void)
{
    _module.active_module = NUX_NULL;
}

nux_status_t
nux_modules_init (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->status != NUX_MODULE_UNINITIALIZED)
        {
            continue;
        }
        if (m->info.data && m->info.size
            && !(m->info.flags & NUX_MODULE_NO_DATA_INITIALIZATION))
        {
            nux_memset(m->info.data, 0, m->info.size);
        }
        if (m->info.init)
        {
            NUX_ENSURE(m->info.init(),
                       return NUX_FAILURE,
                       "failed to init '%s' module",
                       m->info.name);
        }
        m->status = NUX_MODULE_INITIALIZED;
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_free (void)
{
    // Free in reverse order
    for (nux_u32_t i = _module.modules.size; i > 0; --i)
    {
        nux_module_t *m = _module.modules.data + (i - 1);
        if (m->status == NUX_MODULE_UNINITIALIZED)
        {
            continue;
        }
        if (m->info.free)
        {
            NUX_ENSURE(m->info.free(),
                       return NUX_FAILURE,
                       "failed to free '%s' module",
                       m->info.name);
        }
        m->status = NUX_MODULE_UNINITIALIZED;
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_pre_update (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->info.pre_update)
        {
            NUX_ENSURE(m->info.pre_update(),
                       return NUX_FAILURE,
                       "failed to pre update '%s' module",
                       m->info.name);
        }
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_update (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->info.update)
        {
            NUX_ENSURE(m->info.update(),
                       return NUX_FAILURE,
                       "failed to update '%s' module",
                       m->info.name);
        }
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_post_update (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->info.post_update)
        {
            NUX_ENSURE(m->info.post_update(),
                       return NUX_FAILURE,
                       "failed to post update '%s' module",
                       m->info.name);
        }
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_on_event (nux_os_event_t *event)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->info.on_event)
        {
            m->info.on_event(event);
        }
    }
    return NUX_SUCCESS;
}

nux_config_t *
nux_config (void)
{
    return &_module.config;
}
void *
nux_userdata (void)
{
    return _module.userdata;
}

nux_u32_t
nux_stat (nux_stat_t stat)
{
    return _module.stats[stat];
}
nux_f32_t
nux_time_elapsed (void)
{
    return _module.time_elapsed;
}
nux_f32_t
nux_time_delta (void)
{
    return 1. / 60;
}
nux_u32_t
nux_time_frame (void)
{
    return _module.frame;
}
nux_u64_t
nux_time_timestamp (void)
{
    return _module.stats[NUX_STAT_TIMESTAMP];
}

void
nux_error (const nux_c8_t *fmt, ...)
{
    if (_module.error_enable)
    {
        va_list args;
        va_start(args, fmt);
        nux_vsnprintf(
            _module.error_message, sizeof(_module.error_message), fmt, args);
        va_end(args);
#ifdef NUX_BUILD_DEBUG
        NUX_ERROR("%s", nux_error_get_message());
        NUX_ASSERT(NUX_FALSE);
#endif
        _module.error_status = NUX_FAILURE;
    }
}
void
nux_error_enable (void)
{
    _module.error_enable = NUX_TRUE;
}
void
nux_error_disable (void)
{
    _module.error_enable = NUX_FALSE;
}
void
nux_error_reset (void)
{
    _module.error_status = NUX_SUCCESS;
    nux_memset(_module.error_message, 0, sizeof(_module.error_message));
}
const nux_c8_t *
nux_error_get_message (void)
{
    return _module.error_message;
}
nux_status_t
nux_error_get_status (void)
{
    return _module.error_status;
}

nux_arena_t *
nux_arena_core (void)
{
    return _module.core_arena;
}
nux_arena_t *
nux_arena_frame (void)
{
    return _module.frame_arena;
}
