#include "vm.h"
#include "common.h"

#include <nux_api.h>

#define NU_START_CALLBACK  "_start"
#define NU_UPDATE_CALLBACK "_update"

static nu_vm_t vm;

static NativeSymbol nu_wasm_vm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(push_gpu_state, "(*)"),
    EXPORT_WASM_API_WITH_SIG(pop_gpu_state, "(*)"),
};

static char global_heap_buf[512 * 1024];

void
vm_init (const nu_byte_t *buffer, nu_size_t size)
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
    char error_buf[128];
    vm.module = wasm_runtime_load(
        (nu_byte_t *)buffer, size, error_buf, sizeof(error_buf));
    if (!vm.module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
    }

    // Instantiate module
    const nu_size_t stack_size = 8092;
    const nu_size_t heap_size  = 0;
    vm.instance                = wasm_runtime_instantiate(
        vm.module, stack_size, heap_size, error_buf, sizeof(error_buf));
    if (!vm.instance)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
    }

    // Create execution env
    vm.env = wasm_runtime_create_exec_env(vm.instance, stack_size);
    if (!vm.env)
    {
        printf("Create wasm execution environment failed.\n");
    }

    // Find entry point
    vm.start_callback
        = wasm_runtime_lookup_function(vm.instance, NU_START_CALLBACK);
    if (!vm.start_callback)
    {
        printf("The " NU_START_CALLBACK " wasm function is not found.\n");
    }

    // wasm_val_t results[1] = { { .kind = WASM_F32, .of.f32 = 0 } };
    // wasm_val_t arguments[3] = {
    //     { .kind = WASM_I32, .of.i32 = 10 },
    //     { .kind = WASM_F64, .of.f64 = 0.000101 },
    //     { .kind = WASM_F32, .of.f32 = 300.002 },
    // };

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(
            vm.env, vm.start_callback, 0, NU_NULL, 0, NU_NULL))
    {
        printf("Call wasm function " NU_START_CALLBACK " failed. %s\n",
               wasm_runtime_get_exception(vm.instance));
    }
}
