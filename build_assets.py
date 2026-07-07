"""
Build the final atlas and also copy all environment props assets.
"""
import shutil, os
from PIL import Image

TILE = 16
ATLAS_COLS = 25
ATLAS_ROWS = 25
atlas = Image.new("RGBA", (ATLAS_COLS*TILE, ATLAS_ROWS*TILE), (0,0,0,0))

floors  = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Floors_Tiles.png").convert("RGBA")
dungeon = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Dungeon_Tiles.png").convert("RGBA")
water   = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Water_tiles.png").convert("RGBA")

fCols = floors.size[0] // TILE
dCols = dungeon.size[0] // TILE
wCols = water.size[0] // TILE

def paste_sheet_row(src, src_row, dest_row, src_cols):
    for col in range(min(src_cols, ATLAS_COLS)):
        tile = src.crop((col*TILE, src_row*TILE, (col+1)*TILE, (src_row+1)*TILE))
        atlas.paste(tile, (col*TILE, dest_row*TILE))

# paste floors rows 0-9 → atlas rows 0-9
for r in range(min(10, floors.size[1]//TILE)):
    paste_sheet_row(floors, r, r, fCols)

# paste dungeon rows 0-7 → atlas rows 10-17
for r in range(min(8, dungeon.size[1]//TILE)):
    paste_sheet_row(dungeon, r, 10+r, dCols)

# paste water rows 0-4 → atlas rows 18-22
for r in range(min(5, water.size[1]//TILE)):
    paste_sheet_row(water, r, 18+r, wCols)

atlas.save("assets/atlas.png")
print("atlas.png OK:", atlas.size)

base = "Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/"

def cp(src, dst):
    if os.path.exists(src):
        shutil.copy(src, "assets/" + dst)
        img = Image.open(src)
        print(f"  Copied {dst}: {img.size}")
    else:
        print(f"  MISSING: {src}")

print("\nCopying environment props...")
cp(base + "Environment/Props/Static/Trees/Model_01/Size_04.png", "tree1.png")
cp(base + "Environment/Props/Static/Trees/Model_02/Size_04.png", "tree2.png")
cp(base + "Environment/Props/Static/Trees/Model_03/Size_04.png", "tree3.png")
cp(base + "Environment/Props/Static/Rocks.png",                  "rocks.png")
cp(base + "Environment/Props/Static/Vegetation.png",             "vegetation.png")
cp(base + "Environment/Structures/Buildings/Roofs.png",          "roof.png")
cp(base + "Environment/Structures/Buildings/Walls.png",          "walls.png")
cp(base + "Environment/Props/Static/Farm.png",                   "farm.png")
cp(base + "Weapons/Sword.png",                                    "sword.png")
print("Done!")
