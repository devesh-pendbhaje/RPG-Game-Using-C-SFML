"""
Build a custom 400x400 terrain atlas (25x25 tiles of 16px each) used as the sole
tileset for the game. Layout:

Row 0: rows 0–3 copied from Floors_Tiles.png  (grass tiles) 
Row 4: rows 4–7 from Floors_Tiles.png (dirt/stone path)
Row 8: rows 0–3 from Dungeon_Tiles.png (dungeon stone)
Row 12: rows 0–3 from Water_tiles.png (water)

Then we pick specific tile indices for Game.h map generation.
"""

from PIL import Image
import os

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

# --- paste floors rows 0-9 into atlas rows 0-9 ---
for r in range(min(10, floors.size[1]//TILE)):
    paste_sheet_row(floors, r, r, fCols)

# --- paste dungeon rows 0-7 into atlas rows 10-17 ---
for r in range(min(8, dungeon.size[1]//TILE)):
    paste_sheet_row(dungeon, r, 10+r, dCols)

# --- paste water rows 0-4 into atlas rows 18-22 ---
for r in range(min(5, water.size[1]//TILE)):
    paste_sheet_row(water, r, 18+r, wCols)

atlas.save("assets/atlas.png")
print("atlas.png created:", atlas.size)

# Print what tile indices are what
print()
print("=== KEY INDICES ===")
print("Grass tile 0-row0: index  0 (first grass tile)")
print("Stone path row4  : index 100 (row 4 col 0 = 4*25+0)")
print("Dungeon stone r10: index 250 (row 10 col 0)")
print("Water row18      : index 450 (row 18 col 0)")

# Show average colours of key tiles
def avg_color(img, col, row, cols):
    t = img.crop((col*TILE, row*TILE, (col+1)*TILE, (row+1)*TILE))
    px = [p for p in t.getdata() if len(p)>3 and p[3]>10]
    if not px: return (0,0,0)
    return tuple(sum(c)//len(px) for c in zip(*px))[:3]

print()
for r in range(5):
    row_info = []
    for c in range(min(12, fCols)):
        idx = r*ATLAS_COLS + c
        col_v = avg_color(floors, c, r, fCols)
        row_info.append(f"{idx}:{col_v}")
    print(f"Floor row {r}: {row_info}")
