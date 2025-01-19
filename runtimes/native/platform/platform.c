#include "window.h"
#include "renderer.h"
#include "wasm.h"

static nu_byte_t global_heap[NU_MEM_32M];

nu_status_t
os_init_platform (void)
{
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);

    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}
void
os_free_platform (void)
{
    wasm_free();
    renderer_free();
    window_free();
}
