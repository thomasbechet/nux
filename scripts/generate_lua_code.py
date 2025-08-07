from template import apply_template
import argparse
import glob
import os

def generate_files(args, path, output):
    code = []
    for file in glob.glob(os.path.join(args.rootdir, path)):
        string = open(file, "r").read()
        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code.encode()]
            return l
        code += tohex(string)
    code.append("0x00") # null terminated
    apply_template(args.rootdir, "lua_code.c.inc.jinja", output, code=code)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    args = parser.parse_args()

    generate_files(args, "core/base/lua/*", "core/base/lua_code.c.inc")
    generate_files(args, "core/ecs/lua/*", "core/ecs/lua_code.c.inc")

