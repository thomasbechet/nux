import os
import glob
import argparse

def process_shader(shader, name):
    string = "static const nu_sv_t shader_{} = NU_SV(\n".format(name)
    with open(shader, "r") as f:
        for line in f.read().splitlines():
            string += "\"{}\\n\"\n".format(line)
    string += ");\n"
    return string

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    args = parser.parse_args()

    # Generate shader string
    string = "#include <nulib/nulib.h>\n"
    shaders = glob.glob(os.path.join(args.rootdir, "runtimes/native/shaders/*"))

    for shader in shaders:
        name = os.path.basename(shader).replace(".", "_")
        string += process_shader(shader, name)
    uber = os.path.join(args.rootdir, "core/shaders/uber.shader")
    string += process_shader(uber, "uber_vert").replace("VERTEX_SHADER", "1").replace("FRAGMENT_SHADER", "0")
    string += process_shader(uber, "uber_frag").replace("VERTEX_SHADER", "0").replace("FRAGMENT_SHADER", "1")

    # Write output file
    output = os.path.join(args.rootdir, "runtimes/native/shaders_data.c.inc")
    with open(output, "w") as f:
        f.write(string)
        f.close()

