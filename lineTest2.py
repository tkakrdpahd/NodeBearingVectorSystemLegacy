import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from scipy.special import comb
from scipy.interpolate import interp1d

# Node Vectors (r, theta, phi)
node_vectors = [
    (1.0, np.radians(0), np.radians(90)),    # Node 1: along the x-axis
    (1.0, np.radians(120), np.radians(60)),  # Node 2: 120 degrees from Node 1
    (1.0, np.radians(240), np.radians(60)),  # Node 3: 240 degrees from Node 1
    (1.0, np.radians(0), np.radians(0))      # Node 4: at the top
]

# Bearing Vectors for each node (simplified for clarity)
bearing_vectors = [
    {'D_i': 1, 'bearing_vectors': [{'theta': np.radians(30), 'phi': np.radians(75), 'force': np.array([0.6, 0.2, 0.2])}]},
    {'D_i': 1, 'bearing_vectors': [{'theta': np.radians(150), 'phi': np.radians(75), 'force': np.array([0.5, 0.3, 0.2])}]},
    {'D_i': 1, 'bearing_vectors': [{'theta': np.radians(270), 'phi': np.radians(75), 'force': np.array([0.4, 0.4, 0.2])}]},
    {'D_i': 1, 'bearing_vectors': [{'theta': np.radians(0), 'phi': np.radians(45), 'force': np.array([0.3, 0.5, 0.2])}]}
]

def spherical_to_cartesian(r, theta, phi):
    x = r * np.sin(phi) * np.cos(theta)
    y = r * np.sin(phi) * np.sin(theta)
    z = r * np.cos(phi)
    return np.array([x, y, z])

def normalize_vector(v):
    norm = np.linalg.norm(v)
    return v / norm if norm != 0 else v

def plot_node_vector(ax, pos, color, label):
    ax.scatter(*pos, c=color, s=50)
    ax.text(*pos, label, fontsize=10)
    return pos

def plot_bearing_vectors(ax, node_pos, bearing_info):
    cumulative_vector = np.zeros(3)
    D_i = bearing_info['D_i']
    for d in range(D_i):
        bv = bearing_info['bearing_vectors'][d]
        theta_i, phi_i, force_i = bv['theta'], bv['phi'], bv['force']
        
        bearing_dir = normalize_vector(spherical_to_cartesian(1, theta_i, phi_i))
        F_i = force_i[0] * np.array([1, 0, 0]) + force_i[1] * np.array([0, 1, 0]) + force_i[2] * np.array([0, 0, 1])
        
        V_i = (d + 1) * np.outer(bearing_dir, F_i).sum(axis=1)
        cumulative_vector += V_i
        
        end = node_pos + V_i
        ax.plot([node_pos[0], end[0]], [node_pos[1], end[1]], [node_pos[2], end[2]], color='cyan', linewidth=2)
        ax.text(*end, f'BV D{d+1}', fontsize=8)
    
    return cumulative_vector

def bezier_curve(points, nTimes=1000):
    nPoints = len(points)
    xPoints, yPoints, zPoints = np.array([p[0] for p in points]), np.array([p[1] for p in points]), np.array([p[2] for p in points])

    t = np.linspace(0.0, 1.0, nTimes)
    polynomial_array = np.array([comb(nPoints - 1, i) * (t ** i) * (1 - t) ** (nPoints - 1 - i) for i in range(nPoints)])
    
    xvals, yvals, zvals = np.dot(xPoints, polynomial_array), np.dot(yPoints, polynomial_array), np.dot(zPoints, polynomial_array)
    
    return xvals, yvals, zvals

def get_equidistant_points(x, y, z, num_points=10):
    # Compute the cumulative distance along the curve
    dx = np.diff(x)
    dy = np.diff(y)
    dz = np.diff(z)
    distances = np.sqrt(dx**2 + dy**2 + dz**2)
    cumulative_distances = np.concatenate(([0], np.cumsum(distances)))
    
    # Create interpolation functions
    fx = interp1d(cumulative_distances, x)
    fy = interp1d(cumulative_distances, y)
    fz = interp1d(cumulative_distances, z)
    
    # Get equidistant points
    equidistant_cumulative = np.linspace(0, cumulative_distances[-1], num_points)
    equidistant_x = fx(equidistant_cumulative)
    equidistant_y = fy(equidistant_cumulative)
    equidistant_z = fz(equidistant_cumulative)
    
    return equidistant_x, equidistant_y, equidistant_z

def create_visualization():
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    node_positions = []
    cumulative_vectors = []
    for i, node in enumerate(node_vectors):
        r, theta, phi = node
        node_pos = spherical_to_cartesian(r, theta, phi)
        label = f'Node {i+1}'
        plot_node_vector(ax, node_pos, ['red', 'green', 'blue', 'purple'][i], label)
        node_positions.append(node_pos)
        
        cumulative_vector = plot_bearing_vectors(ax, node_pos, bearing_vectors[i])
        cumulative_vectors.append(cumulative_vector)
    
    # Create Bezier curves between all pairs of nodes
    num_points = 20
    curves = []
    for i in range(len(node_positions)):
        for j in range(i+1, len(node_positions)):
            N1, N2 = node_positions[i], node_positions[j]
            C1, C2 = cumulative_vectors[i], cumulative_vectors[j]
            control_points = [N1, N1 + C1 * 0.5, N2 - C2 * 0.5, N2]
            xvals, yvals, zvals = bezier_curve(control_points)
            eq_x, eq_y, eq_z = get_equidistant_points(xvals, yvals, zvals, num_points)
            curves.append((eq_x, eq_y, eq_z))
            ax.plot(eq_x, eq_y, eq_z, color='gray', linewidth=1)
            ax.scatter(eq_x, eq_y, eq_z, color='black', s=10)

    # Create polygons
    polygons = []
    for i in range(len(curves)):
        for j in range(i+1, len(curves)):
            curve1 = curves[i]
            curve2 = curves[j]
            for k in range(num_points - 1):
                for l in range(num_points - 1):
                    polygon = [
                        (curve1[0][k], curve1[1][k], curve1[2][k]),
                        (curve1[0][k+1], curve1[1][k+1], curve1[2][k+1]),
                        (curve2[0][l+1], curve2[1][l+1], curve2[2][l+1]),
                        (curve2[0][l], curve2[1][l], curve2[2][l])
                    ]
                    polygons.append(polygon)

    # Create a Poly3DCollection
    poly3d = Poly3DCollection(polygons, alpha=0.1, facecolor='cyan', edgecolor='gray')
    ax.add_collection3d(poly3d)

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('NBV: Grid of Equidistant Points with Polygon Rendering')

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    create_visualization()