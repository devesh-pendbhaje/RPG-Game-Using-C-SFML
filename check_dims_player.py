import struct
import os

def get_image_info(filename):
    with open(filename, 'rb') as f:
        data = f.read(25)
        if data[:8] == b'\x89PNG\r\n\x1a\n':
            w, h = struct.unpack('>LL', data[16:24])
            return int(w), int(h)
    return None

files = [
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\Walk_Side-Sheet.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\Walk_Down-Sheet.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\Walk_Up-Sheet.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\Slice_Side-Sheet.png"
]

for f in files:
    if os.path.exists(f):
        info = get_image_info(f)
        print(f"{os.path.basename(f)}: {info}")
    else:
        print(f"{f} not found")
