import shutil
import os

files = {
    "Idle_Base/Idle_Down-Sheet.png": "player_idle_down.png",
    "Idle_Base/Idle_Up-Sheet.png": "player_idle_up.png",
    "Idle_Base/Idle_Side-Sheet.png": "player_idle_side.png",
    "Walk_Base/Walk_Down-Sheet.png": "player_walk_down.png",
    "Walk_Base/Walk_Up-Sheet.png": "player_walk_up.png",
    "Walk_Base/Walk_Side-Sheet.png": "player_walk_side.png",
    "Slice_Base/Slice_Down-Sheet.png": "player_slice_down.png",
    "Slice_Base/Slice_Up-Sheet.png": "player_slice_up.png",
    "Slice_Base/Slice_Side-Sheet.png": "player_slice_side.png"
}

base_dir = "Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Entities/Characters/Body_A/Animations/"
dest_dir = "assets/"

for src, dest in files.items():
    src_path = os.path.join(base_dir, src)
    if os.path.exists(src_path):
        shutil.copy(src_path, os.path.join(dest_dir, dest))
    else:
        print(f"Missing: {src_path}")

shutil.copy("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Entities/Mobs/Orc Crew/Orc - Warrior/Idle/Idle-Sheet.png", "assets/enemy_idle.png")
shutil.copy("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/Entities/Mobs/Orc Crew/Orc - Warrior/Run/Run-Sheet.png", "assets/enemy_run.png")
print("Done")
