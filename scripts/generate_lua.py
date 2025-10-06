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

def normalize_typenme(typename):
    if typename != "void":
        typename = typename[4:-2]
    return typename

def parse_function(node, modules):
    funcname = ""
    typename = {}
    params = []
    if type(node.type) == c_parser.c_ast.PtrDecl: # Return pointer
        funcname = node.type.type.declname
        typename["name"] = normalize_typenme(node.type.type.type.names[0])
        typename["type"] = "resource"
        typename["const"] = True
        if node.args:
            params = node.args.params
    elif type(node.type) == c_parser.c_ast.TypeDecl: # Return value
        funcname = node.type.declname 
        typename["name"] = normalize_typenme(node.type.type.names[0])
        typename["type"] = "primitive"
        typename["const"] = True
        if node.args:
            params = node.args.params
        
    # print(f"{funcname}: {typename}")

    module, name = get_name_and_module(modules, funcname)
    func = {}
    func["name"] = name
    func["typename"] = typename
    func["args"] = []
    for param in params:
        if (isinstance(param, c_ast.EllipsisParam)):
            # Ignore functions with variadics
            return
        arg = {}
        arg["name"] = param.name
        typename = {}
        if type(param.type) is c_ast.PtrDecl:
            typename["name"] = normalize_typenme(param.type.type.type.names[0])
            typename["type"] = "resource" 
            typename["const"] = False
            if typename["name"] == "c8": # Special string case
                typename["type"] = "primitive"
                typename["const"] = True
            if param.quals:
                typename["const"] = True
        else:
            typename["name"] = normalize_typenme(param.type.type.names[0])
            typename["type"] = "primitive"
            typename["const"] = True
        arg["typename"] = typename
        if typename["name"] != "void":
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
        typedef int nux_nid_t;

        typedef int nux_status_t;

        typedef union
        {
            int _;
        } nux_v2_t;
        
        typedef union
        {
            int _;
        } nux_v3_t;
        
        typedef union
        {
            int _;
        } nux_q4_t;

        typedef union
        {
            int _;
        } nux_m4_t;
    """

    # Append resource types
    for typename in [
        'nux_arena_t'
        ]:
        prelude += f"\ntypedef struct {typename} {typename};"

    fixed = "\n".join([line if not re.findall("//|#include|#ifdef|#ifndef|#else|#endif|#define", line) else "" for line in src.splitlines()])
    final = prelude + fixed
    ast = c_parser.CParser().parse(final)
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
        "core/lua/api.h",
        "core/scene/api.h",
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
