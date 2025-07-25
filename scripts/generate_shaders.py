import subprocess
import string
import argparse
import os
import sys
import glob

from template import apply_template

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    parser.add_argument("--dump", dest="dump", action="store_true")
    args = parser.parse_args()

    shaders = []
    sources = [
        "core/graphics/shaders/uber.vert",
        "core/graphics/shaders/uber.frag",
        "core/graphics/shaders/canvas.vert",
        "core/graphics/shaders/canvas.frag",
        "core/graphics/shaders/blit.vert",
        "core/graphics/shaders/blit.frag",
    ]
    for source in sources:
        file = os.path.join(args.rootdir, source)
        name = os.path.basename(file).replace(".", "_")

        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code.encode()]
            l.append("0x00") # null terminated
            return l

        with open(file, 'r') as f:
            shader = {}
            shader['name'] = name
            shader['code'] = tohex(f.read())
            shaders.append(shader)

    apply_template(args.rootdir, "shaders_data.c.inc.jinja", "core/graphics/shaders_data.c.inc", shaders=shaders)
