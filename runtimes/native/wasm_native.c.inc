#include "core/vm.h"
#include <wasm_export.h>

static void
trace (wasm_exec_env_t env, const nux_c8_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_trace(vm, text);
}
static void
dbgi32 (wasm_exec_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_dbgi32(vm, name, p);
}
static void
dbgf32 (wasm_exec_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_dbgf32(vm, name, p);
}
static nux_u32_t
stat (wasm_exec_env_t env, nux_console_info_t info)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_stat(vm, info);
}
static nux_f32_t
gtime (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_gtime(vm);
}
static nux_f32_t
dtime (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_dtime(vm);
}
static nux_u32_t
frame (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_frame(vm);
}
static void
pal (wasm_exec_env_t env, nux_u8_t index, nux_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_pal(vm, index, color);
}
static void
cls (wasm_exec_env_t env, nux_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_cls(vm, color);
}
static void
fill (wasm_exec_env_t env,
      nux_i32_t       x0,
      nux_i32_t       y0,
      nux_i32_t       x1,
      nux_i32_t       y1,
      nux_u8_t        color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_fill(vm, x0, y0, x1, y1, color);
}
static void
pset (wasm_exec_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_pset(vm, x, y, color);
}
static nux_u8_t
pget (wasm_exec_env_t env, nux_i32_t x, nux_i32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_pget(vm, x, y);
}
static void
line (wasm_exec_env_t env,
      nux_i32_t       x0,
      nux_i32_t       y0,
      nux_i32_t       x1,
      nux_i32_t       y1,
      nux_u8_t        color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_line(vm, x0, y0, x1, y1, color);
}
static void
filltri (wasm_exec_env_t env,
         nux_i32_t       x0,
         nux_i32_t       y0,
         nux_i32_t       x1,
         nux_i32_t       y1,
         nux_i32_t       x2,
         nux_i32_t       y2,
         nux_u8_t        color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_filltri(vm, x0, y0, x1, y1, x2, y2, color);
}
static void
text (wasm_exec_env_t env,
      nux_i32_t       x,
      nux_i32_t       y,
      const nux_c8_t *text,
      nux_u8_t        c)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_text(vm, x, y, text, c);
}
static void
print (wasm_exec_env_t env, const nux_c8_t *text, nux_u8_t c)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_print(vm, text, c);
}
static nux_i32_t
cursorx (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_cursorx(vm);
}
static nux_i32_t
cursory (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_cursory(vm);
}
static void
cursor (wasm_exec_env_t env, nux_i32_t x, nux_i32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_cursor(vm, x, y);
}
static nux_u32_t
btn (wasm_exec_env_t env, nux_u32_t player)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_btn(vm, player);
}
static nux_f32_t
axs (wasm_exec_env_t env, nux_u32_t player, nux_axis_t axis)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_axs(vm, player, axis);
}

static NativeSymbol wasm_native_symbols[]
    = { EXPORT_WASM_API_WITH_SIG(trace, "(*)"),
        EXPORT_WASM_API_WITH_SIG(dbgi32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(dbgf32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(stat, "(i)"),
        EXPORT_WASM_API_WITH_SIG(gtime, "()"),
        EXPORT_WASM_API_WITH_SIG(dtime, "()"),
        EXPORT_WASM_API_WITH_SIG(frame, "()"),
        EXPORT_WASM_API_WITH_SIG(pal, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(cls, "(i)"),
        EXPORT_WASM_API_WITH_SIG(fill, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(pset, "(iii)"),
        EXPORT_WASM_API_WITH_SIG(pget, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(line, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(filltri, "(iiiiiii)"),
        EXPORT_WASM_API_WITH_SIG(text, "(ii*i)"),
        EXPORT_WASM_API_WITH_SIG(print, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(cursorx, "()"),
        EXPORT_WASM_API_WITH_SIG(cursory, "()"),
        EXPORT_WASM_API_WITH_SIG(cursor, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(btn, "(i)"),
        EXPORT_WASM_API_WITH_SIG(axs, "(ii)") };