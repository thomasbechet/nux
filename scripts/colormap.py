from PIL import Image
from jinja2 import Environment, FileSystemLoader
import argparse

def load_palette(path):
    pal = Image.open(path)
    return list(pal.getdata())

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("palette")
    parser.add_argument("name")
    args = parser.parse_args()
    
    pal = Image.open(args.palette)
    pixels = list(pal.getdata())
    
    env = Environment(loader=FileSystemLoader("templates"))
    template = env.get_template("colormap.inc.c.jinja")
    print(template.render(pixels=pixels, name=args.name))
