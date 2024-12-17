#include <runtimes/native/core.h>
#include <wasm_export.h>
#include <unistd.h>

// GFX resource api
void nu_upload_vertices(int first, int count, void *data);
void nu_upload_texture(int idx, int x, int y, int w, int h, void *data);
int  nu_max_vertices(void);
int  nu_max_textures(void);

// GFX commands api
void nu_bind_texture(int idx);
void nu_draw(int primitive, int first, int count);

// Cart api
void nu_load(const char *section, void *addr);

static nu_byte_t *
nu__seria_load_bytes (const char *filename, nu_size_t *size)
{
    FILE *f = fopen((char *)filename, "rb");
    if (!f)
    {
        printf("failed to open file %s\n", filename);
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

int
main (int argc, char **argv)
{
    // Configure memory allocator
    static char     global_heap_buf[512 * 1024];
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
    if (!wasm_runtime_full_init(&init_args))
    {
        printf("failed to full init wasm\n");
    }

    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);

    // Load wasm file
    nu_size_t  size;
    nu_byte_t *buffer = nu__seria_load_bytes(argv[1], &size);

    // Load module
    char          error_buf[128];
    wasm_module_t module = NU_NULL;
    module               = wasm_runtime_load(
        (nu_byte_t *)buffer, size, error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
        return 0;
    }

    // Instantiate module
    wasm_module_inst_t instance   = NU_NULL;
    const nu_size_t    stack_size = 8092;
    const nu_size_t    heap_size  = 8092;
    instance                      = wasm_runtime_instantiate(
        module, stack_size, heap_size, error_buf, sizeof(error_buf));
    if (!instance)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        return 0;
    }

    // Create execution env
    wasm_exec_env_t env = NU_NULL;
    env                 = wasm_runtime_create_exec_env(instance, stack_size);
    if (!env)
    {
        printf("Create wasm execution environment failed.\n");
        return 0;
    }

    return 0;
}
