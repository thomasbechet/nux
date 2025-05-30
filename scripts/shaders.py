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
    args = parser.parse_args()

    shaders = []
    for file in list(glob.glob(os.path.join(args.rootdir, "core/shaders/*.slang"))):
        name = os.path.splitext(os.path.basename(file))[0]
        def tohex(code):
            l = ["0x{:02x}".format(x) for x in code]
            l.append("0x00") # null terminated
            return l
        def compile_to_glsl(file, entry):
            ps = subprocess.Popen([
                "slangc", file, 
                "-target", "spirv",
                "-entry", entry
                ], stdout=subprocess.PIPE)
            output = subprocess.check_output(['spirv-cross', '-'], stdin=ps.stdout)
            ps.wait()
            return output

        shader = {}
        shader['name'] = name
        shader['vertex'] = tohex(compile_to_glsl(file, "vertexMain"))
        shader['fragment'] = tohex(compile_to_glsl(file, "fragmentMain"))
        shaders.append(shader)

    apply_template(args.rootdir, "shaders_data.c.inc.jinja", "core/shaders_data.c.inc", shaders=shaders)
