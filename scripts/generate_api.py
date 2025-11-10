from pycparser import c_ast, c_parser, c_generator, parse_file
import subprocess
import argparse
import shutil
import sys
import re
import os
import json
import glob

def normalize_type(name):
    if name != "void":
        # Remove nux_ and _t
        if name.startswith("nux_") or name.startswith("NUX_"):
            name = name[4:]
        if name.endswith("_t"):
            name = name[:-2]
    return name

def submodule_entry(module, name):
    name = name[4:] # Remove nux_ or NUX_
    splits = name.split("_", 1)
    submodule = splits[0].lower()
    name = splits[1].lower()

    if submodule not in module:
        module[submodule] = {}
        module[submodule]["functions"] = {}
        module[submodule]["constants"] = {}

    return module[submodule], name

def put_function(module, name, func):
    submodule, name = submodule_entry(module, name)
    submodule["functions"][name] = func

def put_constant(module, name, constant):
    submodule, name = submodule_entry(module, name)
    name = name.upper()
    submodule["constants"][name] = constant

def parse_function(node, module):
    funcname = ""
    typename = {}
    params = []

    # Parse return value and function name
    if type(node.type) == c_parser.c_ast.PtrDecl: # Return pointer
        funcname = node.type.type.declname
        typename["name"] = normalize_type(node.type.type.type.names[0])
        typename["type"] = "resource"
        typename["const"] = False
        if node.type.quals:
            typename["const"] = True
        if typename["name"] == "void": # Ignore void* returned value functions
            return
        if node.args:
            params = node.args.params
    elif type(node.type) == c_parser.c_ast.TypeDecl: # Return value
        funcname = node.type.declname 
        typename["name"] = normalize_type(node.type.type.names[0])
        typename["type"] = "primitive"
        typename["const"] = False
        if node.args:
            params = node.args.params

    # Ignore function callback typedefs
    if (funcname.endswith("_t")): 
        return

    func = {}
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
            typename["name"] = normalize_type(param.type.type.type.names[0])
            typename["type"] = "resource" 
            typename["const"] = False
            if typename["name"] == "c8": # Special string case
                typename["type"] = "primitive"
                typename["const"] = True
            if param.quals:
                typename["const"] = True
        else:
            typename["name"] = normalize_type(param.type.type.names[0])
            typename["type"] = "primitive"
            typename["const"] = True

        arg["typename"] = typename

        # Ignore functions with callbacks arguments
        if "callback" in typename["name"]:
            return
        # Ignore functions with void* arguments
        if typename["name"] == "void" and typename["type"] == "resource":
            return
        # Skip void primitive arguments
        if typename["name"] == "void" and typename["type"] == "primitive":
            continue

        func["args"].append(arg)
    put_function(module, funcname, func)

def parse_enum(node, module):
    for e in node.type.type.values.enumerators:
        constant = {}
        constant["value"] = c_generator.CGenerator().visit(e.value)
        put_constant(module, e.name, constant)

class FuncDefVisitor(c_ast.NodeVisitor):
    def __init__(self, module):
        self._module = module

    def visit_FuncDecl(self, node):
        parse_function(node, self._module)

class TypeDefVisitor(c_ast.NodeVisitor):
    def __init__(self, constants):
        self._constants = constants 

    def visit_Typedef(self, node):
        if type(node.type.type) is c_ast.Enum:
            parse_enum(node, self._constants)

def parse_header(args, header, module):
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
        } nux_v2i_t;
        
        typedef union
        {
            int _;
        } nux_v3_t;

        typedef union
        {
            int _;
        } nux_v4_t;
        
        typedef union
        {
            int _;
        } nux_q4_t;

        typedef union
        {
            int _;
        } nux_m4_t;

        typedef union
        {
            int _;
        } nux_b2i_t;
    """

    # Append resource types
    for typename in [
        'nux_arena_t',
        'nux_texture_t',
        'nux_canvas_t',
        'nux_serde_reader_t',
        'nux_serde_writer_t',
        ]:
        prelude += f"\ntypedef struct {typename} {typename};"

    fixed = ""
    inmacro = False
    for line in src.splitlines():
        if line.startswith("#define"):
            inmacro = True
            continue
        if inmacro:
            if not line.endswith("\\"):
                inmacro = False
            continue
        if re.findall("//|#include|#ifdef|#ifndef|#else|#endif|#define|va_list", line):
            continue
        fixed += "\n" + line
    final = prelude + fixed
    ast = c_parser.CParser().parse(final)
    v = FuncDefVisitor(module)
    v.visit(ast)
    v = TypeDefVisitor(module)
    v.visit(ast)

def parse_module(args, header, modules):
    module_name = re.search(".*/(.*).h", header).group(1)
    module = {}
    parse_header(args, header, module)
    modules[module_name] = module

def generate_api(args):
    # Parse C headers
    headers = [
        "core/core/core.h",
        "core/input/input.h",
        "core/lua/lua.h",
        "core/scene/scene.h",
        "core/graphics/graphics.h",
        "core/physics/physics.h",
        "core/ui/ui.h",
    ]
    modules = {}
    for header in headers:
        parse_module(args, header, modules)
    with open(os.path.join(args.rootdir, 'docs/api.json'), 'w') as f:
        json.dump(modules, f, indent=4)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    parser.add_argument("--dump", dest="dump", action="store_true")
    args = parser.parse_args()
    generate_api(args)

