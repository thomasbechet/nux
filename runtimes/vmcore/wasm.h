#ifndef NU_WASM_H
#define NU_WASM_H

#include <nulib.h>
#include <wasm_export.h>

typedef struct
{
    wasm_module_t        module;
    wasm_module_inst_t   instance;
    wasm_exec_env_t      env;
    wasm_function_inst_t start_callback;
    wasm_function_inst_t update_callback;
} nux_wasm_t;

typedef struct
{
    void     *heap;
    nu_size_t heap_size;
    void     *main_module;
    nu_size_t main_module_size;
} nux_wasm_info_t;

void nux_wasm_init(nux_wasm_t *wasm, const nux_wasm_info_t *info);
void nux_wasm_free(nux_wasm_t *wasm);

#endif
