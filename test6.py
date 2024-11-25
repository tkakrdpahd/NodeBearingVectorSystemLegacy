import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.spatial import cKDTree
from scipy.special import comb

# -------------------- Adjustable Parameters --------------------

# Generate 25 node vectors evenly distributed on a sphere
num_nodes = 25
indices = np.arange(0, num_nodes, dtype=float) + 0.5
phi = np.arccos(1 - 2*indices/num_nodes)
theta = np.pi * (1 + 5**0.5) * indices

node_vectors = np.column_stack([np.sin(phi) * np.cos(theta), 
                                np.sin(phi) * np.sin(theta), 
                                np.cos(phi)])

# Generate outward vectors for each node (depth 2, 45-degree angle)
def generate_outward_vectors(node_pos):
    # First vector: directly outward
    v1 = node_pos / np.linalg.norm(node_pos)
    
    # Second vector: 45 degrees from the first
    perpendicular = np.cross(node_pos, np.array([1, 0, 0]))
    if np.allclose(perpendicular, 0):
        perpendicular = np.cross(node_pos, np.array([0, 1, 0]))
    perpendicular = perpendicular / np.linalg.norm(perpendicular)
    v2 = (v1 + perpendicular) / np.sqrt(2)  # 45-degree angle
    
    return v1 * 0.2, v2 * 0.2  # Adjust these scalars to change vector lengths

outward_vectors = [generate_outward_vectors(node_pos) for node_pos in node_vectors]

# Colors
node_colors = plt.cm.rainbow(np.linspace(0, 1, num_nodes))
curve_color = 'lightblue'
vector_colors = ['red', 'green']

# Number of nearest neighbors to connect
k_neighbors = 5

# Curve Influence
curve_influence_strength = 0.3

# -------------------- Helper Functions --------------------

def bernstein_poly(i, n, t):
    return comb(n, i) * (t**(n-i)) * (1 - t)**i

def bezier_curve(points, nTimes=50):
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

def plot_node_vector(ax, pos, color, index):
    ax.scatter(*pos, c=[color], s=30)
    return pos

def plot_outward_vector(ax, start, vector, color):
    ax.quiver(*start, *vector, color=color, length=1, normalize=True)

# -------------------- Main Visualization Function --------------------

def create_visualization():
    fig = plt.figure(figsize=(15, 12))
    ax = fig.add_subplot(111, projection='3d')

    # Plot node vectors and outward vectors
    for i, pos in enumerate(node_vectors):
        plot_node_vector(ax, pos, node_colors[i], i)
        for j, vector in enumerate(outward_vectors[i]):
            plot_outward_vector(ax, pos, vector, vector_colors[j])

    # Create KD-tree for efficient nearest neighbor search
    tree = cKDTree(node_vectors)

    # Find k nearest neighbors for each node and plot curved connections
    for i, pos in enumerate(node_vectors):
        distances, indices = tree.query(pos, k=k_neighbors+1)
        for neighbor_idx in indices[1:]:
            neighbor_pos = node_vectors[neighbor_idx]
            
            # Calculate control points influenced by outward vectors
            control_point1 = pos + outward_vectors[i][0] * curve_influence_strength
            control_point2 = neighbor_pos + outward_vectors[neighbor_idx][0] * curve_influence_strength

            curve_points = [pos, control_point1, control_point2, neighbor_pos]
            xvals, yvals, zvals = bezier_curve(curve_points)
            ax.plot(xvals, yvals, zvals, color=curve_color, linewidth=1, alpha=0.5)

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f'Sphere Approximation with {num_nodes} Node Vectors and 45-degree Depth 2 Vectors')

    # Set the same scale for all axes
    max_range = np.array([ax.get_xlim(), ax.get_ylim(), ax.get_zlim()]).ptp().max() / 2.0
    mid_x = np.mean(ax.get_xlim())
    mid_y = np.mean(ax.get_ylim())
    mid_z = np.mean(ax.get_zlim())
    ax.set_xlim(mid_x - max_range, mid_x + max_range)
    ax.set_ylim(mid_y - max_range, mid_y + max_range)
    ax.set_zlim(mid_z - max_range, mid_z + max_range)

    plt.tight_layout()
    plt.show()

# -------------------- Run Visualization --------------------

if __name__ == "__main__":
    create_visualization()