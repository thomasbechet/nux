#include "core/vm.h"
#include <wasm_export.h>

static void
trace (wasm_exec_env_t env, const nux_c8_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_trace(vm, text);
}
static void
inspect_i32 (wasm_exec_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_i32(vm, name, p);
}
static void
inspect_f32 (wasm_exec_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_f32(vm, name, p);
}
static nux_u32_t
console_info (wasm_exec_env_t env, nux_console_info_t info)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_console_info(vm, info);
}
static nux_f32_t
global_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_global_time(vm);
}
static nux_f32_t
delta_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_delta_time(vm);
}
static nux_u32_t
button (wasm_exec_env_t env, nux_u32_t player)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_button(vm, player);
}
static nux_f32_t
axis (wasm_exec_env_t env, nux_u32_t player, nux_axis_t axis)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_axis(vm, player, axis);
}

static NativeSymbol wasm_native_symbols[]
    = { EXPORT_WASM_API_WITH_SIG(trace, "(*)"),
        EXPORT_WASM_API_WITH_SIG(inspect_i32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(inspect_f32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(console_info, "(i)"),
        EXPORT_WASM_API_WITH_SIG(global_time, "()"),
        EXPORT_WASM_API_WITH_SIG(delta_time, "()"),
        EXPORT_WASM_API_WITH_SIG(button, "(i)"),
        EXPORT_WASM_API_WITH_SIG(axis, "(ii)") };