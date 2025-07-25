from template import apply_template
import argparse
import glob
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    args = parser.parse_args()

    code = []
    for file in glob.glob(os.path.join(args.rootdir, "core/lua/scripts/*")):
        string = open(file, "r").read()
        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code.encode()]
            return l
        code += tohex(string)
    code.append("0x00") # null terminated
    apply_template(args.rootdir, "lua_ext.c.inc.jinja", "core/lua/lua_ext.c.inc", code=code)

