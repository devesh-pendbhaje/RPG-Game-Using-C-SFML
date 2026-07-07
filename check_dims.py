from PIL import Image
import sys
import os

files = [
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\sawmill.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\anvil.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\furnace.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\workbench.png",
    r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new\bonfire.png"
]

for f in files:
    if os.path.exists(f):
        img = Image.open(f)
        print(f"{os.path.basename(f)}: {img.size}")
    else:
        print(f"{f} not found")
