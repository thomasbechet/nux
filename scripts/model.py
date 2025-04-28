from jinja2 import Environment, FileSystemLoader
import argparse
import pywavefront
from texture import compile_texture
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("mesh")
    parser.add_argument("name")
    parser.add_argument("palette")
    args = parser.parse_args()

    scene = pywavefront.Wavefront(args.mesh)
    objects = []

    for name, material in scene.materials.items():
        if not material.texture:
            continue

        # texpath = os.path.dirname(args.mesh) + '/' + material.texture.path
        texpath = material.texture.path.lower()
        texname = args.name + material.texture.name.replace('.', '_').replace('/', '_') 
        pixels, width, height = compile_texture(texname, texpath, args.palette)

        vertex_format = material.vertex_format
        obj = {}
        obj['name'] = name

        positions = []
        uvs = []
        if vertex_format == 'T2F_N3F_V3F':
            for i in range(0, len(material.vertices), 8):
                v = material.vertices[i:i+8]
                positions.append((v[5], v[6], v[7]))
                uvs.append((v[0], v[1]))
        elif vertex_format == 'T2F_V3F':
            for i in range(0, len(material.vertices), 5):
                v = material.vertices[i:i+5]
                positions.append((v[2], v[3], v[4]))
                uvs.append((v[0], v[1]))
        obj['positions'] = positions
        obj['uvs'] = uvs 
        obj['count'] = len(positions)
        obj['tex'] = pixels 
        obj['texw'] = width 
        obj['texh'] = height 

        objects.append(obj)

    env = Environment(loader=FileSystemLoader("templates"))
    template = env.get_template("mesh.inc.c.jinja")
    print(template.render(name=args.name, objects=objects, objects_count=len(objects)))

