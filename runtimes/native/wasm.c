#include "wasm.h"

#include "logger.h"

#include <wasm_export.h>

#define START_CALLBACK  "start"
#define UPDATE_CALLBACK "update"

static struct
{
    wasm_module_t        module;
    wasm_module_inst_t   instance;
    wasm_exec_env_t      env;
    wasm_function_inst_t start_callback;
    wasm_function_inst_t update_callback;
    vm_t                *active_vm;
} wasm;

static void
trace (wasm_exec_env_t env, const void *s)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_trace(vm, s);
}
static void
alloctex (wasm_exec_env_t env, nu_u32_t idx, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_alloctex(vm, idx, size);
}
static void
writetex (wasm_exec_env_t env,
          nu_u32_t        idx,
          nu_u32_t        x,
          nu_u32_t        y,
          nu_u32_t        w,
          nu_u32_t        h,
          const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_writetex(vm, idx, x, y, w, h, p);
}
static void
allocmesh (wasm_exec_env_t env,
           nu_u32_t        idx,
           nu_u32_t        count,
           nu_u32_t        primitive,
           nu_u32_t        attribs)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_allocmesh(vm, idx, count, primitive, attribs);
}
static void
writemesh (wasm_exec_env_t env,
           nu_u32_t        idx,
           nu_u32_t        attribs,
           nu_u32_t        first,
           nu_u32_t        count,
           const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_writemesh(vm, idx, attribs, first, count, p);
}

static void
allocmodel (wasm_exec_env_t env, nu_u32_t idx, nu_u32_t count)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_allocmodel(vm, idx, count);
}
static void
writemodel (wasm_exec_env_t env,
            nu_u32_t        idx,
            nu_u32_t        node,
            nu_u32_t        mesh,
            nu_u32_t        texture,
            nu_u32_t        parent,
            const nu_f32_t *transform)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_writemodel(vm, idx, node, mesh, texture, parent, transform);
}

static void
transform (wasm_exec_env_t env, nu_u32_t transform, const nu_f32_t *m)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_transform(vm, transform, m);
}
static void
cursor (wasm_exec_env_t env, nu_u32_t x, nu_u32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_cursor(vm, x, y);
}
static void
fogcolor (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_fogcolor(vm, color);
}
static void
fogdensity (wasm_exec_env_t env, nu_f32_t density)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_fogdensity(vm, density);
}
static void
fogrange (wasm_exec_env_t env, nu_f32_t near, nu_f32_t far)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_fogrange(vm, near, far);
}
static void
clear (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_clear(vm, color);
}
static void
draw (wasm_exec_env_t env, nu_u32_t model)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_draw(vm, model);
}
static void
text (wasm_exec_env_t env, const void *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_text(vm, text);
}
static void
print (wasm_exec_env_t env, const void *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_print(vm, text);
}
static void
blit (wasm_exec_env_t env,
      nu_u32_t        idx,
      nu_u32_t        x,
      nu_u32_t        y,
      nu_u32_t        w,
      nu_u32_t        h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    api_blit(vm, idx, x, y, w, h);
}

static nu_u32_t
button (wasm_exec_env_t env, nu_u32_t player)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return api_button(vm, player);
}
static nu_f32_t
axis (wasm_exec_env_t env, nu_u32_t player, nu_u32_t axis)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return api_axis(vm, player, axis);
}

static void *
native_wasm_malloc (mem_alloc_usage_t usage, void *user, nu_size_t n)
{
    switch (usage)
    {
        case Alloc_For_Runtime:
            return malloc(n);
        case Alloc_For_LinearMemory:
            return cpu_malloc(wasm.active_vm, n);
    }
    return NU_NULL;
}
static void *
native_wasm_realloc (mem_alloc_usage_t usage,
                     nu_bool_t         full_size_mmapped,
                     void             *user,
                     void             *p,
                     nu_u32_t          n)
{
    if (usage == Alloc_For_Runtime)
    {
        return realloc(p, n);
    }
    return NU_NULL;
}
void
native_wasm_free (mem_alloc_usage_t usage, void *user, void *p)
{
    if (usage == Alloc_For_Runtime)
    {
        free(p);
    }
}

static NativeSymbol wasm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(trace, "(*)"),
    EXPORT_WASM_API_WITH_SIG(alloctex, "(ii)"),
    EXPORT_WASM_API_WITH_SIG(writetex, "(iiiii*)"),
    EXPORT_WASM_API_WITH_SIG(allocmesh, "(iiii)"),
    EXPORT_WASM_API_WITH_SIG(writemesh, "(iiii*)"),
    EXPORT_WASM_API_WITH_SIG(allocmodel, "(ii)"),
    EXPORT_WASM_API_WITH_SIG(writemodel, "(iiiii*)"),
    EXPORT_WASM_API_WITH_SIG(transform, "(i*)"),
    EXPORT_WASM_API_WITH_SIG(cursor, "(ii)"),
    EXPORT_WASM_API_WITH_SIG(fogcolor, "(i)"),
    EXPORT_WASM_API_WITH_SIG(fogdensity, "(f)"),
    EXPORT_WASM_API_WITH_SIG(fogrange, "(ff)"),
    EXPORT_WASM_API_WITH_SIG(clear, "(i)"),
    EXPORT_WASM_API_WITH_SIG(draw, "(i)"),
    EXPORT_WASM_API_WITH_SIG(text, "(*)"),
    EXPORT_WASM_API_WITH_SIG(print, "(*)"),
    EXPORT_WASM_API_WITH_SIG(blit, "(iiiii)"),
    EXPORT_WASM_API_WITH_SIG(button, "(i)i"),
    EXPORT_WASM_API_WITH_SIG(axis, "(ii)f"),
};

nu_status_t
wasm_init (void)
{
    // Configure memory allocator
    RuntimeInitArgs init_args;
    nu_memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type                          = Alloc_With_Allocator;
    init_args.mem_alloc_option.allocator.malloc_func  = native_wasm_malloc;
    init_args.mem_alloc_option.allocator.realloc_func = native_wasm_realloc;
    init_args.mem_alloc_option.allocator.free_func    = native_wasm_free;
    init_args.mem_alloc_option.allocator.user_data    = NU_NULL;

    init_args.native_module_name = "env";
    init_args.native_symbols     = wasm_native_symbols;
    init_args.n_native_symbols   = NU_ARRAY_SIZE(wasm_native_symbols);

    init_args.max_thread_num = 1;

    // wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
    wasm_runtime_set_log_level(WASM_LOG_LEVEL_ERROR);

    if (!wasm_runtime_full_init(&init_args))
    {
        logger_log(NU_LOG_ERROR, "Failed to fully initialize wasm");
        return NU_FAILURE;
    }

    return NU_SUCCESS;
}
static void
wasm_unload_cart (vm_t *vm)
{
    if (wasm.env)
    {
        wasm_runtime_destroy_exec_env(wasm.env);
    }
    if (wasm.instance)
    {
        wasm_runtime_deinstantiate(wasm.instance);
    }
    if (wasm.module)
    {
        wasm_runtime_unload(wasm.module);
    }
}
void
wasm_free (void)
{
    wasm_runtime_destroy();
}
nu_status_t
os_cpu_load_wasm (vm_t *vm, nu_byte_t *buffer, nu_size_t buffer_size)
{
    // Load module
    nu_char_t error_buf[128];
    wasm.module
        = wasm_runtime_load(buffer, buffer_size, error_buf, sizeof(error_buf));
    if (!wasm.module)
    {
        vm_log(vm, NU_LOG_ERROR, "Load wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Instantiate module
    const nu_size_t init_stack_size = NU_MEM_32K;
    wasm.active_vm                  = vm;
    wasm.instance                   = wasm_runtime_instantiate(
        wasm.module, init_stack_size, 0, error_buf, sizeof(error_buf));
    if (!wasm.instance)
    {
        vm_log(
            vm, NU_LOG_ERROR, "Instantiate wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Create execution env
    wasm.env = wasm_runtime_create_exec_env(wasm.instance, init_stack_size);
    if (!wasm.env)
    {
        vm_log(vm, NU_LOG_ERROR, "Create wasm execution environment failed");
    }
    wasm_runtime_set_user_data(wasm.env, vm);

    // Find entry point
    wasm.start_callback
        = wasm_runtime_lookup_function(wasm.instance, START_CALLBACK);
    if (!wasm.start_callback)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "The " START_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }
    wasm.update_callback
        = wasm_runtime_lookup_function(wasm.instance, UPDATE_CALLBACK);
    if (!wasm.update_callback)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "The " UPDATE_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    return NU_SUCCESS;
}
nu_status_t
os_cpu_call_event (vm_t *vm, cpu_event_t event)
{
    wasm_function_inst_t callback      = NU_NULL;
    const nu_char_t     *callback_name = NU_NULL;
    switch (event)
    {
        case CPU_EVENT_START:
            callback      = wasm.start_callback;
            callback_name = START_CALLBACK;
            break;
        case CPU_EVENT_UPDATE:
            callback      = wasm.update_callback;
            callback_name = UPDATE_CALLBACK;
            break;
    }
    NU_ASSERT(callback_name);
    if (!wasm_runtime_call_wasm_a(wasm.env, callback, 0, NU_NULL, 0, NU_NULL))
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Call wasm function %s failed: %s",
               callback_name,
               wasm_runtime_get_exception(wasm.instance));
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
