import shutil
import os

base_dir = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\Pixel Crawler - Free Pack 2.0.4\Pixel Crawler - Free Pack\Entities\Mobs\Skeleton Crew\Skeleton - Base"
dest_dir = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new"

files = {
    "Idle/Idle-Sheet.png": "skel_idle.png",
    "Run/Run-Sheet.png": "skel_run.png",
    "Hit/Hit-Sheet.png": "skel_hit.png",
    "Death/Death-Sheet.png": "skel_death.png"
}

for src, dest in files.items():
    src_path = os.path.join(base_dir, src)
    if os.path.exists(src_path):
        shutil.copy(src_path, os.path.join(dest_dir, dest))
        print(f"Copied {src} to {dest}")
    else:
        print(f"Missing {src_path}")

# Also copy Orc warrior for variety
base_orc = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\Pixel Crawler - Free Pack 2.0.4\Pixel Crawler - Free Pack\Entities\Mobs\Orc Crew\Orc - Warrior"
orc_files = {
    "Idle/Idle-Sheet.png": "orc_idle.png",
    "Run/Run-Sheet.png": "orc_run.png",
}
for src, dest in orc_files.items():
    src_path = os.path.join(base_orc, src)
    if os.path.exists(src_path):
        shutil.copy(src_path, os.path.join(dest_dir, dest))
        print(f"Copied {src} to {dest}")
