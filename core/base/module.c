#include "internal.h"

static nux_base_module_t _module;

static nux_status_t
bootstrap_resources (void)
{
    nux_resource_pool_t *resources = nux_base_resources();
    // Create core arena
    nux_arena_t core_arena;
    nux_arena_init(&core_arena);
    // Create resource pool
    NUX_CHECK(nux_resource_pool_init(&core_arena, resources),
              return NUX_FAILURE);
    // Reserve index 0 for null id
    nux_resource_pool_add(resources);
    // Reserve index 1 for core arena
    _module.core_arena = nux_resource_new(&core_arena, NUX_RESOURCE_ARENA);
    NUX_CHECK(_module.core_arena, return NUX_FAILURE);
    *_module.core_arena = core_arena;
    // Patch core arena resource to reference itself
    resources->data[1].arena = _module.core_arena;
    // Patch resource pool freelist to reference new core arena
    resources->freelist.arena = _module.core_arena;

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
    nux_arena_reset(nux_arena_frame());
    _module.time_elapsed += nux_time_delta();
    ++_module.frame;
    nux_os_stats_update(nux_userdata(), _module.stats);
    return NUX_SUCCESS;
}
nux_status_t
nux_base_init (void *userdata)
{
    nux_memset(&_module, 0, sizeof(_module));

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

    // Initialize resources with core arena
    NUX_CHECK(bootstrap_resources(), return NUX_FAILURE);

    // Create frame arena
    _module.frame_arena = nux_arena_new(_module.core_arena);
    NUX_ASSERT(_module.frame_arena);
    nux_resource_set_name(_module.frame_arena, "frame_arena");

    // Initialize modules
    NUX_CHECK(nux_module_vec_init_capa(
                  nux_arena_core(), DEFAULT_MODULE_CAPACITY, &_module.modules),
              return NUX_FAILURE);

    // Register itself
    static const nux_module_t info = {
        .name        = "base",
        .size        = sizeof(_module),
        .data        = &_module,
        .flags       = NUX_MODULE_NO_DATA_INITIALIZATION, // already initialized
        .pre_update  = module_pre_update,
        .post_update = module_post_update,
    };
    nux_modules_register(&info);

    return NUX_SUCCESS;
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

nux_status_t
nux_modules_register (const nux_module_t *info)
{
    nux_module_t *m = nux_module_vec_push(&_module.modules);
    NUX_CHECK(m, return NUX_FAILURE);
    *m = *info;
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_init (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->data && m->size
            && !(m->flags & NUX_MODULE_NO_DATA_INITIALIZATION))
        {
            nux_memset(m->data, 0, m->size);
        }
        if (m->init)
        {
            NUX_ENSURE(m->init(),
                       return NUX_FAILURE,
                       "failed to init '%s' module",
                       m->name);
        }
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
        if (m->free)
        {
            NUX_ENSURE(m->free(),
                       return NUX_FAILURE,
                       "failed to free '%s' module",
                       m->name);
        }
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_modules_pre_update (void)
{
    for (nux_u32_t i = 0; i < _module.modules.size; ++i)
    {
        nux_module_t *m = _module.modules.data + i;
        if (m->pre_update)
        {
            NUX_ENSURE(m->pre_update(),
                       return NUX_FAILURE,
                       "failed to pre update '%s' module",
                       m->name);
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
        if (m->update)
        {
            NUX_ENSURE(m->update(),
                       return NUX_FAILURE,
                       "failed to update '%s' module",
                       m->name);
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
        if (m->post_update)
        {
            NUX_ENSURE(m->post_update(),
                       return NUX_FAILURE,
                       "failed to post update '%s' module",
                       m->name);
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
        if (m->on_event)
        {
            m->on_event(event);
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
    return &_module.userdata;
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
