import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def spherical_to_cartesian(r, theta, phi):
    x = r * np.sin(theta) * np.cos(phi)
    y = r * np.sin(theta) * np.sin(phi)
    z = r * np.cos(theta)
    return x, y, z

def create_electron_cloud(center, direction, magnitude, num_points=50):
    cloud_points = []
    for _ in range(num_points):
        distance = np.random.normal(loc=magnitude/2, scale=magnitude/4)
        distance = max(0, min(distance, magnitude))
        angle = np.random.normal(loc=0, scale=np.pi/8)
        rotation_axis = np.cross(direction, [0, 0, 1])
        rotation_axis_norm = np.linalg.norm(rotation_axis)
        if rotation_axis_norm > 1e-10:
            rotation_axis /= rotation_axis_norm
        else:
            rotation_axis = np.array([1, 0, 0])
        rotation_matrix = rotation_matrix_3d(rotation_axis, angle)
        rotated_direction = np.dot(rotation_matrix, direction)
        point = center + rotated_direction * distance
        cloud_points.append(point)
    return np.array(cloud_points)

def rotation_matrix_3d(axis, theta):
    axis = np.asarray(axis)
    axis = axis / np.sqrt(np.dot(axis, axis))
    a = np.cos(theta / 2.0)
    b, c, d = -axis * np.sin(theta / 2.0)
    aa, bb, cc, dd = a * a, b * b, c * c, d * d
    bc, ad, ac, ab, bd, cd = b * c, a * d, a * c, a * b, b * d, c * d
    return np.array([[aa + bb - cc - dd, 2 * (bc + ad), 2 * (bd - ac)],
                     [2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab)],
                     [2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc]])

def curve_line_segment_type1(start, end, control1, control2, num_points=100):
    t = np.linspace(0, 1, num_points)[:, np.newaxis]
    curve_points = ((1-t)**3) * start + \
                   3 * ((1-t)**2 * t) * control1 + \
                   3 * ((1-t) * t**2) * control2 + \
                   (t**3) * end
    return curve_points

def curve_line_segment_type2(start, end, control, num_points=100):
    t = np.linspace(0, 1, num_points)[:, np.newaxis]
    curve_points = ((1-t)**2) * start + \
                   2 * ((1-t) * t) * control + \
                   (t**2) * end
    return curve_points

# Set up the figure and axis
fig = plt.figure(figsize=(12, 12))
ax = fig.add_subplot(111, projection='3d')

# Create sphere
r = 1
theta = np.linspace(0, np.pi, 100)
phi = np.linspace(0, 2*np.pi, 100)
theta, phi = np.meshgrid(theta, phi)
x, y, z = spherical_to_cartesian(r, theta, phi)

# Plot sphere
ax.plot_surface(x, y, z, alpha=0.1, color='lightgray')

# Plot center point (black)
center = np.array([0, 0, 0])
ax.scatter(*center, color='black', s=100, label='Center')

# Define Node Vectors
nodes = np.array([
    spherical_to_cartesian(1, np.pi/4, 0),
    spherical_to_cartesian(1, np.pi/4, np.pi/2),
    spherical_to_cartesian(1, np.pi/2, np.pi/4),
    spherical_to_cartesian(1, 3*np.pi/4, 3*np.pi/4)
])

# Plot Node Vectors (gray)
for i, node in enumerate(nodes):
    ax.scatter(*node, color='gray', s=100, label=f'Node Vector {i+1}' if i == 0 else "")

# Define and plot Bearing Vectors as electron clouds
bearing_length = 0.3
directions = {
    'r': lambda r, theta, phi: np.array([np.sin(theta)*np.cos(phi), np.sin(theta)*np.sin(phi), np.cos(theta)]),
    'theta': lambda r, theta, phi: np.array([np.cos(theta)*np.cos(phi), np.cos(theta)*np.sin(phi), -np.sin(theta)]),
    'phi': lambda r, theta, phi: np.array([-np.sin(phi), np.cos(phi), 0]),
    '-phi': lambda r, theta, phi: np.array([np.sin(phi), -np.cos(phi), 0])
}

colors = ['red', 'green', 'blue', 'purple']

for node in nodes:
    r = np.linalg.norm(node)
    theta = np.arccos(node[2] / r)
    phi = np.arctan2(node[1], node[0])
    for i, (dir_name, dir_func) in enumerate(directions.items()):
        dir_vector = dir_func(r, theta, phi)
        cloud_points = create_electron_cloud(node, dir_vector, bearing_length)
        ax.scatter(*cloud_points.T, color=colors[i], alpha=0.1, s=10)

# Plot Curve Line Segments
for i in range(len(nodes)):
    start = nodes[i]
    end = nodes[(i+1)%len(nodes)]
    
    if i % 2 == 0:  # Curve Line Segment type 1
        control1 = start + np.random.normal(0, 0.2, 3)
        control2 = end + np.random.normal(0, 0.2, 3)
        curve_points = curve_line_segment_type1(start, end, control1, control2)
        ax.plot(curve_points[:, 0], curve_points[:, 1], curve_points[:, 2], color='cyan', label='Curve Line Segment Type 1' if i == 0 else "")
    else:  # Curve Line Segment type 2
        control = (start + end) / 2 + np.random.normal(0, 0.2, 3)
        curve_points = curve_line_segment_type2(start, end, control)
        ax.plot(curve_points[:, 0], curve_points[:, 1], curve_points[:, 2], color='magenta', label='Curve Line Segment Type 2' if i == 1 else "")

# Plot one Beeline Segment
ax.plot(*zip(nodes[0], nodes[2]), color='yellow', label='Beeline Segment')

# Set labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('3D Spherical Coordinate System with Various Curve Line Segments')

# Add legend
ax.legend()

# Show plot
plt.tight_layout()
plt.show()