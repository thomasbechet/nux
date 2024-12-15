#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <wasm_export.h>

// GFX resource api
void nu_update_vertices(int first, int count, void *data);
void nu_update_texture(int idx, int x, int y, int w, int h, void *data);

// GFX commands api
void nu_bind_texture(int idx);
void nu_draw(int first, int count, int primitive);

// Cart api
void nu_load(const char *section, void *addr);

int
main (int argc, const char *argv[])
{
    int t = nu_alloc_texture(256);
    nu_load_vram(t, "t_alfred");
    int v = nu_alloc_vertices(65536);
    nu_load_vram(v, "v_alfred");

    /* all the runtime memory allocations are retricted in the global_heap_buf
     * array */
    static char     global_heap_buf[512 * 1024];
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    /* configure the memory allocator for the runtime */
    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
    wasm_runtime_full_init(&init_args);
    return 0;
}
