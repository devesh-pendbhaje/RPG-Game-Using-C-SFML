import shutil
import os

base_dir = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\Pixel Crawler - Free Pack 2.0.4\Pixel Crawler - Free Pack\Entities\Characters\Body_A\Animations"
dest_dir = r"c:\Users\diksh\OneDrive\Desktop\My_Project\OOPs Game\Game 2\assets_new"

files = {
    "Collect_Base/Collect_Down-Sheet.png": "Collect_Down-Sheet.png",
    "Collect_Base/Collect_Up-Sheet.png": "Collect_Up-Sheet.png",
    "Collect_Base/Collect_Side-Sheet.png": "Collect_Side-Sheet.png",
    "Crush_Base/Crush_Down-Sheet.png": "Crush_Down-Sheet.png",
    "Crush_Base/Crush_Up-Sheet.png": "Crush_Up-Sheet.png",
    "Crush_Base/Crush_Side-Sheet.png": "Crush_Side-Sheet.png"
}

for src, dest in files.items():
    src_path = os.path.join(base_dir, src)
    if os.path.exists(src_path):
        shutil.copy(src_path, os.path.join(dest_dir, dest))
        print(f"Copied {src} to {dest}")
