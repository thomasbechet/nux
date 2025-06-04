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
    slang_files = [
        "core/shaders/main.slang",
        "core/shaders/canvas.slang",
    ]
    for slang_file in slang_files:
        file = os.path.join(args.rootdir, slang_file)
        name = os.path.splitext(os.path.basename(file))[0]
        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code.encode()]
            l.append("0x00") # null terminated
            return l
        def compile_to_glsl(file, entry, ext):
            spirv_output = os.path.join(args.rootdir, "core/shaders/", name + ext + ".spv")
            glsl_output = os.path.join(args.rootdir, "core/shaders/", name + ext)
            subprocess.call([
                "slangc", file, 
                "-target", "spirv",
                "-entry", entry,
                "-o", spirv_output 
                ])
            subprocess.call([
                "spirv-cross", spirv_output,
                "--output", glsl_output,
                ])
            with open(glsl_output, 'r') as file:
                return file.read()

        shader = {}
        shader['name'] = name
        shader['vertex'] = tohex(compile_to_glsl(file, "vertexMain", ".vert"))
        shader['fragment'] = tohex(compile_to_glsl(file, "fragmentMain", ".frag"))
        shaders.append(shader)

    apply_template(args.rootdir, "shaders_data.c.inc.jinja", "core/shaders_data.c.inc", shaders=shaders)
