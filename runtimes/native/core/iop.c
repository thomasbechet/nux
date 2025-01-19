#include "iop.h"

#include "cpu.h"
#include "platform.h"

static nu_u32_t
read_u32 (vm_t *vm)
{
    nu_u32_t v;
    NU_ASSERT(os_iop_read(vm, &v, sizeof(v)));
    return nu_u32_le(v);
}
static nu_bool_t
read_header (vm_t *vm, vm_chunk_header_t *header)
{
    header->type   = read_u32(vm);
    header->length = read_u32(vm);
    switch (header->type)
    {
        case VM_CHUNK_RAW: {
            header->target.raw.addr = read_u32(vm);
        }
        break;
        case VM_CHUNK_WASM:
            break;
        case VM_CHUNK_TEXTURE: {
            header->target.texture.slot = read_u32(vm);
            header->target.texture.x    = read_u32(vm);
            header->target.texture.y    = read_u32(vm);
            header->target.texture.w    = read_u32(vm);
            header->target.texture.h    = read_u32(vm);
        }
        break;
        case VM_CHUNK_MESH: {
            header->target.mesh.first = read_u32(vm);
            header->target.mesh.count = read_u32(vm);
        }
        break;
    }
    return NU_TRUE;
}

nu_status_t
iop_init (vm_t *vm)
{
    vm->iop.heap = os_malloc(vm, VM_IO_MEM_SIZE);
    NU_ASSERT(vm->iop.heap);
    return NU_SUCCESS;
}
nu_status_t
iop_load_full (vm_t *vm, const nu_char_t *name)
{
    nu_status_t status = NU_SUCCESS;

    // Load cart header
    os_iop_mount(vm, name);
    os_iop_seek(vm, 0);
    vm->iop.header.version = read_u32(vm);
    // TODO: validate
    vm->iop.header.chunk_count = read_u32(vm);
    // TODO: validate

    // Load chunks
    os_iop_seek(vm, sizeof(vm_cart_header_t));
    for (nu_size_t i = 0; i < vm->iop.header.chunk_count; ++i)
    {
        // read chunk header
        vm_chunk_header_t header;
        NU_ASSERT(read_header(vm, &header));
        switch (header.type)
        {
            case VM_CHUNK_WASM:
                NU_ASSERT(header.length <= VM_IO_MEM_SIZE);
                cpu_load(vm, &header);
                break;
            case VM_CHUNK_TEXTURE: {
                NU_ASSERT(header.length <= VM_IO_MEM_SIZE);
                NU_ASSERT(os_iop_read(vm, vm->iop.heap, header.length));
                os_gpu_write_texture(vm,
                                 header.target.texture.slot,
                                 header.target.texture.x,
                                 header.target.texture.y,
                                 header.target.texture.w,
                                 header.target.texture.h,
                                 vm->iop.heap);
            }
            break;
            case VM_CHUNK_MESH: {
                NU_ASSERT(header.length <= VM_IO_MEM_SIZE);
                NU_ASSERT(os_iop_read(vm, vm->iop.heap, header.length));
                os_gpu_write_vertex(vm,
                                header.target.mesh.first,
                                header.target.mesh.count,
                                vm->iop.heap);
            }
            break;
            default:
                break;
        }
    }

    return status;
}

void
iop_log (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    iop_vlog(vm, level, fmt, args);
    va_end(args);
}
void
iop_vlog (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    os_iop_vlog(vm, level, fmt, args);
}
