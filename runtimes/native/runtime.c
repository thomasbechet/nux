#include <runtimes/native/core.h>
#include <wasm_export.h>
#include <unistd.h>

// GFX resource api
void nu_upload_vertices(int first, int count, void *data);
void nu_upload_texture(int idx, int x, int y, int w, int h, void *data);

// GFX commands api
void nu_bind_texture(int idx);
void nu_draw(int primitive, int first, int count);

// Cart api
void nu_load(const char *section, void *addr);

static void
print (void)
{
    printf("hello\n");
}

static nu_byte_t *
nu__seria_load_bytes (const char *filename, nu_size_t *size)
{
    FILE *f = fopen((char *)filename, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", filename);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)malloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

static NativeSymbol native_symbols[] = {
    {
        "print", // the name of WASM function name
        print,   // the native function pointer
        "()",    // the function prototype signature, avoid to use i32
        NULL     // attachment is NULL
    },
};

static char global_heap_buf[512 * 1024];

int
main (int argc, char **argv)
{
    // Configure memory allocator
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    init_args.native_module_name = "env";
    init_args.native_symbols     = native_symbols;
    init_args.n_native_symbols   = NU_ARRAY_SIZE(native_symbols);

    if (!wasm_runtime_full_init(&init_args))
    {
        printf("Failed to full init wasm\n");
    }

    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);

    // Load wasm file
    nu_size_t  size;
    nu_byte_t *buffer = nu__seria_load_bytes(argv[1], &size);

    // Load module
    char          error_buf[128];
    wasm_module_t module = wasm_runtime_load(
        (nu_byte_t *)buffer, size, error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
        return 0;
    }

    // Instantiate module
    const nu_size_t    stack_size = 8092;
    const nu_size_t    heap_size  = 8092;
    wasm_module_inst_t instance   = wasm_runtime_instantiate(
        module, stack_size, heap_size, error_buf, sizeof(error_buf));
    if (!instance)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        return 0;
    }

    // Create execution env
    wasm_exec_env_t env = wasm_runtime_create_exec_env(instance, stack_size);
    if (!env)
    {
        printf("Create wasm execution environment failed.\n");
        return 0;
    }

    // Find entry point
    wasm_function_inst_t func = wasm_runtime_lookup_function(instance, "start");
    if (!func)
    {
        printf("The start wasm function is not found.\n");
        return 0;
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
        return 0;
    }

    return 0;
}
