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
from generate_api import generate_api

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

    generate_api(args)

    with open(os.path.join(args.rootdir, "docs/api.json")) as f:
        modules = json.load(f)

    api = {}
    api["functions"] = {}
    api["constants"] = {}
    # Flatten modules
    for v in modules.values():
        api["functions"] |= v["functions"]
        api["constants"] |= v["constants"]
    # Filter functions
    for name in [
            "register_module",
            "register_resource",
            "register_component",
            "new_event",
            "unsubscribe_event",
            "event_handler_event",
            ]:
        del api["functions"][name]

    # Generate lua_bindings.c
    apply_template(args.rootdir, "lua_bindings.c.jinja", "core/lua/lua_bindings.c", clang_format=True, api=api)
    # # Generate lua embedded code
    # code = generate_lua_code(args, "core/lua/embedded/*")
    # apply_template(args.rootdir, "lua_code.c.inc.jinja", "core/lua/lua_code.c.inc", clang_format=True, code=code)
    # # Generate lua annotations for LSP
    # apply_template(args.rootdir, "lua_api.lua.jinja", "core/lua/lsp/api.lua", modules=modules)
