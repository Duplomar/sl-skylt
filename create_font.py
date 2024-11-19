from PIL import ImageDraw, ImageFont, Image
from sys import argv
from pathlib import Path

def bitstring_to_bytes(s):
    return int(s, 2).to_bytes(len(s) // 8 + 1, byteorder='little')


signs = [chr(c) for c in range(32, 127)] + list("ÅÄÖåäö")
size = 20


input_font = Path(argv[1])
fnt = ImageFont.truetype(input_font, size=size)

output = Path("fonts") / f"{input_font.stem}.h"
with output.open("w") as f:
    f.write(f"const unsigned short letter_dimension = {size};\n")
    f.write(f"const unsigned short letter_size = {(size * size)//8 + 1};\n")
    f.write(f"const unsigned short num_letters = {len(signs)};\n")
    f.write("const char _letters[] = {\n")
    for c in signs:
        width = fnt.size
        height = fnt.size
        im = Image.new("1", (width, height), 0)
        draw = ImageDraw.Draw(im)
        draw.text((0, 0), c, 1, font=fnt)

        im = im.rotate(180)

        byte_string = bitstring_to_bytes("".join([str(int(bit)) for bit in im.getdata()]))
        c_byte_array = ", ".join(["0x" + byte for byte in byte_string.hex(":").split(":")])
        if c != signs[-1]:
            c_byte_array += ", "
        c_byte_array += "\n"

        f.write(c_byte_array)
    f.write("};")
        
        
        