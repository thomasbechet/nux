#ifndef NU_VM_IMPL_H
#define NU_VM_IMPL_H

#include <internal.h>

static NativeSymbol nu_wasm_vm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(push_gpu_state, "(*)"),
    EXPORT_WASM_API_WITH_SIG(pop_gpu_state, "(*)"),
};

static char global_heap_buf[512 * 1024];

static void
wasm_init (const nu_byte_t *buffer, nu_size_t size)
{
    // Configure memory allocator
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    init_args.native_module_name = "env";
    init_args.native_symbols     = nu_wasm_vm_native_symbols;
    init_args.n_native_symbols   = NU_ARRAY_SIZE(nu_wasm_vm_native_symbols);

    if (!wasm_runtime_full_init(&init_args))
    {
        printf("Failed to full init wasm\n");
    }

    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);

    // Load module
    char          error_buf[128];
    wasm_module_t module = wasm_runtime_load(
        (nu_byte_t *)buffer, size, error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
    }

    // Instantiate module
    const nu_size_t    stack_size = 8092;
    const nu_size_t    heap_size  = 8092;
    wasm_module_inst_t instance   = wasm_runtime_instantiate(
        module, stack_size, heap_size, error_buf, sizeof(error_buf));
    if (!instance)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
    }

    // Create execution env
    wasm_exec_env_t env = wasm_runtime_create_exec_env(instance, stack_size);
    if (!env)
    {
        printf("Create wasm execution environment failed.\n");
    }

    // Find entry point
    wasm_function_inst_t func = wasm_runtime_lookup_function(instance, "start");
    if (!func)
    {
        printf("The start wasm function is not found.\n");
    }

    // wasm_val_t results[1] = { { .kind = WASM_F32, .of.f32 = 0 } };
    // wasm_val_t arguments[3] = {
    //     { .kind = WASM_I32, .of.i32 = 10 },
    //     { .kind = WASM_F64, .of.f64 = 0.000101 },
    //     { .kind = WASM_F32, .of.f32 = 300.002 },
    // };

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(env, func, 0, NU_NULL, 0, NU_NULL))
    {
        printf("Call wasm function start failed. %s\n",
               wasm_runtime_get_exception(instance));
    }
}

#endif
