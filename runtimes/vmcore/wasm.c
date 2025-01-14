#include "wasm.h"

#include "vm.h"
#include "gpu.h"
#include "platform.h"

#define VM_START_CALLBACK  "start"
#define VM_UPDATE_CALLBACK "update"

static void
trace (wasm_exec_env_t env, const void *s, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    os_log(vm->user, NU_LOG_INFO, "%.*s", n, s);
}

static NativeSymbol nux_wasm_vm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(trace, "(*i)"),
    EXPORT_WASM_API_WITH_SIG(write_texture, "(iiiii*)"),
    EXPORT_WASM_API_WITH_SIG(write_vertex, "(ii*)"),
    EXPORT_WASM_API_WITH_SIG(bind_texture, "(i)"),
    EXPORT_WASM_API_WITH_SIG(draw, "(ii)"),
};

nu_status_t
vm_wasm_init (vm_t *vm)
{
    // Configure memory allocator
    RuntimeInitArgs init_args;
    nu_memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf
        = vm_malloc(vm, vm->config.mem_heap_size);
    init_args.mem_alloc_option.pool.heap_size = vm->config.mem_heap_size;
    NU_ASSERT(init_args.mem_alloc_option.pool.heap_buf);

    init_args.native_module_name = "env";
    init_args.native_symbols     = nux_wasm_vm_native_symbols;
    init_args.n_native_symbols   = NU_ARRAY_SIZE(nux_wasm_vm_native_symbols);

    init_args.max_thread_num = 1;

    // wasm_runtime_set_log_level(WASM_LOG_LEVEL_FATAL);
    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);

    if (!wasm_runtime_full_init(&init_args))
    {
        vm_log(vm, NU_LOG_ERROR, "Failed to fully initialize wasm");
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
nu_status_t
vm_wasm_load (vm_t *vm, const vm_chunk_header_t *header)
{
    vm_wasm_t *wasm = &vm->wasm;

    // Load module data
    wasm->buffer_size = header->length;
    NU_ASSERT(header->length);
    wasm->buffer = vm_malloc(vm, header->length);
    NU_ASSERT(wasm->buffer);
    NU_ASSERT(os_read(vm->user, wasm->buffer, header->length));

    // Load module
    nu_char_t error_buf[128];
    wasm->module = wasm_runtime_load(
        wasm->buffer, wasm->buffer_size, error_buf, sizeof(error_buf));
    if (!wasm->module)
    {
        vm_log(vm, NU_LOG_ERROR, "Load wasm module failed: %s", error_buf);
        return NU_FAILURE;
    }

    // Instantiate module
    const nu_size_t init_stack_size = vm->config.mem_stack_size;
    // const nu_size_t init_heap_size  = NU_MEM_1K;
    const nu_size_t init_heap_size = 0;
    wasm->instance                 = wasm_runtime_instantiate(wasm->module,
                                              init_stack_size,
                                              init_heap_size,
                                              error_buf,
                                              sizeof(error_buf));
    if (!wasm->instance)
    {
        vm_log(
            vm, NU_LOG_ERROR, "Instantiate wasm module failed: %s", error_buf);
        return NU_FAILURE;
    }

    // Create execution env
    wasm->env = wasm_runtime_create_exec_env(wasm->instance, init_stack_size);
    if (!wasm->env)
    {
        vm_log(vm, NU_LOG_ERROR, "Create wasm execution environment failed");
    }
    wasm_runtime_set_user_data(wasm->env, vm);

    // Find entry point
    wasm->start_callback
        = wasm_runtime_lookup_function(wasm->instance, VM_START_CALLBACK);
    if (!wasm->start_callback)
    {
        vm_log(vm,
               NU_LOG_INFO,
               "The " VM_START_CALLBACK " wasm function is not found");
    }
    wasm->update_callback
        = wasm_runtime_lookup_function(wasm->instance, VM_UPDATE_CALLBACK);
    if (!wasm->update_callback)
    {
        vm_log(vm,
               NU_LOG_INFO,
               "The " VM_UPDATE_CALLBACK " wasm function is not found");
    }

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(
            wasm->env, wasm->start_callback, 0, NU_NULL, 0, NU_NULL))
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Call wasm function " VM_START_CALLBACK " failed: %s",
               wasm_runtime_get_exception(wasm->instance));
    }

    return NU_SUCCESS;
}
nu_status_t
vm_wasm_update (vm_t *vm)
{
    if (!wasm_runtime_call_wasm_a(
            vm->wasm.env, vm->wasm.update_callback, 0, NU_NULL, 0, NU_NULL))
    {
        vm_log(vm,
               NU_LOG_ERROR,
               "Call wasm function " VM_UPDATE_CALLBACK " failed: %s",
               wasm_runtime_get_exception(vm->wasm.instance));
    }
    return NU_SUCCESS;
}
