#include "io.h"

#include "types.h"
#include "platform.h"
#include "vm.h"
#include "wasm.h"

static nu_u32_t
read_u32 (nux_vm_t *vm)
{
    nu_u32_t v;
    NU_ASSERT(os_read(&vm->user, &v, sizeof(v)));
    return nu_u32_le(v);
}
static nu_bool_t
read_header (nux_vm_t *vm, nux_chunk_header_t *header)
{
    header->type   = read_u32(vm);
    header->length = read_u32(vm);
    switch (header->type)
    {
        case NUX_CHUNK_RAW: {
            header->target.raw.addr = read_u32(vm);
        }
        break;
        case NUX_CHUNK_WASM:
            break;
        case NUX_CHUNK_TEXTURE: {
            header->target.texture.slot = read_u32(vm);
            header->target.texture.x    = read_u32(vm);
            header->target.texture.y    = read_u32(vm);
            header->target.texture.w    = read_u32(vm);
            header->target.texture.h    = read_u32(vm);
        }
        break;
        case NUX_CHUNK_MESH: {
            header->target.mesh.first = read_u32(vm);
            header->target.mesh.count = read_u32(vm);
        }
        break;
    }
    return NU_TRUE;
}

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
    vm->io.header.version = read_u32(vm);
    // TODO: validate
    vm->io.header.chunk_count = read_u32(vm);
    // TODO: validate

    // Load chunks
    os_seek(vm->user, sizeof(nux_cart_header_t));
    for (nu_size_t i = 0; i < vm->io.header.chunk_count; ++i)
    {
        // read chunk header
        nux_chunk_header_t header;
        NU_ASSERT(read_header(vm, &header));
        switch (header.type)
        {
            case NUX_CHUNK_WASM:
                nux_wasm_load(vm, &header);
                break;
            case NUX_CHUNK_TEXTURE: {
                NU_ASSERT(os_read(vm->user, vm->io.heap, header.length));
                os_write_texture(vm->user,
                                 header.target.texture.slot,
                                 header.target.texture.x,
                                 header.target.texture.y,
                                 header.target.texture.w,
                                 header.target.texture.h,
                                 vm->io.heap);
            }
            break;
            case NUX_CHUNK_MESH: {
                NU_ASSERT(os_read(vm->user, vm->io.heap, header.length));
                os_write_vertex(vm->user,
                                header.target.mesh.first,
                                header.target.mesh.count,
                                vm->io.heap);
            }
            break;
            default:
                break;
        }
    }
}
