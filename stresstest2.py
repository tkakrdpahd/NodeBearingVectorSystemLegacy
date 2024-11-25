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
import multiprocessing
from multiprocessing import Process, current_process
import gc  # Added import for garbage collection

# Logging configuration
if not os.path.exists('logs'):
    os.makedirs('logs')
logging.basicConfig(
    filename='logs/stress_test.log',
    level=logging.INFO,
    format='%(asctime)s %(levelname)s [%(processName)s]: %(message)s'
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

def process_iteration(i, num_nodes, num_bearings):
    """Process a single iteration in a child process."""
    process_name = current_process().name
    logging.info(f'[{process_name}] Iteration {i}: Number of Nodes: {num_nodes}')
    logging.info(f'[{process_name}] Iteration {i}: Vector Depth (Number of Bearing Vectors): {num_bearings}')
    print(f'[{process_name}] Iteration {i}: Number of Nodes: {num_nodes}')
    print(f'[{process_name}] Iteration {i}: Vector Depth (Number of Bearing Vectors): {num_bearings}')

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
    logging.info(f'[{process_name}] Iteration {i}: Total Sum: {total_sum.numpy()}')
    print(f'[{process_name}] Iteration {i}: Total Sum: {total_sum.numpy()}')

    end_time = time.time()
    duration = end_time - start_time
    logging.info(f'[{process_name}] Iteration {i}: Completed (Duration: {duration:.2f} seconds)')
    print(f'[{process_name}] Iteration {i}: Completed (Duration: {duration:.2f} seconds)')

    # Log system resource usage
    log_system_usage()

    # Convert nodes to NumPy for B-Spline generation
    nodes_np = nodes.numpy()

    # Generate B-Spline if enough nodes are present
    if num_nodes >= 4:
        try:
            spline_points = create_b_spline(nodes_np, num_points=1000, smoothing=0)
            
            # Save each B-Spline as a separate file
            spline_filename = f'logs/b_spline_iteration_{i}.npy'
            np.save(spline_filename, spline_points)
            logging.info(f'[{process_name}] Iteration {i}: B-Spline saved to {spline_filename}')
            print(f'[{process_name}] Iteration {i}: B-Spline saved to {spline_filename}')
        except Exception as e:
            logging.error(f'[{process_name}] Iteration {i}: Failed to create B-Spline. Error: {e}')
            print(f'[{process_name}] Iteration {i}: Failed to create B-Spline. Error: {e}')
    else:
        logging.warning(f'[{process_name}] Iteration {i}: Not enough nodes to create a B-Spline.')
        print(f'[{process_name}] Iteration {i}: Not enough nodes to create a B-Spline.')

    # 메모리 최적화를 위해 불필요한 변수 해제
    del r_np, theta_np, phi_np, theta_bearing_np, phi_bearing_np, force_np, nodes, bearing_vectors, bearing_directions, result, nodes_np, spline_points
    gc.collect()  # 가비지 컬렉션 호출
    logging.info(f'[{process_name}] Iteration {i}: Garbage collection completed.')
    print(f'[{process_name}] Iteration {i}: Garbage collection completed.')

def stress_test():
    """Performs the stress test by generating nodes, bearing vectors, and B-Splines using multiprocessing."""
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
    step = 100000          # Step size for increasing nodes
    num_iterations = max_nodes // step
    num_bearings = 10     # Number of bearing vectors (vector depth)

    for i in range(1, num_iterations + 1):
        num_nodes = i * step

        # Create a child process for each iteration
        p = Process(target=process_iteration, args=(i, num_nodes, num_bearings))
        p.start()
        p.join()  # Wait for the child process to complete

    logging.info('Stress test completed.')
    print('Stress test completed.')

if __name__ == '__main__':
    stress_test()
