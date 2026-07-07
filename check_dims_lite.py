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
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\sawmill.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\anvil.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\furnace.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\workbench.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\bonfire.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\player_idle_down.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\Idle_Side-Sheet.png"
]

for f in files:
    if os.path.exists(f):
        info = get_image_info(f)
        print(f"{os.path.basename(f)}: {info}")
    else:
        print(f"{f} not found")
