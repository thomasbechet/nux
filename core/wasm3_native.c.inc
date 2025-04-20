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
static m3ApiRawFunction(dbgi32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE dbgi32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_i32_t *, p);
    nux_dbgi32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(dbgf32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE dbgf32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_f32_t *, p);
    nux_dbgf32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(stat)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE stat\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_stat_t, info);
    m3ApiReturn(nux_stat(env, info));
    m3ApiSuccess();
}
static m3ApiRawFunction(gtime)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE gtime\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_time(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(dtime)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE dtime\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_dt(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(frame)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE frame\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiReturn(nux_frame(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(pal)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE pal\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u8_t, index);
    m3ApiGetArg(nux_u32_t, color);
    nux_pal(env, index, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(cls)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE cls\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, color);
    nux_cls(env, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(fill)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE fill\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x0);
    m3ApiGetArg(nux_i32_t, y0);
    m3ApiGetArg(nux_i32_t, x1);
    m3ApiGetArg(nux_i32_t, y1);
    m3ApiGetArg(nux_u8_t, color);
    nux_fill(env, x0, y0, x1, y1, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(pset)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE pset\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x);
    m3ApiGetArg(nux_i32_t, y);
    m3ApiGetArg(nux_u8_t, color);
    nux_pset(env, x, y, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(pget)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE pget\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u8_t);
    m3ApiGetArg(nux_i32_t, x);
    m3ApiGetArg(nux_i32_t, y);
    m3ApiReturn(nux_pget(env, x, y));
    m3ApiSuccess();
}
static m3ApiRawFunction(line)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE line\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x0);
    m3ApiGetArg(nux_i32_t, y0);
    m3ApiGetArg(nux_i32_t, x1);
    m3ApiGetArg(nux_i32_t, y1);
    m3ApiGetArg(nux_u8_t, color);
    nux_line(env, x0, y0, x1, y1, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(filltri)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE filltri\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x0);
    m3ApiGetArg(nux_i32_t, y0);
    m3ApiGetArg(nux_i32_t, x1);
    m3ApiGetArg(nux_i32_t, y1);
    m3ApiGetArg(nux_i32_t, x2);
    m3ApiGetArg(nux_i32_t, y2);
    m3ApiGetArg(nux_u8_t, color);
    nux_filltri(env, x0, y0, x1, y1, x2, y2, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(text)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE text\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x);
    m3ApiGetArg(nux_i32_t, y);
    m3ApiGetArgMem(const nux_c8_t *, text);
    m3ApiGetArg(nux_u8_t, c);
    nux_text(env, x, y, text, c);
    m3ApiSuccess();
}
static m3ApiRawFunction(print)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE print\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, text);
    m3ApiGetArg(nux_u8_t, c);
    nux_print(env, text, c);
    m3ApiSuccess();
}
static m3ApiRawFunction(cursorx)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE cursorx\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_i32_t);
    m3ApiReturn(nux_cursorx(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(cursory)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE cursory\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_i32_t);
    m3ApiReturn(nux_cursory(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(cursor)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE cursor\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_i32_t, x);
    m3ApiGetArg(nux_i32_t, y);
    nux_cursor(env, x, y);
    m3ApiSuccess();
}
static m3ApiRawFunction(btn)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE btn\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiReturn(nux_btn(env, player));
    m3ApiSuccess();
}
static m3ApiRawFunction(axs)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE axs\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiGetArg(nux_axis_t, axis);
    m3ApiReturn(nux_axs(env, player, axis));
    m3ApiSuccess();
}

static void
link_raw_functions (IM3Module module)
{
    m3_LinkRawFunction(module, "*", "trace", "v(*)", trace);
    m3_LinkRawFunction(module, "*", "dbgi32", "v(**)", dbgi32);
    m3_LinkRawFunction(module, "*", "dbgf32", "v(**)", dbgf32);
    m3_LinkRawFunction(module, "*", "stat", "i(i)", stat);
    m3_LinkRawFunction(module, "*", "gtime", "f()", gtime);
    m3_LinkRawFunction(module, "*", "dtime", "f()", dtime);
    m3_LinkRawFunction(module, "*", "frame", "i()", frame);
    m3_LinkRawFunction(module, "*", "pal", "v(ii)", pal);
    m3_LinkRawFunction(module, "*", "cls", "v(i)", cls);
    m3_LinkRawFunction(module, "*", "fill", "v(iiiii)", fill);
    m3_LinkRawFunction(module, "*", "pset", "v(iii)", pset);
    m3_LinkRawFunction(module, "*", "pget", "(ii)", pget);
    m3_LinkRawFunction(module, "*", "line", "v(iiiii)", line);
    m3_LinkRawFunction(module, "*", "filltri", "v(iiiiiii)", filltri);
    m3_LinkRawFunction(module, "*", "text", "v(ii*i)", text);
    m3_LinkRawFunction(module, "*", "print", "v(*i)", print);
    m3_LinkRawFunction(module, "*", "cursorx", "i()", cursorx);
    m3_LinkRawFunction(module, "*", "cursory", "i()", cursory);
    m3_LinkRawFunction(module, "*", "cursor", "v(ii)", cursor);
    m3_LinkRawFunction(module, "*", "btn", "i(i)", btn);
    m3_LinkRawFunction(module, "*", "axs", "f(ii)", axs);
}
