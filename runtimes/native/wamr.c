#include "runtime.h"
#include "wasm_native.c.inc"

#include <wasm_export.h>
#include <wasm_native.h>

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
    nu_bool_t            debug;
} wamr;

static void *
native_wasm_malloc (mem_alloc_usage_t usage, void *user, nu_size_t n)
{
    switch (usage)
    {
        case Alloc_For_Runtime:
            return malloc(n);
        case Alloc_For_LinearMemory: {
            nu_u32_t addr = vm_malloc(wamr.active_vm, n);
            NU_CHECK(addr != ADDR_INVALID, return NU_NULL);
            return wamr.active_vm->mem + addr;
        }
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

static nu_u32_t
fd_write (wasm_exec_env_t env,
          nu_u32_t        fd,
          nu_u32_t       *iovs,
          nu_u32_t        iovs_len,
          nu_u32_t       *nwritten)
{
    vm_t              *vm      = wasm_runtime_get_user_data(env);
    wasm_module_inst_t inst    = wasm_runtime_get_module_inst(env);
    nu_u32_t           written = 0;
    for (nu_size_t i = 0; i < iovs_len; ++i)
    {
        nu_char_t *buf
            = wasm_runtime_addr_app_to_native(inst, (nu_u64_t)iovs[i * 2 + 0]);
        nu_u32_t buf_len = iovs[i * 2 + 1];
        if (buf_len)
        {
            vm_log(vm, NU_LOG_INFO, "%.*s", buf_len, buf);
            written += buf_len;
        }
    }
    *nwritten = written;
    return 0;
}
static nu_u32_t
fd_fdstat_get (wasm_exec_env_t env, nu_u32_t fd, void *p)
{
    return 0;
}
static NativeSymbol wasi_wasm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(fd_write, "(i*i*)i"),
    EXPORT_WASM_API_WITH_SIG(fd_fdstat_get, "(i*)i"),
};

nu_status_t
wamr_init (nu_bool_t debug)
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

    wamr.debug = debug;
    if (debug)
    {
        strcpy(init_args.ip_addr, "127.0.0.1");
        init_args.instance_port = 1234;
    }

    // wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
    wasm_runtime_set_log_level(WASM_LOG_LEVEL_ERROR);

    if (!wasm_runtime_full_init(&init_args))
    {
        logger_log(NU_LOG_ERROR, "Failed to fully initialize wasm");
        return NU_FAILURE;
    }

    // register wasi api
    if (!wasm_runtime_register_natives("wasi_snapshot_preview1",
                                       wasi_wasm_native_symbols,
                                       NU_ARRAY_SIZE(wasi_wasm_native_symbols)))
    {
        logger_log(NU_LOG_ERROR, "Failed to register wasi api");
        return NU_FAILURE;
    }

    return NU_SUCCESS;
}
static void
wasm_unload_cart (vm_t *vm)
{
    if (wamr.env)
    {
        wasm_runtime_destroy_exec_env(wamr.env);
    }
    if (wamr.instance)
    {
        wasm_runtime_deinstantiate(wamr.instance);
    }
    if (wamr.module)
    {
        wasm_runtime_unload(wamr.module);
    }
}
void
wamr_free (void)
{
    wasm_runtime_destroy();
}
void
wamr_override_value (vm_t *vm, const inspect_value_t *value)
{
    void *p = vm->mem + value->addr;
    switch (value->type)
    {
        case SYS_INSPECT_I32:
            *((nu_i32_t *)p) = value->value.i32;
            break;
        case SYS_INSPECT_F32: {
            printf("override %lf\n", value->value.f32);
            *((nu_f32_t *)p) = value->value.f32;
        }
        break;
    }
}
nu_status_t
os_cpu_load_wasm (vm_t *vm, nu_byte_t *buffer, nu_size_t buffer_size)
{
    // Load module
    nu_char_t error_buf[128];
    wamr.module
        = wasm_runtime_load(buffer, buffer_size, error_buf, sizeof(error_buf));
    if (!wamr.module)
    {
        vm_log(vm, NU_LOG_ERROR, "Load wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Instantiate module
    const nu_size_t init_stack_size = NU_MEM_32K;
    wamr.active_vm                  = vm;
    wamr.instance                   = wasm_runtime_instantiate(
        wamr.module, init_stack_size, 0, error_buf, sizeof(error_buf));
    if (!wamr.instance)
    {
        vm_log(
            vm, NU_LOG_ERROR, "Instantiate wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Create execution env
    wamr.env = wasm_runtime_create_exec_env(wamr.instance, init_stack_size);
    if (!wamr.env)
    {
        vm_log(vm, NU_LOG_ERROR, "Create wasm execution environment failed");
    }
    wasm_runtime_set_user_data(wamr.env, vm);

    // Find entry point
    wamr.start_callback
        = wasm_runtime_lookup_function(wamr.instance, START_CALLBACK);
    if (!wamr.start_callback)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "The " START_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }
    wamr.update_callback
        = wasm_runtime_lookup_function(wamr.instance, UPDATE_CALLBACK);
    if (!wamr.update_callback)
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "The " UPDATE_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Enable debugging
    if (wamr.debug)
    {
#ifdef NUX_BUILD_WASM_DEBUG
        nu_u32_t port = wasm_runtime_start_debug_instance(wamr.env);
        logger_log(NU_LOG_INFO, "Starting debug instance on port %u", port);
#endif
    }

    return NU_SUCCESS;
}
nu_status_t
os_cpu_call_event (vm_t *vm, wasm_event_t event)
{
    wasm_function_inst_t callback      = NU_NULL;
    const nu_char_t     *callback_name = NU_NULL;
    switch (event)
    {
        case WASM_EVENT_START:
            callback      = wamr.start_callback;
            callback_name = START_CALLBACK;
            break;
        case WASM_EVENT_UPDATE:
            callback      = wamr.update_callback;
            callback_name = UPDATE_CALLBACK;
            break;
    }
    NU_ASSERT(callback_name);
    if (!wasm_runtime_call_wasm_a(wamr.env, callback, 0, NU_NULL, 0, NU_NULL))
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Call wasm function %s failed: %s",
               callback_name,
               wasm_runtime_get_exception(wamr.instance));
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
