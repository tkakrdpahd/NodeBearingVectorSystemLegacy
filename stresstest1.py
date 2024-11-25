import tensorflow as tf
import numpy as np
import psutil
import logging
import time
from datetime import datetime
import os
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import scipy.interpolate as si  # Added import for B-Splines

# Logging configuration
if not os.path.exists('logs'):
    os.makedirs('logs')
logging.basicConfig(
    filename='logs/stress_test.log',
    level=logging.INFO,
    format='%(asctime)s %(levelname)s: %(message)s'
)

def log_system_usage():
    """Logs the current CPU and memory usage."""
    process = psutil.Process(os.getpid())
    cpu_usage = psutil.cpu_percent(interval=1)
    memory_info = process.memory_info()
    memory_usage = memory_info.rss / (1024 ** 3)  # Memory usage in GB
    memory_percent = process.memory_percent()     # Memory usage percentage

    logging.info(f'CPU Usage: {cpu_usage}%')
    logging.info(f'Process Memory Usage: {memory_usage:.2f}GB ({memory_percent:.2f}%)')
    print(f'CPU Usage: {cpu_usage}%')
    print(f'Process Memory Usage: {memory_usage:.2f}GB ({memory_percent:.2f}%)')

def create_node_vectors(num_nodes):
    """Generate node vectors in spherical coordinates."""
    r = np.random.uniform(0.5, 1.5, num_nodes).astype(np.float32)
    theta = np.random.uniform(0, 2 * np.pi, num_nodes).astype(np.float32)
    phi = np.random.uniform(0, np.pi, num_nodes).astype(np.float32)
    return r, theta, phi

def create_bearing_vectors(num_nodes, num_bearings=2):
    """Generate a fixed number of bearing vectors for each node."""
    theta_bearing = np.random.uniform(0, 2 * np.pi, size=(num_nodes, num_bearings)).astype(np.float32)
    phi_bearing = np.random.uniform(0, np.pi, size=(num_nodes, num_bearings)).astype(np.float32)
    force = np.random.uniform(0, 1, size=(num_nodes, num_bearings, 3)).astype(np.float32)
    force = force / np.linalg.norm(force, axis=2, keepdims=True)  # Normalize
    return theta_bearing, phi_bearing, force

def spherical_to_cartesian(r, theta, phi):
    """Convert spherical coordinates to Cartesian coordinates using TensorFlow."""
    x = r * tf.sin(phi) * tf.cos(theta)
    y = r * tf.sin(phi) * tf.sin(theta)
    z = r * tf.cos(phi)
    return tf.stack([x, y, z], axis=1)

def create_b_spline(nodes, num_points=1000, smoothing=0):
    """
    Generate B-Spline curve from node positions.

    Args:
        nodes (np.ndarray): Array of node positions with shape (num_nodes, 3).
        num_points (int): Number of points to generate on the B-Spline curve.
        smoothing (float): Smoothing factor for the spline. 0 means interpolation.

    Returns:
        np.ndarray: Array of points on the B-Spline curve with shape (num_points, 3).
    """
    if len(nodes) < 4:
        raise ValueError("At least 4 nodes are required to create a cubic B-Spline.")

    # Transpose nodes to separate x, y, z
    x, y, z = nodes[:, 0], nodes[:, 1], nodes[:, 2]

    # Parameterize the spline
    tck, u = si.splprep([x, y, z], s=smoothing)

    # Generate new points on the spline
    u_fine = np.linspace(0, 1, num_points)
    x_fine, y_fine, z_fine = si.splev(u_fine, tck)

    return np.vstack((x_fine, y_fine, z_fine)).T

def stress_test():
    """Performs the stress test by generating nodes, bearing vectors, and B-Splines."""
    # Check if TensorFlow is using the GPU
    physical_devices = tf.config.list_physical_devices('GPU')
    if len(physical_devices) > 0:
        logging.info('Using GPU with TensorFlow-metal.')
        print('Using GPU with TensorFlow-metal.')
    else:
        logging.info('No GPU found. Running on CPU.')
        print('No GPU found. Running on CPU.')

    # Start stress test
    logging.info('Starting stress test...')
    print('Starting stress test...')

    max_nodes = 10000000  # Maximum number of nodes
    step = 100000        # Step size for increasing nodes
    num_iterations = max_nodes // step

    # Variables to store nodes and bearing vectors from each iteration
    all_spline_points = []  # To store all B-Spline points if cumulative plotting is desired
    num_bearings = 10  # Number of bearing vectors (vector depth)

    for i in range(1, num_iterations + 1):
        num_nodes = i * step
        logging.info(f'Iteration {i}: Number of Nodes: {num_nodes}')
        logging.info(f'Iteration {i}: Vector Depth (Number of Bearing Vectors): {num_bearings}')
        print(f'Iteration {i}: Number of Nodes: {num_nodes}')
        print(f'Iteration {i}: Vector Depth (Number of Bearing Vectors): {num_bearings}')

        start_time = time.time()

        # Generate node vectors
        r_np, theta_np, phi_np = create_node_vectors(num_nodes)
        r = tf.constant(r_np)
        theta = tf.constant(theta_np)
        phi = tf.constant(phi_np)

        # Generate bearing vectors (num_bearings per node)
        theta_bearing_np, phi_bearing_np, force_np = create_bearing_vectors(num_nodes, num_bearings=num_bearings)
        theta_bearing = tf.constant(theta_bearing_np)
        phi_bearing = tf.constant(phi_bearing_np)
        force = tf.constant(force_np)

        # Compute node coordinates using TensorFlow (parallel processing)
        nodes = spherical_to_cartesian(r, theta, phi)

        # Compute bearing vectors (parallel processing)
        def compute_bearing_vectors(theta_b, phi_b, force_b):
            x_b = tf.sin(phi_b) * tf.cos(theta_b)
            y_b = tf.sin(phi_b) * tf.sin(theta_b)
            z_b = tf.cos(phi_b)
            direction = tf.stack([x_b, y_b, z_b], axis=-1) * force_b  # (num_nodes, num_bearings, 3)
            return direction  # Return individual bearing vectors

        # Compute bearing vectors for each node
        bearing_vectors = compute_bearing_vectors(theta_bearing, phi_bearing, force)

        # Sum bearing vectors for each node
        bearing_directions = tf.reduce_sum(bearing_vectors, axis=1)  # (num_nodes, 3)

        # Perform arbitrary computation using node positions and bearing vectors
        result = nodes + bearing_directions

        # Compute arbitrary result (e.g., sum of all elements)
        total_sum = tf.reduce_sum(result)
        logging.info(f'Iteration {i}: Total Sum: {total_sum.numpy()}')
        print(f'Iteration {i}: Total Sum: {total_sum.numpy()}')

        end_time = time.time()
        duration = end_time - start_time
        logging.info(f'Iteration {i}: Completed (Duration: {duration:.2f} seconds)')
        print(f'Iteration {i}: Completed (Duration: {duration:.2f} seconds)')

        # Log system resource usage
        log_system_usage()

        # Convert nodes to NumPy for B-Spline generation
        nodes_np = nodes.numpy()

        # Generate B-Spline if enough nodes are present
        if num_nodes >= 4:
            try:
                spline_points = create_b_spline(nodes_np, num_points=1000, smoothing=0)
                all_spline_points.append(spline_points)  # Store for cumulative plotting if desired

                # Optionally, save each B-Spline as a separate file
                spline_filename = f'logs/b_spline_iteration_{i}.npy'
                np.save(spline_filename, spline_points)
                logging.info(f'Iteration {i}: B-Spline saved to {spline_filename}')
                print(f'Iteration {i}: B-Spline saved to {spline_filename}')
            except Exception as e:
                logging.error(f'Iteration {i}: Failed to create B-Spline. Error: {e}')
                print(f'Iteration {i}: Failed to create B-Spline. Error: {e}')
        else:
            logging.warning(f'Iteration {i}: Not enough nodes to create a B-Spline.')
            print(f'Iteration {i}: Not enough nodes to create a B-Spline.')

        # Optionally, visualize each B-Spline in separate plots
        visualize_each_spline = False  # Set to True to generate separate plots for each iteration
        if visualize_each_spline and num_nodes >= 4:
            visualize_b_spline(
                nodes=nodes_np,
                bearing_vectors=bearing_vectors.numpy(),
                num_bearings=num_bearings,
                spline_points=spline_points,
                iteration=i
            )

    logging.info('Stress test completed.')
    print('Stress test completed.')

    # Optionally, visualize all B-Splines together
    visualize_all_splines = True  # Set to True to plot all B-Splines in one figure
    if visualize_all_splines and all_spline_points:
        visualize_all_b_splines(all_spline_points)
        print('All B-Splines visualized together.')

def visualize_b_spline(nodes, bearing_vectors, num_bearings, spline_points, iteration):
    """Visualize nodes, bearing vectors, and a B-Spline curve for a specific iteration."""
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    # Plot nodes
    ax.scatter(nodes[:, 0], nodes[:, 1], nodes[:, 2], c='blue', s=20, label='Nodes')

    # Set colors based on vector depth
    color_list = ['red', 'green', 'orange', 'purple', 'cyan', 'magenta', 'yellow', 'black']
    if num_bearings > len(color_list):
        # Use a color map if num_bearings exceeds the color list
        cmap = plt.get_cmap('hsv', num_bearings)
        colors = [cmap(i) for i in range(num_bearings)]
    else:
        colors = color_list[:num_bearings]

    # Plot bearing vectors
    for i in range(len(nodes)):
        start = nodes[i]
        for d in range(num_bearings):
            vec = bearing_vectors[i, d]
            end = start + vec
            ax.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]],
                    color=colors[d], linewidth=1)

    # Plot B-Spline curve
    ax.plot(spline_points[:, 0], spline_points[:, 1], spline_points[:, 2],
            color='black', linewidth=2, label='B-Spline Curve')

    # Create legend
    legend_elements = [
        plt.Line2D([0], [0], marker='o', color='w', label='Nodes',
                   markerfacecolor='blue', markersize=10)
    ]
    for d in range(num_bearings):
        legend_elements.append(
            plt.Line2D([0], [0], color=colors[d], lw=2, label=f'Bearing Vector {d+1}')
        )
    legend_elements.append(
        plt.Line2D([0], [0], color='black', lw=2, label='B-Spline Curve')
    )
    ax.legend(handles=legend_elements)

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f'Iteration {iteration}: Nodes, Bearing Vectors, and B-Spline Curve Visualization')

    plt.tight_layout()
    plot_filename = f'nodes_bearings_spline_iteration_{iteration}.png'
    plt.savefig(plot_filename)
    plt.close(fig)  # Close the figure to free memory
    logging.info(f'Iteration {iteration}: Visualization saved to {plot_filename}')
    print(f'Iteration {iteration}: Visualization saved to {plot_filename}')

def visualize_all_b_splines(all_spline_points):
    """Visualize all B-Spline curves together."""
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    # Plot all B-Splines
    for idx, spline in enumerate(all_spline_points, start=1):
        ax.plot(spline[:, 0], spline[:, 1], spline[:, 2],
                label=f'B-Spline Iteration {idx}')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('All B-Spline Curves Across Iterations')
    ax.legend()

    plt.tight_layout()
    plot_filename = 'all_b_splines_visualization.png'
    plt.savefig(plot_filename)
    plt.close(fig)  # Close the figure to free memory
    logging.info(f'All B-Splines visualized and saved to {plot_filename}')
    print(f'All B-Splines visualized and saved to {plot_filename}')

if __name__ == '__main__':
    stress_test()
