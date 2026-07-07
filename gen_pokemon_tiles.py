"""
Generate a Pokemon Ruby GBA-style tileset + map data for the C++ game.
Produces assets/pokemon_tiles.png and prints a C++ map array.

Tile size: 16x16 px
Atlas: 16 tiles wide
"""
from PIL import Image, ImageDraw
import struct

# ─────────────────────────────────────────────────────────────
# Pokemon Ruby palette (GBA circa 2003)
# ─────────────────────────────────────────────────────────────
GRASS1   = (120, 200,  56)   # bright GBA green
GRASS2   = (104, 184,  40)   # mid green
GRASS3   = (136, 216,  72)   # light green
TALLGRASS= ( 64, 136,   8)   # tall grass dark
TALL_TIP = ( 80, 168,  16)   # tall grass tips
PATH1    = (232, 200, 112)   # sandy beige path
PATH2    = (216, 184,  96)   # path shadow
PATH_EDGE= (192, 168,  80)   # path edge
WATER1   = ( 56, 152, 240)   # bright pokemon blue
WATER2   = (  8, 120, 216)   # water deep
WATER_SP = (168, 224, 248)   # sparkle / highlight
CLIFF_T  = (192, 160,  48)   # cliff top face
CLIFF_B  = (224, 192,  80)   # cliff top surface (path-like)
CLIFF_S  = (144, 120,  24)   # cliff shadow
TREE_TOP = ( 32, 104,   0)   # dark tree canopy
TREE_MID = ( 56, 136,  16)   # tree canopy lighter
TREE_EDGE= ( 16,  72,   0)   # tree edge dark
TREE_TRK = (136,  96,  24)   # trunk brown
TRUNK_LT = (168, 120,  40)
FLOWER_R = (240,  32,  32)   # red flower
FLOWER_Y = (248, 224,   8)   # yellow flower
FLOWER_W = (248, 248, 240)   # white flower
FLOWER_G = ( 48, 136,  16)   # stem green
HOUSE_WALL=(248, 224, 160)   # house wall cream
HOUSE_W2 = (216, 192, 128)   # house wall shadow
HOUSE_RF = (208,  48,  48)   # house roof red
HOUSE_R2 = (160,  24,  24)   # roof shadow
HOUSE_WN = (168, 216, 248)   # window blue
BORDER   = ( 24,  40,  16)   # dark world border
BLACK    = (  0,   0,   0)
WHITE    = (255, 255, 255)
SAND     = (240, 208, 128)
SIGN_POST= (168, 128,  56)
DIRT     = (192, 152,  88)

TILE = 16

def new_tile(base):
    img = Image.new("RGBA", (TILE, TILE), base + (255,))
    return img

def rgba(col, a=255):
    if len(col) == 4: return col
    return col + (a,)

def draw_tile(idx):
    """Return a 16×16 RGBA tile image for the given index."""
    img = Image.new("RGBA", (TILE, TILE), (0,0,0,0))
    d   = ImageDraw.Draw(img)

    def px(x,y,col,a=255):
        if 0 <= x < TILE and 0 <= y < TILE:
            img.putpixel((x,y), rgba(col,a))
    def rect(x0,y0,x1,y1,col,a=255):
        d.rectangle([x0,y0,x1,y1], fill=rgba(col,a))
    def fill(col):
        rect(0,0,15,15,col)
    def hline(y,x0,x1,col,a=255):
        for x in range(max(0,x0),min(TILE,x1+1)): px(x,y,col,a)
    def vline(x,y0,y1,col,a=255):
        for y in range(max(0,y0),min(TILE,y1+1)): px(x,y,col,a)

    # ── 0  BORDER / BLACK ─────────────────────────────────────
    if idx == 0:
        fill(BORDER)
        for i in range(0,16,4):
            px(i,i, (30,50,20))

    # ── 1  GRASS standard ─────────────────────────────────────
    elif idx == 1:
        fill(GRASS1)
        for pts in [(2,2),(8,5),(14,2),(5,10),(11,12),(3,14)]:
            px(*pts, GRASS2)
        for pts in [(4,4),(10,3),(6,13),(13,8)]:
            px(*pts, GRASS3)

    # ── 2  GRASS variant (lighter spots) ──────────────────────
    elif idx == 2:
        fill(GRASS2)
        for pts in [(1,1),(7,4),(13,1),(4,9),(10,11),(0,13)]:
            px(*pts, GRASS1)
        for pts in [(3,7),(9,2),(12,9)]:
            px(*pts, GRASS3)

    # ── 3  GRASS with tiny flowers ────────────────────────────
    elif idx == 3:
        fill(GRASS1)
        # red dot flower
        px(3,3, FLOWER_R); px(3,4, FLOWER_G)
        # yellow dot
        px(11,11, FLOWER_Y); px(11,12, FLOWER_G)
        # white dot
        px(7,7, FLOWER_W)
        for pts in [(1,9),(5,5),(13,3),(9,13)]:
            px(*pts, GRASS2)

    # ── 4  TALL GRASS ─────────────────────────────────────────
    elif idx == 4:
        fill(TALLGRASS)
        # wavy tops (blades of grass)
        for x in range(0,16,2):
            h = 5 + (x%3)
            for y in range(0, h):
                px(x, y, TALL_TIP)
            for y in range(h, 16):
                px(x, y, TALLGRASS)
        # some lighter marks
        for pts in [(1,8),(5,6),(9,7),(13,5)]:
            px(*pts, (80,168,24))

    # ── 5  SANDY PATH center ──────────────────────────────────
    elif idx == 5:
        fill(PATH1)
        # subtle stone texture
        for pts in [(2,3),(6,11),(10,5),(14,13),(3,9),(8,2),(12,7)]:
            rect(pts[0], pts[1], pts[0]+1, pts[1]+1, PATH2)

    # ── 6  PATH left edge (grass→path) ────────────────────────
    elif idx == 6:
        fill(GRASS1)
        rect(8,0,15,15, PATH1)
        # Transition pixels
        for y in range(16):
            px(8,y, PATH_EDGE)

    # ── 7  PATH right edge (path→grass) ───────────────────────
    elif idx == 7:
        fill(PATH1)
        rect(8,0,15,15, GRASS1)
        for y in range(16):
            px(7,y, PATH_EDGE)

    # ── 8  WATER deep ─────────────────────────────────────────
    elif idx == 8:
        fill(WATER2)
        for pts in [(2,2),(6,8),(10,4),(14,10),(4,13),(12,1)]:
            rect(pts[0],pts[1],pts[0]+1,pts[1]+1, WATER1)

    # ── 9  WATER sparkle ──────────────────────────────────────
    elif idx == 9:
        fill(WATER1)
        for pts in [(3,3),(11,5),(7,11),(5,8),(13,13)]:
            px(*pts, WATER_SP)
        for pts in [(1,12),(9,1),(15,7)]:
            rect(pts[0],pts[1],pts[0]+1,pts[1]+1, WATER2)

    # ── 10 WATER top-edge (grass meets water) ─────────────────
    elif idx == 10:
        fill(GRASS1)
        rect(0,12,15,15, WATER1)
        hline(11,0,15, WATER_SP)
        for x in range(16):
            if x%3==0: px(x,12, WATER_SP)

    # ── 11 WATER bottom-edge ──────────────────────────────────
    elif idx == 11:
        fill(WATER1)
        rect(0,0,15,3, GRASS1)
        hline(4,0,15, WATER_SP)
        for x in range(16):
            if x%3==0: px(x,3, GRASS2)

    # ── 12 CLIFF top surface ──────────────────────────────────
    elif idx == 12:
        fill(CLIFF_B)
        hline(15,0,15, CLIFF_T)
        for pts in [(3,5),(9,3),(6,10),(12,8)]:
            px(*pts, PATH2)

    # ── 13 CLIFF face ─────────────────────────────────────────
    elif idx == 13:
        fill(CLIFF_T)
        hline(0,0,15, CLIFF_B)
        hline(15,0,15, CLIFF_S)
        for y in range(1,15):
            for x in range(0,16,4):
                px(x+(y%2)*2, y, CLIFF_S)

    # ── 14 TREE canopy top ────────────────────────────────────
    elif idx == 14:
        fill(TREE_TOP)
        # Rounded top
        for x in [0,1,14,15]:
            px(x,0, rgba(TREE_TOP,0))
            px(x,1, rgba(TREE_TOP,0))
        px(0,2,rgba(TREE_TOP,0)); px(15,2,rgba(TREE_TOP,0))
        # lighter centre
        rect(3,3,12,10, TREE_MID)
        # edge shading
        for x in range(16):
            if x<2 or x>13: continue
            px(x,0, TREE_EDGE)
        for y in range(5,11,2):
            px(2,y, TREE_EDGE); px(13,y, TREE_EDGE)
        # highlight top
        px(6,3, (88,176,48)); px(9,3,(88,176,48))

    # ── 15 TREE canopy bottom / trunk ─────────────────────────
    elif idx == 15:
        fill(TREE_MID)
        # bottom shading
        rect(0,10,15,15, TREE_EDGE)
        # trunk
        rect(5,10,10,15, TREE_TRK)
        px(5,10, TREE_EDGE); px(10,10, TREE_EDGE)
        # trunk highlight
        vline(6,11,14, TRUNK_LT)

    # ── 16 HOUSE WALL ─────────────────────────────────────────
    elif idx == 16:
        fill(HOUSE_WALL)
        # Wood plank lines
        for y in range(4,16,4):
            hline(y,0,15, HOUSE_W2)
        # Left shadow
        vline(0,0,15, HOUSE_W2)

    # ── 17 HOUSE ROOF top ─────────────────────────────────────
    elif idx == 17:
        fill(HOUSE_RF)
        # Ridge line at top
        hline(0,0,15, (224,64,64))
        # Shadow bottom
        hline(15,0,15, HOUSE_R2)
        # Shingle lines
        for y in range(4,15,4):
            for x in range(0,16,3):
                px(x+y%3,y, HOUSE_R2)

    # ── 18 HOUSE ROOF bottom / eave ───────────────────────────
    elif idx == 18:
        fill(HOUSE_RF)
        hline(0,0,15,(224,64,64))
        hline(14,0,15, HOUSE_R2)
        hline(15,0,15,(80,20,20))
        for y in range(0,14,3):
            for x in range(0,16,3):
                px(x+(y//3)%3,y, HOUSE_R2)

    # ── 19 HOUSE WINDOW ───────────────────────────────────────
    elif idx == 19:
        fill(HOUSE_WALL)
        rect(3,3,12,12, HOUSE_WN)
        rect(4,4,11,11, (200,232,255))
        # cross divider
        hline(7,3,12, HOUSE_W2)
        vline(7,3,12, HOUSE_W2)
        # frame
        for x in range(3,13): px(x,3,HOUSE_W2); px(x,12,HOUSE_W2)
        for y in range(3,13): px(3,y,HOUSE_W2); px(12,y,HOUSE_W2)

    # ── 20 HOUSE DOOR ─────────────────────────────────────────
    elif idx == 20:
        fill(HOUSE_WALL)
        rect(4,4,11,15, (160,96,40))   # brown door
        rect(5,5,10,14, (192,128,64))  # door highlight
        px(10,9, (248,200,120))        # knob
        hline(4,4,11, (128,72,16))     # top frame
        vline(4,4,15, (128,72,16))     # left frame
        vline(11,4,15,(128,72,16))     # right frame

    # ── 21 SIGN POST ──────────────────────────────────────────
    elif idx == 21:
        fill(GRASS1)
        # Post
        rect(7,6,8,15, SIGN_POST)
        # Sign board
        rect(3,2,12,6, (224,176,96))
        rect(4,3,11,5, (248,208,128))
        # text lines
        hline(3,5,10, SIGN_POST)
        hline(5,5,10, SIGN_POST)

    # ── 22 POKE-CENTRE (red cross) ────────────────────────────
    elif idx == 22:
        fill(HOUSE_WALL)
        # Red cross
        rect(6,3,9,12, HOUSE_RF)
        rect(3,6,12,9, HOUSE_RF)
        # White centre
        rect(7,4,8,11, (255,255,255))
        rect(4,7,11,8, (255,255,255))

    # ── 23 SAND / DIRT ────────────────────────────────────────
    elif idx == 23:
        fill(SAND)
        for pts in [(2,5),(8,2),(13,8),(5,13),(11,11),(3,10)]:
            d.rectangle([pts[0],pts[1],pts[0]+1,pts[1]+1], fill=rgba(DIRT))

    # ── 24 LEDGE (one-way cliff) ──────────────────────────────
    elif idx == 24:
        fill(GRASS1)
        hline(12,0,15, CLIFF_S)
        hline(13,0,15, CLIFF_T)
        hline(14,0,15, CLIFF_S)
        hline(15,0,15, (80,60,10))
        # Arrow hint
        for x in [6,7,8,9]: px(x,14,(248,216,80))

    # ── 25 POND / LAKE centre ─────────────────────────────────
    elif idx == 25:
        fill(WATER1)
        # Ripple circles
        for pts in [(3,3,2),(9,6,2),(5,12,2),(12,10,2)]:
            d.ellipse([pts[0]-pts[2],pts[1]-pts[2],pts[0]+pts[2],pts[1]+pts[2]],
                      outline=rgba(WATER_SP), fill=None)

    # ── 26 GRASS path (town cobble-stone) ─────────────────────
    elif idx == 26:
        fill(PATH1)
        # Cobble stones
        for gy in range(2):
            for gx in range(3):
                ox, oy = gx*5+1, gy*7+1
                rect(ox,oy,ox+3,oy+5, PATH2)
                px(ox,oy, PATH_EDGE); px(ox+3,oy,PATH_EDGE)

    # ── 27 FLOWER BED (red yellow white) ──────────────────────
    elif idx == 27:
        fill(GRASS1)
        for fx,fy,fc in [(2,2,FLOWER_R),(7,4,FLOWER_Y),(12,3,FLOWER_W),
                          (4,11,FLOWER_Y),(10,12,FLOWER_R),(14,9,FLOWER_W),
                          (1,7,FLOWER_W),(8,8,FLOWER_R)]:
            px(fx,fy,fc); px(fx,fy+1,FLOWER_G)

    # ── 28 WATER left-edge ────────────────────────────────────
    elif idx == 28:
        fill(GRASS1)
        rect(12,0,15,15, WATER1)
        vline(11,0,15, WATER_SP)
        for y in range(16):
            if y%3==0: px(12,y, WATER_SP)

    # ── 29 WATER right-edge ───────────────────────────────────
    elif idx == 29:
        fill(WATER1)
        rect(0,0,3,15, GRASS1)
        vline(4,0,15, WATER_SP)

    # ── 30 INSIDE HOUSE FLOOR ─────────────────────────────────
    elif idx == 30:
        fill((240,224,176))
        for y in range(0,16,4): hline(y,0,15,(208,192,144))

    # ── 31 BLANK (transparent) ────────────────────────────────
    elif idx == 31:
        pass  # fully transparent

    else:
        fill(BLACK)  # undefined tile = black

    return img


# ─────────────────────────────────────────────────────────────
# Build the atlas  (32 tiles per row, 1 row → 32 tiles total)
# ─────────────────────────────────────────────────────────────
ATLAS_COLS = 32
TOTAL_TILES = 32
atlas = Image.new("RGBA", (ATLAS_COLS * TILE, TILE), (0,0,0,0))

for i in range(TOTAL_TILES):
    t = draw_tile(i)
    atlas.paste(t, (i * TILE, 0), t)

atlas.save("assets/pokemon_tiles.png")
print(f"Saved assets/pokemon_tiles.png  ({atlas.size})")

# ─────────────────────────────────────────────────────────────
# Generate a stand-alone tree sprite (3×4 tiles = 48×64 px)
# ─────────────────────────────────────────────────────────────
TREE_W, TREE_H = 48, 48

def draw_tree_sprite():
    img = Image.new("RGBA", (TREE_W, TREE_H), (0,0,0,0))
    d   = ImageDraw.Draw(img)

    # Shadow at bottom
    d.ellipse([10,40,37,47], fill=(0,0,0,60))

    # Canopy (big round blob)
    d.ellipse([2,2,45,38], fill=rgba(TREE_TOP))
    d.ellipse([4,4,43,32], fill=rgba(TREE_MID))
    
    # Highlight
    d.ellipse([8,6,22,18], fill=(88,176,48,180))

    # Trunk
    d.rectangle([18,30,29,47], fill=rgba(TREE_TRK))
    d.rectangle([19,30,22,46], fill=rgba(TRUNK_LT))

    return img

tree_sprite = draw_tree_sprite()
tree_sprite.save("assets/tree_pokemon.png")
print("Saved assets/tree_pokemon.png")

# ─────────────────────────────────────────────────────────────
# Build a proper overworld background texture (not atlas-based)
# using direct pillow drawing for the full map background
# ─────────────────────────────────────────────────────────────
print("Done! Tile indices:")
names = {
    0:"BORDER",1:"GRASS",2:"GRASS2",3:"GRASS_FLOWER",4:"TALL_GRASS",
    5:"PATH",6:"PATH_L",7:"PATH_R",8:"WATER",9:"WATER_SPARKLE",
    10:"WATER_TOP",11:"WATER_BOT",12:"CLIFF_TOP",13:"CLIFF_FACE",
    14:"TREE_TOP",15:"TREE_BOT",16:"WALL",17:"ROOF_TOP",18:"ROOF_BOT",
    19:"WINDOW",20:"DOOR",21:"SIGN",22:"POKECENTER",23:"SAND",
    24:"LEDGE",25:"POND",26:"COBBLE",27:"FLOWERS",28:"WATER_L",
    29:"WATER_R",30:"FLOOR",31:"BLANK"
}
for i,n in names.items():
    print(f"  {i:2d}  {n}")
