from pycparser import c_ast, c_parser, c_generator, parse_file
import subprocess
import argparse
import shutil
import sys
import re
import os
import json
from template import apply_template

constants = []
modules = {}

def parse_function(node):
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

def parse_enum(node):
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
    def visit_FuncDecl(self, node):
        parse_function(node)

class TypeDefVisitor(c_ast.NodeVisitor):
    def visit_Typedef(self, node):
        if type(node.type.type) is c_ast.Enum:
            parse_enum(node)

def parse_header(args):
    api_header = os.path.join(args.rootdir, "core/nux_api.h")
    with open(api_header, 'r') as file:
        src = file.read()

    prelude = """
    typedef char nux_c8_t;\n
    typedef int nux_i32_t;\n
    typedef unsigned int nux_u32_t;\n
    typedef float nux_f32_t;\n
    typedef int nux_status_t;\n
    """

    fixed = "\n".join([line if not re.findall("//|#include|#ifdef|#ifndef|#else|#endif|#define", line) else "" for line in src.splitlines()])
    ast = c_parser.CParser().parse(prelude + fixed)
    v = FuncDefVisitor()
    v.visit(ast)
    v = TypeDefVisitor()
    v.visit(ast)

def dump():
    print(json.dumps(constants, indent=4))
    print(json.dumps(modules, indent=4))

def generate_files(args):

    # lua_api.c.inc
    apply_template(args.rootdir, "lua_api.c.inc.jinja", "core/lua_api.c.inc", modules=modules, constants=constants)

    # # nux.h
    # template = env.get_template("nux.h.jinja")
    # r = template.render(functions=functions, enums=enums)
    # for output in ["sdk/templates/c/src/nux.h", "sdk/templates/cxx/src/nux.h"]:
    #     with open(os.path.join(args.rootdir, output), "w") as f:
    #         f.write(r)
    #         f.close()
    #         subprocess.call(["clang-format", "-i", output], cwd=args.rootdir)
    #
    # # wasm_native.h
    # template = env.get_template("wasm_native.h.jinja")
    # r = template.render(functions=functions, enums=enums)
    # output = "runtimes/native/wasm_native.c.inc"
    # with open(os.path.join(args.rootdir, output), "w") as f:
    #     f.write(r)
    #     f.close()
    #     subprocess.call(["clang-format", "-i", output], cwd=args.rootdir)
    #
    # # wasm3_native.h
    # template = env.get_template("wasm3_native.h.jinja")
    # r = template.render(functions=functions, enums=enums)
    # output = "core/wasm3_native.c.inc"
    # with open(os.path.join(args.rootdir, output), "w") as f:
    #     f.write(r)
    #     f.close()
    #     subprocess.call(["clang-format", "-i", output], cwd=args.rootdir)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    parser.add_argument("--dump", dest="dump", action="store_true")
    args = parser.parse_args()

    parse_header(args)

    if args.dump:
        dump()
    else:
        generate_files(args)


    

