import shutil, os

def cp(src, dst):
    src_path = os.path.join("Pixel Crawler - Free Pack 2.0.4/Pixel Crawler - Free Pack/", src)
    if os.path.exists(src_path):
        shutil.copy(src_path, "assets_new/" + dst)
        print(f"Copied {dst}")
    else:
        print(f"MISSING: {src_path}")

cp("Environment/Props/Static/Trees/Model_01/Size_02.png", "tree_s1.png")
cp("Environment/Props/Static/Trees/Model_01/Size_05.png", "tree_hq.png")
print("Tree assets copied.")
