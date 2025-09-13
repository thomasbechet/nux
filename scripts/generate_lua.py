from pycparser import c_ast, c_parser, c_generator, parse_file
import subprocess
import argparse
import shutil
import sys
import re
import os
import json
import glob
from template import apply_template

def get_name_and_module(modules, fullname):
    parts = fullname.split("_", 2)
    if len(parts) < 3: # In core module
        module_name = "core"
    else:
        module_name = parts[1].lower()
    if module_name not in modules.keys():
        module = {}
        module["functions"] = []
        module["constants"] = []
        modules[module_name] = module
    return modules[module_name], parts[-1]

def parse_function(node, modules):
    if type(node.type) == c_parser.c_ast.PtrDecl:
        params = node.args.params
        node = node.type
    else:
        params = node.args.params
    module, name = get_name_and_module(modules, node.type.declname)
    func = {}
    # print(node.type)
    func["name"] = name
    # Return string type
    if node.type.type.names[0] == "nux_c8_t" and node.type.quals[0] == "const":
        func["returntype"] = "const nux_c8_t*"
    else:
        func["returntype"] = node.type.type.names[0]
    func["args"] = []
    for param in params[1:]:
        if (isinstance(param, c_ast.EllipsisParam)):
            # Ignore functions with variadics
            return
        arg = {}
        arg["name"] = param.name
        if type(param.type) is c_ast.PtrDecl:
            arg["isptr"] = True
            if param.quals:
                arg["isconst"] = True
            arg["typename"] = param.type.type.type.names[0]
        else:
            arg["typename"] = param.type.type.names[0]
            arg["isptr"] = False
        func["args"].append(arg)
    module["functions"].append(func)

def parse_enum(node, modules):
    for e in node.type.type.values.enumerators:
        module, name = get_name_and_module(modules, e.name)
        constant = {}
        constant["name"] = name
        constant["value"] = c_generator.CGenerator().visit(e.value)
        module["constants"].append(constant)

class FuncDefVisitor(c_ast.NodeVisitor):
    def __init__(self, modules):
        self._modules = modules

    def visit_FuncDecl(self, node):
        parse_function(node, self._modules)

class TypeDefVisitor(c_ast.NodeVisitor):
    def __init__(self, constants):
        self._constants = constants 

    def visit_Typedef(self, node):
        if type(node.type.type) is c_ast.Enum:
            parse_enum(node, self._constants)

def parse_header(args, header, modules):
    api_header = os.path.join(args.rootdir, header)
    with open(api_header, 'r') as file:
        src = file.read()

    prelude = """
        typedef int uint8_t;
        typedef int int16_t;
        typedef int uint16_t;
        typedef int int32_t;
        typedef int uint32_t;
        typedef int int64_t;
        typedef int uint64_t;
        typedef int intptr_t;

        typedef int nux_b32_t;
        typedef int nux_u8_t;
        typedef int nux_c8_t;
        typedef int nux_i16_t;
        typedef int nux_u16_t;
        typedef int nux_i32_t;
        typedef int nux_u32_t;
        typedef int nux_i64_t;
        typedef int nux_u64_t;
        typedef int nux_f32_t;
        typedef int nux_f64_t;
        typedef int nux_intptr_t;
        typedef int nux_rid_t;
        typedef int nux_eid_t;

        typedef int nux_status_t;
        
        typedef struct nux_context nux_ctx_t;
        
        typedef union
        {
            struct
            {
                nux_f32_t x;
                nux_f32_t y;
                nux_f32_t z;
            };
            nux_f32_t data[3];
        } nux_v3_t;
        
        typedef union
        {
            struct
            {
                nux_f32_t x;
                nux_f32_t y;
                nux_f32_t z;
                nux_f32_t w;
            };
            nux_f32_t data[4];
        } nux_q4_t;

        typedef union
        {
            struct
            {
                nux_f32_t x1;
                nux_f32_t x2;
                nux_f32_t x3;
                nux_f32_t x4;
                nux_f32_t y1;
                nux_f32_t y2;
                nux_f32_t y3;
                nux_f32_t y4;
                nux_f32_t z1;
                nux_f32_t z2;
                nux_f32_t z3;
                nux_f32_t z4;
                nux_f32_t w1;
                nux_f32_t w2;
                nux_f32_t w3;
                nux_f32_t w4;
            };
            nux_f32_t data[16];
        } nux_m4_t;
    """

    fixed = "\n".join([line if not re.findall("//|#include|#ifdef|#ifndef|#else|#endif|#define", line) else "" for line in src.splitlines()])
    ast = c_parser.CParser().parse(prelude + fixed)
    v = FuncDefVisitor(modules)
    v.visit(ast)
    v = TypeDefVisitor(modules)
    v.visit(ast)

def generate_lua_code(args, path):
    code = []
    for file in glob.glob(os.path.join(args.rootdir, path)):
        string = open(file, "r").read()
        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code.encode()]
            return l
        code += tohex(string)
    code.append("0x00") # null terminated
    return code

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    parser.add_argument("--dump", dest="dump", action="store_true")
    args = parser.parse_args()

    # Parse C headers
    headers = [
        "core/base/api.h",
        "core/io/api.h",
        "core/lua/api.h",
        "core/ecs/api.h",
        "core/graphics/api.h",
        "core/physics/api.h",
    ]
    modules = {}
    for header in headers:
        parse_header(args, header, modules)
    # Generate lua_bindings.c
    apply_template(args.rootdir, "lua_bindings.c.jinja", "core/lua/lua_bindings.c", clang_format=True, modules=modules)
    # Generate lua embedded code
    code = generate_lua_code(args, "core/lua/embedded/*")
    apply_template(args.rootdir, "lua_code.c.inc.jinja", "core/lua/lua_code.c.inc", clang_format=True, code=code)
    # Generate lua annotations for LSP
    apply_template(args.rootdir, "lua_api.lua.jinja", "core/lua/lsp/api.lua", modules=modules)
