#include "iou.h"

#include "cpu.h"
#include "gpu.h"
#include "platform.h"

static nu_u32_t
read_u32 (vm_t *vm)
{
    nu_u32_t v;
    NU_ASSERT(os_iop_read(vm, &v, sizeof(v)));
    return nu_u32_le(v);
}
static nu_f32_t
read_f32 (vm_t *vm)
{
    nu_f32_t v;
    NU_ASSERT(os_iop_read(vm, &v, sizeof(v)));
    return nu_f32_le(v);
}
static nu_m4_t
read_m4 (vm_t *vm)
{
    nu_m4_t m;
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        m.data[i] = read_f32(vm);
    }
    return m;
}
static nu_bool_t
read_header (vm_t *vm, cart_chunk_header_t *header)
{
    header->type   = read_u32(vm);
    header->length = read_u32(vm);
    switch (header->type)
    {
        case CART_CHUNK_RAW: {
            header->raw.addr = read_u32(vm);
        }
        break;
        case CART_CHUNK_WASM:
            break;
        case CART_CHUNK_TEXTURE: {
            header->texture.index = read_u32(vm);
            header->texture.size  = read_u32(vm);
            NU_ASSERT(header->length == 128 * 128 * 4);
        }
        break;
        case CART_CHUNK_MESH: {
            header->mesh.index     = read_u32(vm);
            header->mesh.count     = read_u32(vm);
            header->mesh.primitive = read_u32(vm);
            header->mesh.flags     = read_u32(vm);
        }
        break;
        case CART_CHUNK_MODEL: {
            header->model.index = read_u32(vm);
            header->model.count = read_u32(vm);
        }
        break;
    }
    return NU_TRUE;
}

nu_status_t
iou_init (vm_t *vm)
{
    vm->iou.heap = os_malloc(vm, IOU_MEM_SIZE);
    NU_ASSERT(vm->iou.heap);
    return NU_SUCCESS;
}
nu_status_t
iou_load_full (vm_t *vm, const nu_char_t *name)
{
    nu_status_t status = NU_SUCCESS;

    // Load cart header
    os_iop_mount(vm, name);
    os_iop_seek(vm, 0);
    vm->iou.header.version = read_u32(vm);
    // TODO: validate
    vm->iou.header.chunk_count = read_u32(vm);
    // TODO: validate

    // Load chunks
    os_iop_seek(vm, sizeof(cart_header_t));
    for (nu_size_t i = 0; i < vm->iou.header.chunk_count; ++i)
    {
        // read chunk header
        cart_chunk_header_t header;
        NU_ASSERT(read_header(vm, &header));
        switch (header.type)
        {
            case CART_CHUNK_WASM:
                NU_ASSERT(header.length <= IOU_MEM_SIZE);
                cpu_load(vm, &header);
                break;
            case CART_CHUNK_TEXTURE: {
                NU_ASSERT(header.length <= IOU_MEM_SIZE);
                NU_ASSERT(os_iop_read(vm, vm->iou.heap, header.length));
                gpu_alloc_texture(vm,
                                  header.texture.index,
                                  header.texture.size,
                                  vm->iou.heap);
            }
            break;
            case CART_CHUNK_MESH: {
                NU_ASSERT(header.length <= IOU_MEM_SIZE);
                NU_ASSERT(os_iop_read(vm, vm->iou.heap, header.length));
                gpu_alloc_mesh(vm,
                               header.mesh.index,
                               header.mesh.count,
                               header.mesh.primitive,
                               header.mesh.flags,
                               vm->iou.heap);
            }
            break;
            case CART_CHUNK_MODEL: {
                NU_ASSERT(header.model.count);
                for (nu_u32_t m = 0; m < header.model.count; ++m)
                {
                    nu_u32_t index = header.model.index + m;
                    nu_u32_t mesh  = read_u32(vm);
                    gpu_set_model_mesh(vm, index, mesh);
                    nu_u32_t texture = read_u32(vm);
                    gpu_set_model_texture(vm, index, texture);
                    nu_m4_t transform = read_m4(vm);
                    gpu_set_model_transform(vm, index, transform.data);
                    if (m != header.model.count - 1)
                    {
                        gpu_set_model_parent(vm, index, index + 1);
                    }
                }
            }
            break;
            default:
                break;
        }
    }

    return status;
}

void
iou_log (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    iou_vlog(vm, level, fmt, args);
    va_end(args);
}
void
iou_vlog (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    os_iop_vlog(vm, level, fmt, args);
}
