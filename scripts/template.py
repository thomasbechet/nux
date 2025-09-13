import os
import subprocess
from jinja2 import Environment, FileSystemLoader

def apply_template(rootdir, template, output, clang_format=True, **kwargs):
    env = Environment(
            loader=FileSystemLoader(os.path.join(rootdir, "scripts/templates")),
            trim_blocks=True,
            lstrip_blocks=True
            )
    template = env.get_template(template)
    r = template.render(kwargs)
    with open(os.path.join(rootdir, output), "w") as f:
        f.write(r)
        f.close()
        # if clang_format:
        #     subprocess.call(["clang-format", "-i", output], cwd=rootdir)
