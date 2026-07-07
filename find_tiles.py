from PIL import Image

img = Image.open('assets/tileset.png').convert('RGB')
w, h = img.size
tw, th = 16, 16

cols = w // tw
rows = h // th

for y in range(rows):
    line = []
    for x in range(cols):
        r_sum, g_sum, b_sum = 0, 0, 0
        for dy in range(th):
            for dx in range(tw):
                r, g, b = img.getpixel((x*tw + dx, y*th + dy))
                r_sum += r
                g_sum += g
                b_sum += b
        total = tw * th
        r_avg = r_sum // total
        g_avg = g_sum // total
        b_avg = b_sum // total
        if r_avg == 0 and g_avg == 0 and b_avg == 0:
            line.append("BLK")
        elif g_avg > r_avg and g_avg > b_avg:
            line.append("GRN")
        elif r_avg > g_avg and r_avg > b_avg and r_avg > 100:
            line.append("ORG")
        elif abs(r_avg - g_avg) < 30 and abs(r_avg - b_avg) < 30 and r_avg > 100:
            line.append("GRY")
        else:
            line.append("???")
    print(f"Row {y:2}: {' '.join(line)}")

