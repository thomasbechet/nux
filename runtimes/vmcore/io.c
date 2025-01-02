#include "io.h"

#include "types.h"
#include "platform.h"
#include "wasm.h"
#include "vm.h"

#define READ_U32(p)                                       \
    {                                                     \
        NU_ASSERT(os_read(&vm->user, (p), sizeof(*(p)))); \
        *(p) = nu_u32_le(*(p));                           \
    }

#define CART_HEADER_SIZE sizeof(nux_cart_header_t)

void
nux_io_init (nux_vm_t *vm)
{
    vm->io.heap = vm_malloc(vm, NUX_IO_MEM_SIZE);
}
void
nux_cart_load_full (nux_vm_t *vm, const nu_byte_t *name)
{
    // Load cart header
    os_mount(vm->user, name);
    os_seek(vm->user, 0);
    READ_U32(&vm->io.header.version);
    // TODO: validate
    READ_U32(&vm->io.header.chunk_count);
    // TODO: validate

    // Load chunks
    os_seek(vm->user, CART_HEADER_SIZE);
    for (nu_size_t i = 0; i < vm->io.header.chunk_count; ++i)
    {
        // read chunk header
        nux_chunk_header_t header;
        READ_U32(&header.type);
        READ_U32(&header.length);
        READ_U32(&header.dst);
        printf("%d %d %d\n", header.type, header.length, header.dst);

        switch (header.type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM:
                printf("load wasm %d\n", header.length);
                nux_wasm_load(vm, &header);
                break;
            case NUX_CHUNK_TEX64:
                break;
            case NUX_CHUNK_TEX128: {
                printf("load tex128 %d\n", header.length);
                const nu_size_t size = 128 * 128 * 4;
                NU_ASSERT(os_read(vm->user, vm->io.heap, size));
                os_write_texture(vm->user, NUX_TEX128, header.dst, vm->io.heap);
            }
            break;
            case NUX_CHUNK_TEX256:
                break;
        }
    }
}
