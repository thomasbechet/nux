from PIL import Image, ImagePalette
from jinja2 import Environment, FileSystemLoader
from colormap import load_palette
import argparse

def compile_texture(name, path, palette):
    img = Image.open(path).convert('RGB')
    pal = generate_palette(palette)

    w, h = img.size
    if w > 128 or h > 128:
        img.resize((128, 128))
    
    conv = img.quantize(palette=pal, dither=0)
    # conv = conv.resize((32, 32))
    pixels = list(conv.getdata())
    width, height = conv.size
    
    return pixels, width, height


def generate_palette(path):
    pal = load_palette(path)
    palette = []
    for p in pal:
        palette.extend([p[0], p[1], p[2]])
    for _ in range(256 - len(pal)):
        palette.extend(pal[0][:3])
        # palette.extend([0, 0, 0])
    pimage = Image.new("P", (1, 1), 0)
    pimage.putpalette(palette)
    return pimage

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("palette")
    parser.add_argument("path")
    parser.add_argument("name")
    args = parser.parse_args()

    pixels, width, height = compile_texture(args.name, args.path, args.palette)

    env = Environment(loader=FileSystemLoader("templates"))
    template = env.get_template("texture.inc.c.jinja")
    print(template.render(pixels=pixels, name=name, width=width, height=height))
