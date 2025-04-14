#include "internal.h"
#include "nux_api.h"
#include <wasm3.h>

static m3ApiRawFunction(trace)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE trace\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, text);
    nux_trace(env, text);
    m3ApiSuccess();
}
static m3ApiRawFunction(inspect_i32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE inspect_i32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_i32_t *, p);
    nux_inspect_i32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(inspect_f32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE inspect_f32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_f32_t *, p);
    nux_inspect_f32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(console_info)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE console_info\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_console_info_t, info);
    m3ApiReturn(nux_console_info(env, info));
    m3ApiSuccess();
}
static m3ApiRawFunction(global_time)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE global_time\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_global_time(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(delta_time)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE delta_time\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_delta_time(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(button)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE button\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiReturn(nux_button(env, player));
    m3ApiSuccess();
}
static m3ApiRawFunction(axis)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE axis\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiGetArg(nux_axis_t, axis);
    m3ApiReturn(nux_axis(env, player, axis));
    m3ApiSuccess();
}

static void
link_raw_functions (IM3Module module)
{
    m3_LinkRawFunction(module, "*", "trace", "v(*)", trace);
    m3_LinkRawFunction(module, "*", "inspect_i32", "v(**)", inspect_i32);
    m3_LinkRawFunction(module, "*", "inspect_f32", "v(**)", inspect_f32);
    m3_LinkRawFunction(module, "*", "console_info", "i(i)", console_info);
    m3_LinkRawFunction(module, "*", "global_time", "f()", global_time);
    m3_LinkRawFunction(module, "*", "delta_time", "f()", delta_time);
    m3_LinkRawFunction(module, "*", "button", "i(i)", button);
    m3_LinkRawFunction(module, "*", "axis", "f(ii)", axis);
}