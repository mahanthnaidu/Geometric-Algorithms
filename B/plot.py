import pandas as pd
import matplotlib.pyplot as plt

def read_points_and_square(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    global maxi
    n = int(lines[0])
    point_lines = lines[1:n+1]
    square_lines = lines[n+2:n+6]
    maxi = lines[n+1]
    # Parse points
    points = [list(map(float, line.strip().split(' '))) for line in point_lines]
    square = [list(map(float, line.strip().split(' '))) for line in square_lines]
    
    # Close the square by repeating the first point
    square.append(square[0])

    return points, square


points, square = read_points_and_square("out.txt")

# Separate x and y
px, py = zip(*points)
sx, sy = zip(*square)

# Plot
plt.figure(figsize=(6, 6))
plt.scatter(px, py, color='blue', label='Points')
plt.plot(sx, sy, color='red', linewidth=2, label='Square')

plt.title(f"Maximum unit square with {maxi} points")
plt.xlabel("x")
plt.ylabel("y")
plt.axis('equal')
plt.grid(True)
plt.legend()

plt.margins(0.05)  # 5% margin
plt.tight_layout(pad=1)  # reduce padding
plt.xlim(min(px + sx) - 1 , max(px + sx) + 1)
plt.ylim(min(py + sy) - 1, max(py + sy) + 1)
plt.show()