import math
from PIL import Image, ImageDraw, ImageFont

width = 1920
height = 1080
image = Image.new("RGB", (width, height), "#13141f")
draw = ImageDraw.Draw(image)

# Background subtle radial/vertical gradient
for y in range(height):
    ratio = y / height
    r = int(19 * (1 - ratio * 0.4))
    g = int(20 * (1 - ratio * 0.4) + 10 * ratio)
    b = int(31 * (1 - ratio * 0.3) + 35 * ratio)
    draw.line([(0, y), (width, y)], fill=(r, g, b))

# Elegant geometric accent lines in center
center_x, center_y = width // 2, height // 2
radius = 160

# Concentric polygon rings
points = []
for i in range(6):
    angle = i * (2 * math.pi / 6) - (math.pi / 2)
    x = center_x + radius * math.cos(angle)
    y = center_y + radius * math.sin(angle)
    points.append((x, y))

draw.polygon(points, outline="#6366f1", width=3)

# Inner hexagon
inner_pts = []
for i in range(6):
    angle = i * (2 * math.pi / 6) - (math.pi / 2)
    x = center_x + (radius * 0.6) * math.cos(angle)
    y = center_y + (radius * 0.6) * math.sin(angle)
    inner_pts.append((x, y))

draw.polygon(inner_pts, outline="#818cf8", width=2)

# Central node
draw.ellipse(
    [center_x - 12, center_y - 12, center_x + 12, center_y + 12],
    fill="#a5b4fc",
    outline="#e0e7ff",
    width=2
)

# Diagonal subtle accent grids
for offset in range(-800, 800, 80):
    draw.line([(0, center_y + offset), (width, center_y + offset + 300)], fill=(35, 38, 58), width=1)

image.save("dist_ubuntu_de/wallpaper.png", "PNG")
image.save("ubuntu_dense_de/wallpaper.png", "PNG")
print("Saved wallpaper.png successfully (1920x1080)!")
