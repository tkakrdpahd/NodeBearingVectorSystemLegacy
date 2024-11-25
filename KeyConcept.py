import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.special import comb

# -------------------- Adjustable Parameters --------------------

# Node Vectors (r, theta, phi)
node_vectors = [
    (1.0, np.radians(0), np.radians(90)),    # Node 1: along the x-axis
    (1.0, np.radians(120), np.radians(60)),  # Node 2: 120 degrees from Node 1
    (1.0, np.radians(240), np.radians(60)),  # Node 3: symmetrical position to Node 2
    (0.5, np.radians(0), np.radians(0)),     # Node 4: positioned upwards
    # New nodes without bearing vectors
    (1.0, np.radians(180), np.radians(90)),  # Node 5: opposite to Node 1 on x-axis
    (1.0, np.radians(90), np.radians(90))    # Node 6: along the y-axis
]

# Bearing Vectors for each node
bearing_vectors = [
    {
        'D_i': 2,
        'bearing_vectors': [
            {'theta': np.radians(30), 'phi': np.radians(75), 'force': np.array([0.6, 0.2, 0.2])},
            {'theta': np.radians(60), 'phi': np.radians(60), 'force': np.array([0.4, 0.4, 0.2])}
        ]
    },
    {
        'D_i': 2,
        'bearing_vectors': [
            {'theta': np.radians(150), 'phi': np.radians(75), 'force': np.array([0.5, 0.3, 0.2])},
            {'theta': np.radians(180), 'phi': np.radians(60), 'force': np.array([0.3, 0.5, 0.2])}
        ]
    },
    {
        'D_i': 2,
        'bearing_vectors': [
            {'theta': np.radians(270), 'phi': np.radians(75), 'force': np.array([0.5, 0.2, 0.3])},
            {'theta': np.radians(300), 'phi': np.radians(60), 'force': np.array([0.2, 0.6, 0.2])}
        ]
    },
    {
        'D_i': 1,
        'bearing_vectors': [
            {'theta': np.radians(0), 'phi': np.radians(45), 'force': np.array([0.7, 0.2, 0.1])}
        ]
    },
    # New nodes without bearing vectors (None indicates no bearing vectors)
    None,  # Node 5 has no bearing vectors
    None   # Node 6 has no bearing vectors
]

# Visualization Parameters
max_bar_length = 0.5
min_bar_length = 0.1

# Colors
node_colors = ['red', 'green', 'blue', 'purple', 'orange', 'cyan']
bar_colors = {
    'outgoing': 'cyan',
    'incoming': 'magenta'
}
curve_colors = ['orange', 'green', 'pink', 'brown', 'gray', 'olive', 'blue', 'red']
control_point_color = 'black'
line_color = 'black'  # Color for straight lines

# User-defined alpha parameter for Bézier curve
alpha = 0.5

# -------------------- Helper Functions --------------------

def spherical_to_cartesian(r, theta, phi):
    """Converts spherical coordinates to Cartesian coordinates."""
    x = r * np.sin(phi) * np.cos(theta)
    y = r * np.sin(phi) * np.sin(theta)
    z = r * np.cos(phi)
    return np.array([x, y, z])

def normalize_vector(v):
    """Safely normalizes a vector to avoid NaN if the magnitude is zero."""
    norm = np.linalg.norm(v)
    return v / norm if norm != 0 else v

def plot_node_vector(ax, pos, color, label):
    """Plots a node vector and returns its position in Cartesian coordinates."""
    ax.scatter(*pos, c=color, s=50)
    ax.text(*pos, label, fontsize=10)
    return pos

def plot_bearing_vectors(ax, node_pos, bearing_info):
    """Plots bearing vectors for a given node."""
    cumulative_vector = np.zeros(3)
    D_i = bearing_info['D_i']
    for d in range(D_i):
        bv = bearing_info['bearing_vectors'][d]
        theta_i = bv['theta']
        phi_i = bv['phi']
        force_i = bv['force']
        
        # Bearing vector in Cartesian coordinates
        bearing_dir = spherical_to_cartesian(1, theta_i, phi_i)
        bearing_dir = normalize_vector(bearing_dir)
        
        # Apply force components
        F_i = force_i[0] * np.array([1, 0, 0]) + \
              force_i[1] * np.array([0, 1, 0]) + \
              force_i[2] * np.array([0, 0, 1])
        
        # Cumulative bearing vector
        V_i = (d + 1) * np.outer(bearing_dir, F_i).sum(axis=1)
        cumulative_vector += V_i
        
        # Plot bearing vector
        start = node_pos
        end = node_pos + V_i
        ax.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]],
                color=bar_colors['outgoing'], linewidth=2)
        ax.text(*end, f'BV D{d+1}', fontsize=8)
    
    return cumulative_vector

def bezier_curve(points, nTimes=100):
    """Generates a Bézier curve of degree n."""
    nPoints = len(points)
    xPoints = np.array([p[0] for p in points])
    yPoints = np.array([p[1] for p in points])
    zPoints = np.array([p[2] for p in points])

    t = np.linspace(0.0, 1.0, nTimes)
    polynomial_array = np.array([
        comb(nPoints - 1, i) * (t ** i) * (1 - t) ** (nPoints - 1 - i)
        for i in range(nPoints)
    ])
    
    xvals = np.dot(xPoints, polynomial_array)
    yvals = np.dot(yPoints, polynomial_array)
    zvals = np.dot(zPoints, polynomial_array)
    
    return xvals, yvals, zvals

# -------------------- Main Visualization Function --------------------

def create_visualization():
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    node_positions = []
    cumulative_vectors = []
    for i, node in enumerate(node_vectors):
        r, theta, phi = node
        node_pos = spherical_to_cartesian(r, theta, phi)
        label = f'Node {i+1}'
        plot_node_vector(ax, node_pos, node_colors[i % len(node_colors)], label)
        node_positions.append(node_pos)
        
        if bearing_vectors[i] is not None:
            cumulative_vector = plot_bearing_vectors(ax, node_pos, bearing_vectors[i])
        else:
            cumulative_vector = np.zeros(3)
        cumulative_vectors.append(cumulative_vector)
    
    # Initialize legend elements
    legend_elements = []

    # Calculate and plot Bézier curves between nodes
    curve_index = 0
    for i in range(len(node_positions)):
        for j in range(i + 1, len(node_positions)):
            N1 = node_positions[i]
            N2 = node_positions[j]
            C1 = cumulative_vectors[i]
            C2 = cumulative_vectors[j]
            D1 = bearing_vectors[i]['D_i'] if bearing_vectors[i] is not None else 0
            D2 = bearing_vectors[j]['D_i'] if bearing_vectors[j] is not None else 0
            
            # Check if both nodes have no bearing vectors
            if D1 == 0 and D2 == 0:
                # Only draw a straight line (polygon modeling)
                ax.plot([N1[0], N2[0]],
                        [N1[1], N2[1]],
                        [N1[2], N2[2]],
                        color=line_color,
                        linestyle='--',
                        linewidth=1)
            else:
                # Define control points
                control_points = [N1]
                
                # Control points from node i
                if D1 > 0:
                    for d in range(D1):
                        control_points.append(N1 + C1 * ((d + 1) / D1))
                
                # Middle control point
                middle_point = alpha * (N1 + C1) + (1 - alpha) * (N2 - C2)
                control_points.append(middle_point)
                
                # Control points from node j
                if D2 > 0:
                    for d in range(D2, 0, -1):
                        control_points.append(N2 - C2 * (d / D2))
                
                control_points.append(N2)
                
                # Plot control points
                for cp in control_points:
                    ax.scatter(*cp, color=control_point_color, s=30)
                
                # Generate Bézier curve
                xvals, yvals, zvals = bezier_curve(control_points)
                bezier_line, = ax.plot(xvals, yvals, zvals,
                                       color=curve_colors[curve_index % len(curve_colors)],
                                       linewidth=2)
                
                # Plot straight line between nodes (polygon modeling)
                ax.plot([N1[0], N2[0]],
                        [N1[1], N2[1]],
                        [N1[2], N2[2]],
                        color=line_color,
                        linestyle='--',
                        linewidth=1)
                
            curve_index += 1

    # Create custom legend
    from matplotlib.lines import Line2D
    legend_elements = [
        Line2D([0], [0], color=line_color, linestyle='--', label='Polygon Modeling (No Bearing Vector)'),
        Line2D([0], [0], color='black', linewidth=2, label='Bézier Curve Modeling (Including Bearing Vector)')
    ]

    ax.legend(handles=legend_elements, loc='upper right')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Integration of Polygon Modeling and Non-Rational Integer Curve')

    # Set the same scale for all axes
    max_range = np.array([
        np.ptp(ax.get_xlim()),
        np.ptp(ax.get_ylim()),
        np.ptp(ax.get_zlim())
    ]).max() / 2.0

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
