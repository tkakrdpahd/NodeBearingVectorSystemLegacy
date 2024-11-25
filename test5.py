import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import time
import psutil
import platform
import random 
from matplotlib import cm

# -------------------- Adjustable Parameters --------------------

initial_nodes = 250
max_nodes = 1500
step_size = 10

node_color = 'black'
curve_color = 'lightblue'

# -------------------- Helper Functions --------------------

def fibonacci_sphere(samples=1000):
    points = []
    phi = np.pi * (3. - np.sqrt(5.))  # golden angle in radians

    for i in range(samples):
        y = 1 - (i / float(samples - 1)) * 2  # y goes from 1 to -1
        radius = np.sqrt(1 - y * y)  # radius at y

        theta = phi * i  # golden angle increment

        x = np.cos(theta) * radius
        z = np.sin(theta) * radius

        points.append((x, y, z))

    return np.array(points)

def generate_bearing_vectors(point, num_vectors):
    vectors = []
    for _ in range(num_vectors):
        # Generate one orthogonal vector
        v = np.random.randn(3)
        v -= np.dot(v, point) * point  # Make v perpendicular to point
        v /= np.linalg.norm(v)
        
        # Random depth between 0 and 3
        random_depth = random.uniform(0.5, 3)  # Adjust the range if needed
        
        # Append the scaled vector and its depth
        vectors.append((v * random_depth, random_depth))
    
    return vectors

def quadratic_bezier_curve(p0, p1, p2, num_points=20):
    t = np.linspace(0, 1, num_points)[:, np.newaxis]
    curve = (1-t)**2 * p0 + 2*(1-t)*t * p1 + t**2 * p2
    return curve

def get_system_info():
    return {
        'system': platform.system(),
        'release': platform.release(),
        'machine': platform.machine(),
        'processor': platform.processor(),
    }

def get_process_resource_usage(process, prev_cpu_times):
    current_cpu_times = process.cpu_times()
    cpu_time = sum(current_cpu_times) - sum(prev_cpu_times)
    
    # Calculate CPU usage as a percentage of elapsed time
    elapsed_time = time.time() - process.create_time()
    cpu_percent = (cpu_time / elapsed_time) * 100
    
    memory_info = process.memory_info()
    ram_used = memory_info.rss
    ram_percent = ram_used / psutil.virtual_memory().total * 100

    system_cpu_percent = psutil.cpu_percent(interval=0.1)

    return cpu_percent, system_cpu_percent, ram_percent, ram_used, current_cpu_times

# -------------------- Visualization Function --------------------

def create_visualization(num_nodes, log_file, prev_cpu_times):
    start_time = time.time()
    
    # Generate points on a sphere
    points = fibonacci_sphere(num_nodes)

    fig = plt.figure(figsize=(15, 12))
    ax = fig.add_subplot(111, projection='3d')

    # Plot points and randomly generated bearing vectors
    for point in points:
        ax.scatter(*point, c=node_color, s=20)
        
        # Generate a random number of bearing vectors (0 to 3)
        num_bearing_vectors = random.randint(0, 3)
        bearing_vectors = generate_bearing_vectors(point, num_bearing_vectors)
        
        for idx, (bearing_vector, depth) in enumerate(bearing_vectors):
            # Map vector index to a color (using a colormap, like viridis)
            color = cm.viridis(idx / 3)  # Normalize index for the colormap
            
            # Log the bearing vector depth
            log_file.write(f"Bearing vector {idx} depth for point {point}: {depth:.2f}\n")
            
            # Plot the vector with the corresponding color
            ax.quiver(*point, *bearing_vector, color=color, length=1, normalize=True)

    # Calculate and plot quadratic Bezier curves (optional, unchanged from the original code)
    for i in range(len(points)):
        for j in range(i + 1, len(points)):
            p0 = points[i]
            p2 = points[j]
            v1 = generate_bearing_vectors(p0, 1)[0][0]  # Only get the first vector
            p1 = (p0 + p2) / 2 + v1 * 0.5  # Control point
            curve = quadratic_bezier_curve(p0, p1, p2)
            ax.plot(curve[:, 0], curve[:, 1], curve[:, 2], color=curve_color, linewidth=1, alpha=0.3)

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f'Sphere Approximation with {num_nodes} Node Vectors')

    # Set the aspect of the plot to be equal
    ax.set_box_aspect((1, 1, 1))

    plt.tight_layout()
    plt.savefig(f'sphere_visualization_{num_nodes}_nodes.png')
    plt.close()

    end_time = time.time()
    execution_time = end_time - start_time
    
    process = psutil.Process()
    cpu_percent, system_cpu_percent, ram_percent, ram_used, current_cpu_times = get_process_resource_usage(process, prev_cpu_times)
    
    log_entry = (f"Nodes: {num_nodes}, Time: {execution_time:.2f}s, "
                 f"Process CPU: {cpu_percent:.2f}%, System CPU: {system_cpu_percent:.2f}%, "
                 f"Process RAM: {ram_percent:.2f}% ({ram_used / (1024**3):.2f} GB)\n")
    log_file.write(log_entry)
    log_file.flush()
    
    print(f"Visualization with {num_nodes} nodes completed in {execution_time:.2f} seconds")
    print(f"Process CPU: {cpu_percent:.2f}%, System CPU: {system_cpu_percent:.2f}%, "
          f"Process RAM: {ram_percent:.2f}% ({ram_used / (1024**3):.2f} GB)")
    
    return execution_time, current_cpu_times

# -------------------- Main Load Test Function --------------------

def run_load_test():
    log_filename = "load_test_results.txt"
    with open(log_filename, "w") as log_file:
        system_info = get_system_info()
        log_file.write(f"System Information:\n")
        for key, value in system_info.items():
            log_file.write(f"{key}: {value}\n")
        log_file.write("\n")

        process = psutil.Process()
        prev_cpu_times = process.cpu_times()

        current_nodes = initial_nodes
        while current_nodes <= max_nodes:
            print(f"Starting visualization with {current_nodes} nodes...")
            execution_time, prev_cpu_times = create_visualization(current_nodes, log_file, prev_cpu_times)
            
            if current_nodes == max_nodes:
                break
            
            current_nodes = min(current_nodes + step_size, max_nodes)

    print(f"Load test completed. Results saved in {log_filename}")

# -------------------- Run Load Test --------------------

if __name__ == "__main__":
    run_load_test()
