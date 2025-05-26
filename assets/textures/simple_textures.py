#!/usr/bin/env python3
"""
Simple script to create basic textures for testing your 3D viewer
Run this in your project's assets/textures/ directory
"""

from PIL import Image, ImageDraw
import numpy as np

def create_checkerboard(size=256, squares=8):
    """Create a classic checkerboard texture"""
    image = Image.new('RGB', (size, size), 'white')
    draw = ImageDraw.Draw(image)

    square_size = size // squares

    for i in range(squares):
        for j in range(squares):
            if (i + j) % 2 == 1:
                x1 = i * square_size
                y1 = j * square_size
                x2 = x1 + square_size
                y2 = y1 + square_size
                draw.rectangle([x1, y1, x2, y2], fill='black')

    return image

def create_brick_texture(size=256):
    """Create a simple brick texture"""
    image = Image.new('RGB', (size, size), (139, 69, 19))  # Brown base
    draw = ImageDraw.Draw(image)

    brick_height = size // 8
    brick_width = size // 4
    mortar_color = (200, 200, 200)  # Light gray mortar

    # Draw horizontal mortar lines
    for y in range(0, size, brick_height):
        draw.line([(0, y), (size, y)], fill=mortar_color, width=3)

    # Draw vertical mortar lines (offset every other row)
    for row in range(size // brick_height):
        offset = (brick_width // 2) if row % 2 == 1 else 0
        y = row * brick_height

        for x in range(-brick_width, size + brick_width, brick_width):
            x_pos = x + offset
            if 0 <= x_pos <= size:
                draw.line([(x_pos, y), (x_pos, y + brick_height)],
                          fill=mortar_color, width=2)

    return image

def create_wood_texture(size=256):
    """Create a simple wood grain texture"""
    # Create base brown color with some variation
    np.random.seed(42)  # For reproducible results

    # Base wood color
    base_color = np.array([101, 67, 33])  # Dark brown

    # Create some grain patterns
    image_array = np.zeros((size, size, 3), dtype=np.uint8)

    for y in range(size):
        # Create wood ring pattern
        ring_intensity = abs(np.sin(y * 0.1)) * 30

        for x in range(size):
            # Add some noise for grain texture
            noise = np.random.randint(-20, 20)

            # Calculate final color
            color = base_color + ring_intensity + noise
            color = np.clip(color, 0, 255)

            image_array[y, x] = color

    return Image.fromarray(image_array)

def create_metal_texture(size=256):
    """Create a brushed metal texture"""
    np.random.seed(42)

    # Base silver color
    base_color = 150

    image_array = np.zeros((size, size, 3), dtype=np.uint8)

    for y in range(size):
        # Create horizontal brush lines
        brush_intensity = np.sin(y * 0.5) * 20

        for x in range(size):
            # Add horizontal streak pattern
            streak = np.sin(x * 0.02) * 10
            noise = np.random.randint(-10, 10)

            final_intensity = base_color + brush_intensity + streak + noise
            final_intensity = np.clip(final_intensity, 100, 200)

            # Make it slightly blue-tinted metal
            image_array[y, x] = [final_intensity, final_intensity, final_intensity + 10]

    return Image.fromarray(image_array)

def create_grass_texture(size=256):
    """Create a simple grass texture"""
    # Green base
    image = Image.new('RGB', (size, size), (34, 139, 34))  # Forest green
    draw = ImageDraw.Draw(image)

    # Add some darker green patches
    np.random.seed(42)
    for _ in range(50):
        x = np.random.randint(0, size)
        y = np.random.randint(0, size)
        radius = np.random.randint(5, 20)
        dark_green = (0, 100, 0)
        draw.ellipse([x-radius, y-radius, x+radius, y+radius],
                     fill=dark_green)

    # Add some lighter patches
    for _ in range(30):
        x = np.random.randint(0, size)
        y = np.random.randint(0, size)
        radius = np.random.randint(3, 15)
        light_green = (50, 205, 50)
        draw.ellipse([x-radius, y-radius, x+radius, y+radius],
                     fill=light_green)

    return image

def main():
    """Create all texture examples"""
    textures = {
        'checkerboard.png': create_checkerboard(),
        'brick.png': create_brick_texture(),
        'wood.png': create_wood_texture(),
        'metal.png': create_metal_texture(),
        'grass.png': create_grass_texture()
    }

    for filename, texture in textures.items():
        texture.save(filename)
        print(f"Created {filename}")

    print("All textures created successfully!")
    print("You can now use these in your 3D viewer.")

if __name__ == "__main__":
    main()