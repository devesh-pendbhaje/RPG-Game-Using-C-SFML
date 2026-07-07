from PIL import Image, ImageDraw

def gen_wood():
    img = Image.new("RGBA", (16, 16), (0,0,0,0))
    d = ImageDraw.Draw(img)
    
    # Simple log shape
    # Dark brown outline
    d.rectangle([2, 5, 13, 10], fill=(101, 67, 33), outline=(60, 40, 20))
    # Rings/details
    d.line([4, 6, 4, 9], fill=(139, 90, 43))
    d.line([8, 6, 8, 9], fill=(139, 90, 43))
    d.line([12, 6, 12, 9], fill=(139, 90, 43))
    
    img.save("assets/wood.png")
    print("Saved assets/wood.png")

gen_wood()
