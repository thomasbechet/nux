#include "gpu.h"

#include "platform.h"

void
nux_gpu_init (nux_vm_t *vm)
{
}

void
write_texture (wasm_exec_env_t env,
               nu_u32_t        slot,
               nu_u32_t        x,
               nu_u32_t        y,
               nu_u32_t        w,
               nu_u32_t        h,
               const void     *p)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    NU_ASSERT(slot <= vm->config.gpu_texture_count);
    os_write_texture(vm->user, slot, x, y, w, h, p);
}
void
write_vertex (wasm_exec_env_t env,
              nu_u32_t        first,
              nu_u32_t        count,
              const void     *p)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    NU_ASSERT(first + count <= vm->config.gpu_vertex_count);
    os_write_vertex(vm->user, first, count, p);
}
void
bind_texture (wasm_exec_env_t env, nu_u32_t slot)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    os_bind_texture(vm->user, slot);
}
void
draw (wasm_exec_env_t env, nu_u32_t first, nu_u32_t count)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    os_draw(vm->user, first, count);
}
