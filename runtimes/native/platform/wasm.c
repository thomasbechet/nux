#include "wasm.h"

#include "../gpu.h"
#include "../iop.h"
#include "logger.h"

#include <wasm_export.h>

#define VM_START_CALLBACK  "start"
#define VM_UPDATE_CALLBACK "update"

static struct
{
    wasm_module_t        module;
    wasm_module_inst_t   instance;
    wasm_exec_env_t      env;
    wasm_function_inst_t start_callback;
    wasm_function_inst_t update_callback;
} wasm;

static void
native_write_texture (wasm_exec_env_t env,
                      nu_u32_t        slot,
                      nu_u32_t        x,
                      nu_u32_t        y,
                      nu_u32_t        w,
                      nu_u32_t        h,
                      const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    gpu_write_texture(vm, slot, x, y, w, h, p);
}
static void
native_write_vertex (wasm_exec_env_t env,
                     nu_u32_t        first,
                     nu_u32_t        count,
                     const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    gpu_write_vertex(vm, first, count, p);
}
static void
native_bind_texture (wasm_exec_env_t env, nu_u32_t slot)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    gpu_bind_texture(vm, slot);
}
static void
native_draw (wasm_exec_env_t env, nu_u32_t first, nu_u32_t count)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    gpu_draw(vm, first, count);
}

static void
native_trace (wasm_exec_env_t env, const void *s, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    iop_log(vm, NU_LOG_INFO, "trace: %.*s", n, s);
}

void *
native_wasm_malloc (mem_alloc_usage_t usage, void *user, nu_size_t n)
{
    logger_log(NU_LOG_INFO,
               "MALLOC %s %lu",
               usage == Alloc_For_Runtime ? "runtime" : "linear",
               n);
    // return vm_malloc(user, n);
    return malloc(n);
}
void *
native_wasm_realloc (mem_alloc_usage_t usage,
                     nu_bool_t         full_size_mmapped,
                     void             *user,
                     void             *p,
                     nu_u32_t          n)
{
    logger_log(NU_LOG_INFO,
               "REALLOC %s %u",
               usage == Alloc_For_Runtime ? "runtime" : "linear",
               n);
    return realloc(p, n);
}
void
native_wasm_free (mem_alloc_usage_t usage, void *user, void *p)
{
    logger_log(NU_LOG_INFO,
               "FREE %s %p",
               usage == Alloc_For_Runtime ? "runtime" : "linear",
               p);
    free(p);
}

static NativeSymbol wasm_native_symbols[]
    = { { "trace", native_trace, "(*i)", NU_NULL },
        { "write_texture", native_write_texture, "(iiiii*)", NU_NULL },
        { "write_vertex", native_write_texture, "(ii*)", NU_NULL },
        { "bind_texture", native_bind_texture, "(i)", NU_NULL },
        { "draw", native_draw, "(ii)", NU_NULL } };

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

    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);

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
os_load_wasm (vm_t *vm, nu_byte_t *buffer, nu_size_t buffer_size)
{
    // Load module
    nu_char_t error_buf[128];
    wasm.module
        = wasm_runtime_load(buffer, buffer_size, error_buf, sizeof(error_buf));
    if (!wasm.module)
    {
        iop_log(vm, NU_LOG_ERROR, "Load wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Instantiate module
    const nu_size_t init_stack_size = vm->config.mem_stack_size;
    const nu_size_t init_heap_size  = 0;
    wasm.instance                   = wasm_runtime_instantiate(wasm.module,
                                             init_stack_size,
                                             init_heap_size,
                                             error_buf,
                                             sizeof(error_buf));
    if (!wasm.instance)
    {
        iop_log(
            vm, NU_LOG_ERROR, "Instantiate wasm module failed: %s", error_buf);
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // Create execution env
    wasm.env = wasm_runtime_create_exec_env(wasm.instance, init_stack_size);
    if (!wasm.env)
    {
        iop_log(vm, NU_LOG_ERROR, "Create wasm execution environment failed");
    }
    wasm_runtime_set_user_data(wasm.env, vm);

    // Find entry point
    wasm.start_callback
        = wasm_runtime_lookup_function(wasm.instance, VM_START_CALLBACK);
    if (!wasm.start_callback)
    {
        iop_log(vm,
                NU_LOG_INFO,
                "The " VM_START_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }
    wasm.update_callback
        = wasm_runtime_lookup_function(wasm.instance, VM_UPDATE_CALLBACK);
    if (!wasm.update_callback)
    {
        iop_log(vm,
                NU_LOG_INFO,
                "The " VM_UPDATE_CALLBACK " wasm function is not found");
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(
            wasm.env, wasm.start_callback, 0, NU_NULL, 0, NU_NULL))
    {
        iop_log(vm,
                NU_LOG_ERROR,
                "Call wasm function " VM_START_CALLBACK " failed: %s",
                wasm_runtime_get_exception(wasm.instance));
        wasm_unload_cart(vm);
        return NU_FAILURE;
    }

    return NU_SUCCESS;
}
nu_status_t
os_update_wasm (vm_t *vm)
{
    if (!wasm_runtime_call_wasm_a(
            wasm.env, wasm.update_callback, 0, NU_NULL, 0, NU_NULL))
    {
        iop_log(vm,
                NU_LOG_ERROR,
                "Call wasm function " VM_UPDATE_CALLBACK " failed: %s",
                wasm_runtime_get_exception(wasm.instance));
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
