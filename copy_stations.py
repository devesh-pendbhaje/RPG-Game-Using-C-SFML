import shutil, os
from PIL import Image

def cp(src, dst):
    src_path = os.path.join("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/", src)
    if os.path.exists(src_path):
        shutil.copy(src_path, "assets_new/" + dst)
        print(f"Copied {dst}")
    else:
        print(f"MISSING: {src_path}")

# Copy Stations (Final Correction)
cp("Environment/Structures/Stations/Anvil/Anvil_01-Sheet.png", "anvil.png")           # 32x32 frames
cp("Environment/Structures/Stations/Furnace/Stone_01-Sheet.png", "furnace.png")      # 32x32 frames
cp("Environment/Structures/Stations/Sawmill/Level_2-Sheet.png", "sawmill.png")       # 64x64 frames
cp("Environment/Structures/Stations/Workbench/Workbench.png", "workbench.png")       # 32x32 frames
cp("Environment/Structures/Stations/Bonfire/Bonfire_01-Sheet.png", "bonfire.png")    # 32x32 frames

# Copy Decorations / Furniture
cp("Environment/Props/Static/Furniture.png", "furniture.png")
cp("Environment/Props/Static/Tools.png", "tools.png")

print("Assets for new stations copied.")
