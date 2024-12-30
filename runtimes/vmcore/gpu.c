#include "gpu.h"

#include "platform.h"

void
nux_gpu_init (nux_gpu_t *gpu)
{
}

void
write_texture (wasm_exec_env_t env, nu_u32_t type, nu_u32_t slot, const void *p)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    NU_ASSERT(type <= NUX_TEX256);
    switch ((nux_gpu_texture_t)type)
    {
        case NUX_TEX64:
            NU_ASSERT(slot < vm->gpu.config.max_tex64);
            break;
        case NUX_TEX128:
            NU_ASSERT(slot < vm->gpu.config.max_tex128);
            break;
        case NUX_TEX256:
            NU_ASSERT(slot < vm->gpu.config.max_tex256);
            break;
    }
    os_write_texture(vm->user, (nux_gpu_texture_t)type, slot, p);
}
void
write_vertex (wasm_exec_env_t env,
              nu_u32_t        first,
              nu_u32_t        count,
              const void     *p)
{
}
void
bind_texture (wasm_exec_env_t env, nu_u32_t type, nu_u32_t slot)
{
}
void
draw (wasm_exec_env_t env, nu_u32_t first, nu_u32_t count)
{
    nux_vm_t *vm = wasm_runtime_get_user_data(env);
    os_draw(vm->user);
}
