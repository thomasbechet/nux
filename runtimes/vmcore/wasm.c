#include "wasm.h"

#include "vm.h"
#include "platform.h"
#include "gpu.h"

#define NU_START_CALLBACK  "start"
#define NU_UPDATE_CALLBACK "update"

static NativeSymbol nux_wasm_vm_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(write_texture, "(ii*)"),
    EXPORT_WASM_API_WITH_SIG(write_vertex, "(ii*)"),
    EXPORT_WASM_API_WITH_SIG(bind_texture, "(i)"),
    EXPORT_WASM_API_WITH_SIG(draw, "(ii)"),
};

void
nux_wasm_init (nux_wasm_t *wasm, const nux_wasm_info_t *info)
{
    // Configure memory allocator
    RuntimeInitArgs init_args;
    nu_memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = info->runtime_heap;
    init_args.mem_alloc_option.pool.heap_size = info->runtime_heap_size;

    init_args.native_module_name = "env";
    init_args.native_symbols     = nux_wasm_vm_native_symbols;
    init_args.n_native_symbols   = NU_ARRAY_SIZE(nux_wasm_vm_native_symbols);

    if (!wasm_runtime_full_init(&init_args))
    {
        printf("Failed to full init wasm\n");
    }

    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
}
void
nux_wasm_load (nux_vm_t *vm, const nux_chunk_header_t *header)
{
    nux_wasm_t *wasm = &vm->wasm;

    // Load module data
    wasm->buffer_size = header->length;
    wasm->buffer      = vm_malloc(vm, header->length);
    NU_ASSERT(os_read(vm->user, wasm->buffer, header->length));

    // Load module
    char error_buf[128];
    wasm->module = wasm_runtime_load(
        wasm->buffer, wasm->buffer_size, error_buf, sizeof(error_buf));
    if (!wasm->module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
    }

    // Instantiate module
    const nu_size_t init_stack_size = 8092;
    const nu_size_t init_heap_size  = 0;
    wasm->instance                  = wasm_runtime_instantiate(wasm->module,
                                              init_stack_size,
                                              init_heap_size,
                                              error_buf,
                                              sizeof(error_buf));
    if (!wasm->instance)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
    }

    // Create execution env
    wasm->env = wasm_runtime_create_exec_env(wasm->instance, init_stack_size);
    if (!wasm->env)
    {
        printf("Create wasm execution environment failed.\n");
    }

    // Find entry point
    wasm->start_callback
        = wasm_runtime_lookup_function(wasm->instance, NU_START_CALLBACK);
    if (!wasm->start_callback)
    {
        printf("The " NU_START_CALLBACK " wasm function is not found.\n");
    }

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(
            wasm->env, wasm->start_callback, 0, NU_NULL, 0, NU_NULL))
    {
        printf("Call wasm function " NU_START_CALLBACK " failed. %s\n",
               wasm_runtime_get_exception(wasm->instance));
    }
}
