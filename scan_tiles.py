"""
Build a composite 400×416 terrain atlas that combines:
  - Base grass from Floors_Tiles.png tile[0,0] (16×16 grass)
  - Stone path from Floors_Tiles.png (different row)
  - Water tile from Water_tiles.png
  - Dungeon stone from Dungeon_Tiles.png
The atlas is exported as assets/terrain_atlas.png
Then we print the tile index layout so we know what number = what.
"""

from PIL import Image
import os

TILE = 16

def get_tile(sheet, col, row):
    return sheet.crop((col*TILE, row*TILE, (col+1)*TILE, (row+1)*TILE))

floors  = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Floors_Tiles.png").convert("RGBA")
water   = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Water_tiles.png").convert("RGBA")
dungeon = Image.open("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Environment/Tilesets/Dungeon_Tiles.png").convert("RGBA")

fw, fh = floors.size
print(f"Floors: {fw}x{fh} => {fw//TILE} cols x {fh//TILE} rows")

# Show first few tiles in floors for identification
for r in range(min(5, fh//TILE)):
    row_info = []
    for c in range(min(10, fw//TILE)):
        t = get_tile(floors, c, r)
        pixels = list(t.getdata())
        non_alpha = [(px[0],px[1],px[2]) for px in pixels if len(px)>3 and px[3]>10]
        if non_alpha:
            avg = tuple(sum(x)//len(non_alpha) for x in zip(*non_alpha))
        else:
            avg = (0,0,0)
        row_info.append(f"({r*fw//TILE+c}){avg}")
    print("  Row", r, ":", " ".join(row_info))

print()
# Now build a simple 25-col atlas out of the floors sheet directly 
# We use the first row as grass, row 6 onwards as stone/path
# Print water tile summary
ww, wh = water.size
print(f"Water: {ww}x{wh} => {ww//TILE} cols, rows={wh//TILE}")
for r in range(min(3, wh//TILE)):
    row_info = []
    for c in range(min(8, ww//TILE)):
        t = get_tile(water, c, r)
        pixels = list(t.getdata())
        non_alpha = [(px[0],px[1],px[2]) for px in pixels if len(px)>3 and px[3]>10]
        if non_alpha:
            avg = tuple(sum(x)//len(non_alpha) for x in zip(*non_alpha))
        else:
            avg = (0,0,0)
        row_info.append(f"({r*ww//TILE+c}){avg}")
    print("  Row", r, ":", " ".join(row_info))
