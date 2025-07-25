from PIL import Image, ImageFont, ImageDraw
from bitarray import bitarray
import subprocess
import string
import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rootdir")
    args = parser.parse_args()

    font_path = os.path.join(args.rootdir, "core/graphics/fonts/PublicPixel.ttf")
    font = ImageFont.truetype(font_path, 8)
    left, top, right, bottom = font.getbbox("A", stroke_width=0)
    w = right - left + 1
    h = bottom - top + 1
    # print('w', w, 'h', h)
    im = Image.new("RGB", (w, h))
    draw = ImageDraw.Draw(im)

    ba = bitarray()

    chars = [0] * 256

    ascii = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    # print(len(ascii))
    for i, c in enumerate(ascii):
        chars[ord(c)] = i
        draw.text((0, 0), c, font=font)
    
        for i in range(w * h):
            coord = (i % w, i // w)
            p = im.getpixel(coord)
            if p[0] > 0:
                ba.append(1)
            else:
                ba.append(0)
    
        draw.rectangle((0, 0, w, h), fill=(0, 0, 0, 0))
    
    output = "core/graphics/fonts_data.c.inc"
    with open(os.path.join(args.rootdir, output), "w") as f:
        print('#include "internal.h"', file=f)
        print("#define DEFAULT_FONT_DATA_WIDTH ", w, file=f);
        print("#define DEFAULT_FONT_DATA_HEIGHT", h, file=f);
        print("static const nux_u8_t default_font_data_chars[] = {", ','.join(['{: 3d}'.format(x) for x in chars]), "};", file=f)
        print("static const nux_u8_t default_font_data[] = {", ','.join(['0x{:02x}'.format(x) for x in ba.tobytes()]), "};", file=f)
        f.close()
        subprocess.call(["clang-format", "-i", output], cwd=args.rootdir)
