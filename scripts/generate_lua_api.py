from pycparser import c_ast, c_parser, c_generator, parse_file
import subprocess
import argparse
import shutil
import sys
import re
import os
import json
from template import apply_template

def parse_function(node, modules):
    parts = node.type.declname.split("_", 2)
    if len(parts) < 3: # In core module
        module = "core"
    else:
        module = parts[1]
    if module not in modules.keys():
        modules[module] = []
    func = {}
    # print(node.type)
    func["name"] = parts[-1]
    func["returntype"] = node.type.type.names[0]
    func["args"] = []
    for param in node.args.params[1:]:
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
    modules[module].append(func)

def parse_enum(node, constants):
    constant = {}
    constant["name"] = node.name.replace("nux_", "")
    constant["values"] = []
    for e in node.type.type.values.enumerators:
        val = {}
        val["name"] = e.name.replace("NUX_", "")
        val["value"] = c_generator.CGenerator().visit(e.value)
        constant["values"].append(val)
    constants.append(constant)

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

def parse_header(args, header):
    api_header = os.path.join(args.rootdir, header)
    with open(api_header, 'r') as file:
        src = file.read()

    constants = []
    modules = {}

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
        typedef int nux_res_t;
        typedef int nux_ent_t;
        
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
    """

    fixed = "\n".join([line if not re.findall("//|#include|#ifdef|#ifndef|#else|#endif|#define", line) else "" for line in src.splitlines()])
    ast = c_parser.CParser().parse(prelude + fixed)
    v = FuncDefVisitor(modules)
    v.visit(ast)
    v = TypeDefVisitor(constants)
    v.visit(ast)
    return modules, constants

def generate_files(args, source, api_name, modules, constants):

    # lua_api.c.inc
    apply_template(args.rootdir, "lua_api.c.inc.jinja", source, modules=modules, constants=constants, api_name=api_name)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    parser.add_argument("--dump", dest="dump", action="store_true")
    args = parser.parse_args()

    m, c = parse_header(args, "core/nux_api_base.h")
    generate_files(args, "core/lua/lua_api_base.c", "base", m, c)
    m, c = parse_header(args, "core/nux_api_graphics.h")
    generate_files(args, "core/lua/lua_api_graphics.c", "graphics", m, c)
    m, c = parse_header(args, "core/nux_api_ecs.h")
    generate_files(args, "core/lua/lua_api_ecs.c", "ecs", m, c)
