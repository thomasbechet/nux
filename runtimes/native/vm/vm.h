#ifndef NU_VM_H
#define NU_VM_H

#include <wasm_export.h>

typedef struct
{
    wasm_module_t        module;
    wasm_module_inst_t   instance;
    wasm_exec_env_t      env;
    wasm_function_inst_t start_callback;
    wasm_function_inst_t update_callback;
} nu_vm_t;

#endif
