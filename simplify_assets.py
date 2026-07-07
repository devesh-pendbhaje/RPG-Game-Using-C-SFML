import os
import shutil

# Define source and destination
base_src = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\Pixel Crawler - Free Pack 2.0.4\Pixel Crawler - Free Pack"
dest = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new"

if not os.path.exists(dest):
    os.makedirs(dest)

mappings = {
    # Tilesets
    "Environment/Tilesets/Floors_Tiles.png": "floors.png",
    "Environment/Tilesets/Water_tiles.png": "water.png",
    "Environment/Tilesets/Wall_Tiles.png": "walls_tile.png",
    
    # Props
    "Environment/Props/Static/Trees/Model_01/Size_03.png": "tree.png",
    "Environment/Props/Static/Rocks.png": "rocks.png",
    "Environment/Props/Static/Farm.png": "farm.png",
    "Environment/Props/Static/Resources.png": "resources.png",
    "Environment/Props/Static/Vegetation.png": "vegetation.png",
    
    # Structures
    "Environment/Structures/Buildings/Walls.png": "house_walls.png",
    "Environment/Structures/Buildings/Roofs.png": "house_roofs.png",
    
    # Character Animations (Player)
    "Entities/Characters/Body_A/Animations/Idle_Base/Idle_Down-Sheet.png": "player_idle_down.png",
    "Entities/Characters/Body_A/Animations/Walk_Base/Walk_Down-Sheet.png": "player_walk_down.png",
    "Entities/Characters/Body_A/Animations/Slice_Base/Slice_Down-Sheet.png": "player_attack_down.png",
    
    # Skeleton Animations (Enemy)
    "Entities/Mobs/Skeleton Crew/Skeleton - Base/Idle/Idle-Sheet.png": "enemy_idle.png",
    "Entities/Mobs/Skeleton Crew/Skeleton - Base/Run/Run-Sheet.png": "enemy_run.png",
}

for src_rel, dest_name in mappings.items():
    src_path = os.path.join(base_src, src_rel.replace("/", os.sep))
    dest_path = os.path.join(dest, dest_name)
    if os.path.exists(src_path):
        shutil.copy2(src_path, dest_path)
        print(f"Copied {src_rel} -> {dest_name}")
    else:
        print(f"MISSING: {src_path}")
