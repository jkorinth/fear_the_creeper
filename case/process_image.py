import numpy as np
import sys
from PIL import Image

# 1. Load the image
img = Image.open("creeper_face.webp")

# 2. Convert to luminance (grayscale)
img = img.convert("L")

# Get dimensions
width, height = img.size

# 3. Crop to dimensions divisible by 8 (for clean 8x8 grid division)
new_width = width - (width % 8)
new_height = height - (height % 8)
img = img.crop((0, 0, new_width, new_height))

print(f"Cropped {width}x{height} to {new_width}x{new_height}")

# Convert to numpy array
data = np.asarray(img)

# Calculate block dimensions
block_height = new_height // 8
block_width = new_width // 8

print(f"Each block is {block_width}x{block_height} pixels")

# 4. Split image into 8x8 grid of blocks
# Reshape to create 8x8 grid where each block can be any size
blocks = data.reshape(8, block_height, 8, block_width)

# 5. Compute the average for each block in the 8x8 grid
averages = np.mean(blocks, axis=(1, 3))

print(f"Shape of averages array: {averages.shape}")
print(f"This is an 8x8 grid of block averages")

# Apply threshold: set values below 50 to 0
threshold = 50
averages_thresholded = np.where(averages >= threshold, averages, 0)

# 6. Normalize only the non-zero values to range [0,1]
# Get mask of non-zero values for normalization
valid_mask = averages_thresholded > 0

if np.any(valid_mask):
    min_val = np.min(averages_thresholded[valid_mask])
    max_val = np.max(averages_thresholded[valid_mask])
    
    # Create normalized array, keeping zeros as zeros
    normalized = np.zeros_like(averages_thresholded)
    if max_val > min_val:  # Avoid division by zero
        normalized[valid_mask] = 0.5 + 0.5 * (averages_thresholded[valid_mask] - min_val) / (max_val - min_val)
    else:
        normalized[valid_mask] = 1.0  # If all values are the same, set to 1
else:
    min_val = max_val = 0
    normalized = np.zeros_like(averages_thresholded)

# 7. Output the values with custom formatting
print(f"\nThreshold: {threshold}")
print(f"Min value (above threshold): {min_val:.0f}, Max value: {max_val:.0f}")

print("\n--- Original averages (step 5) - Integer part only ---")
# Convert to integers and format as aligned grid
raw_integers = averages_thresholded.astype(int)
for row in raw_integers:
    print(' '.join(f"{val:3d}" for val in row))

print("\n--- Normalized values (step 6) - 2 decimal places ---")
# Format normalized values with 2 decimal places, aligned
for row in normalized:
    print(' '.join(f"{val:4.2f}" for val in row))

np.savetxt('heights.csv', normalized, delimiter=',', fmt='%2.2f')

WIDTH = 40
LENGTH = 40
THICK = 5
WALL = 2
W = WIDTH // 8
L = LENGTH // 8
# Generate OpenSCAD format
with open("creeper.scad", "w") as f:
    f.write(f'''
      color("green")
      difference() {{
        translate([0, 0, {-THICK}])
        cube([{WIDTH}, {LENGTH}, {THICK}]);
        translate([{WALL}, {WALL}, {-THICK - 0.1}])
        cube([{WIDTH - 2 * WALL}, {LENGTH - 2 * WALL}, {WALL + THICK}]);
      }};
      difference() {{
      cube([{WIDTH}, {LENGTH}, {THICK}]);
    ''');
    for y, row in enumerate(normalized * THICK):
        for x, v in enumerate(row):
            #@if v > 0:
            f.write(f'color("green") translate([{ y * W }, { x * L }, {v}]) cube([{W + 0.001}, {L + 0.001}, {THICK + 0.001 - v}]);\n');
    f.write("};\n");
        
