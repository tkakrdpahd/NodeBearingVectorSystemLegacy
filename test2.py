import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.special import comb

# -------------------- Adjustable Parameters --------------------

# Node Vectors
node_vectors = [
    (1, np.radians(45), np.radians(45)),
    (0.8, np.radians(135), np.radians(60)),
    (0.9, np.radians(90), np.radians(30)),  # Node without bearing vectors
    (1.1, np.radians(180), np.radians(45))  # Another node without bearing vectors
]

# Bearing Vectors (now explicitly representing force)
bearing_vectors = [
    {'lateral': 0.3, 'axial': 0.4, 'radial': 0.2},
    {'lateral': 0.5, 'axial': 0.2, 'radial': 0.3},
    None,  # No bearing vector for the third node
    None   # No bearing vector for the fourth node
]

# Visualization Parameters
max_bar_length = 0.5  # Maximum length of the bearing vector bars
min_bar_length = 0.1  # Minimum length of the bearing vector bars

# Colors
node_colors = ['r', 'g', 'b', 'purple']  # Added purple for the fourth node
bar_colors = {'lateral': 'cyan', 'axial': 'm', 'radial': 'yellow'}
curve_colors = ['orange', 'green', 'pink', 'brown', 'gray', 'olive']  # Multiple colors for different connections
control_point_color = 'black'

# Curve Influence
curve_influence_strength = 0.5

# -------------------- Helper Functions --------------------

def spherical_to_cartesian(r, theta, phi):
    x = r * np.sin(phi) * np.cos(theta)
    y = r * np.sin(phi) * np.sin(theta)
    z = r * np.cos(phi)
    return np.array([x, y, z])

def plot_node_vector(ax, r, theta, phi, color):
    pos = spherical_to_cartesian(r, theta, phi)
    ax.scatter(*pos, c=color, s=50)
    ax.text(*pos, f'Node ({r:.1f}, {np.degrees(theta):.1f}°, {np.degrees(phi):.1f}°)', fontsize=8)
    return pos

def plot_asymmetric_bar(ax, center, direction, force, color, start_label, end_label):
    length = min_bar_length + (max_bar_length - min_bar_length) * force
    asymmetry = 0.5 + (force - 0.5) * 0.5
    start = center - direction * length * (1 - asymmetry)
    end = center + direction * length * asymmetry
    ax.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]], color=color, linewidth=3)
    ax.text(*start, start_label, fontsize=8, ha='center', va='center')
    ax.text(*end, end_label, fontsize=8, ha='center', va='center')
    return start, end

def bernstein_poly(i, n, t):
    return comb(n, i) * (t**(n-i)) * (1 - t)**i

def bezier_curve(points, nTimes=1000):
    nPoints = len(points)
    xPoints = np.array([p[0] for p in points])
    yPoints = np.array([p[1] for p in points])
    zPoints = np.array([p[2] for p in points])

    t = np.linspace(0.0, 1.0, nTimes)
    polynomial_array = np.array([bernstein_poly(i, nPoints-1, t) for i in range(0, nPoints)])

    xvals = np.dot(xPoints, polynomial_array)
    yvals = np.dot(yPoints, polynomial_array)
    zvals = np.dot(zPoints, polynomial_array)

    return xvals, yvals, zvals

# -------------------- Main Visualization Function --------------------

def create_visualization():
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    # Plot the unit sphere
    u, v = np.mgrid[0:2*np.pi:20j, 0:np.pi:10j]
    x = np.cos(u) * np.sin(v)
    y = np.sin(u) * np.sin(v)
    z = np.cos(v)
    ax.plot_wireframe(x, y, z, color="gray", alpha=0.2)

    node_positions = []
    for i, (r, theta, phi) in enumerate(node_vectors):
        node_pos = plot_node_vector(ax, r, theta, phi, node_colors[i])
        node_positions.append(node_pos)
        
        if bearing_vectors[i] is not None:
            radial_dir = node_pos / np.linalg.norm(node_pos)
            axial_dir = np.cross(radial_dir, [0, 0, 1])
            axial_dir /= np.linalg.norm(axial_dir)
            lateral_dir = np.cross(axial_dir, radial_dir)
            
            # Plot lateral bar (East-West)
            force = bearing_vectors[i]['lateral']
            plot_asymmetric_bar(ax, node_pos, lateral_dir, force, bar_colors['lateral'], 'W', 'E')
            
            # Plot axial bar (North-South)
            force = bearing_vectors[i]['axial']
            plot_asymmetric_bar(ax, node_pos, axial_dir, force, bar_colors['axial'], 'S', 'N')
            
            # Plot radial bar (Pole-Antipole)
            force = bearing_vectors[i]['radial']
            plot_asymmetric_bar(ax, node_pos, radial_dir, force, bar_colors['radial'], 'Antipole', 'Pole')

    # Calculate and plot Bézier curves
    curve_index = 0
    for i in range(len(node_positions)):
        for j in range(i+1, len(node_positions)):
            start_point = node_positions[i]
            end_point = node_positions[j]
            mid_point = (np.array(start_point) + np.array(end_point)) / 2

            # Calculate influence vector
            if bearing_vectors[i] is not None:
                start_influence = np.array([bearing_vectors[i][direction] for direction in ['lateral', 'axial', 'radial']])
                start_influence /= np.linalg.norm(start_influence)
            else:
                start_influence = np.zeros(3)

            if bearing_vectors[j] is not None:
                end_influence = np.array([bearing_vectors[j][direction] for direction in ['lateral', 'axial', 'radial']])
                end_influence /= np.linalg.norm(end_influence)
            else:
                end_influence = np.zeros(3)

            # Calculate control points
            if bearing_vectors[i] is None and bearing_vectors[j] is None:
                # Straight line for connections between nodes without bearing vectors
                control_point1 = start_point + (end_point - start_point) / 3
                control_point2 = end_point - (end_point - start_point) / 3
            else:
                control_point1 = start_point + (end_point - start_point) / 3 + start_influence * curve_influence_strength
                control_point2 = end_point - (end_point - start_point) / 3 - end_influence * curve_influence_strength

            points = [start_point, control_point1, control_point2, end_point]
            xvals, yvals, zvals = bezier_curve(points)
            ax.plot(xvals, yvals, zvals, color=curve_colors[curve_index % len(curve_colors)], linewidth=2, 
                    label=f'Curve {i+1}-{j+1}')

            # Plot control points
            ax.scatter(*control_point1, color=control_point_color, s=30)
            ax.scatter(*control_point2, color=control_point_color, s=30)

            curve_index += 1

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Node Vectors With and Without Bearing Vectors')

    # Set the same scale for all axes
    max_range = np.array([ax.get_xlim(), ax.get_ylim(), ax.get_zlim()]).ptp().max() / 2.0
    mid_x = np.mean(ax.get_xlim())
    mid_y = np.mean(ax.get_ylim())
    mid_z = np.mean(ax.get_zlim())
    ax.set_xlim(mid_x - max_range, mid_x + max_range)
    ax.set_ylim(mid_y - max_range, mid_y + max_range)
    ax.set_zlim(mid_z - max_range, mid_z + max_range)

    plt.legend()
    plt.tight_layout()
    plt.show()

# -------------------- Run Visualization --------------------

if __name__ == "__main__":
    create_visualization()