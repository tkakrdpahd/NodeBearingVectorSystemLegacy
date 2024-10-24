import socket
import bpy
import threading
import queue
import yaml  # PyYAML 사용을 권장
import subprocess
import sys

# 스레드 간에 데이터를 전달할 큐 생성
vertex_queue = queue.Queue()

# 스레드 상태를 추적하기 위한 플래그
thread_running = False

def install(package):
    subprocess.check_call([sys.executable, "-m", "pip", "install", package])

# PyYAML 설치 (Blender 내에서 필요 시 실행)
try:
    import yaml
except ImportError:
    install('PyYAML')
    import yaml

def communicate_with_cpp_server(messages):
    HOST = "127.0.0.1"  # C++ 서버의 호스트 주소
    PORT = 8080         # C++ 서버에서 열려 있는 포트

    global thread_running
    if thread_running:
        print("이미 서버 통신 스레드가 실행 중입니다.")
        return
    thread_running = True

    def socket_thread():
        try:
            # 소켓 생성 및 서버에 연결
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect((HOST, PORT))
                for message in messages:
                    s.sendall(message.encode())  # 서버로 메시지 전송
                    response = ""
                    s.settimeout(5.0)  # 타임아웃 설정 (5초)

                    try:
                        while True:
                            part = s.recv(4096).decode()
                            if not part:
                                break
                            response += part
                    except socket.timeout:
                        print("서버로부터의 응답 수신 타임아웃.")

                    print(f"Response from C++ Server:\n{response}")

                    # 응답에서 정점 생성에 필요한 데이터를 큐에 넣기
                    vertex_data = parse_response(response)
                    if vertex_data:
                        for vertex in vertex_data:
                            vertex_queue.put(vertex)
        except socket.error as e:
            print(f"Socket error: {e}")
        finally:
            thread_running = False

    # 소켓 통신을 별도의 스레드에서 실행
    thread = threading.Thread(target=socket_thread, daemon=True)
    thread.start()

def parse_response(response):
    try:
        # PyYAML을 사용하여 YAML 응답 파싱
        data = yaml.safe_load(response)

        node_vectors = data.get('NodeVectors', [])
        liner_segments = data.get('LinerSegments', [])

        vertex_data = []

        # NodeVectors 파싱
        for node in node_vectors:
            index = node.get('index')
            cartesian = node.get('cartesian', {})
            x = float(cartesian.get('x', 0.0))
            y = float(cartesian.get('y', 0.0))
            z = float(cartesian.get('z', 0.0))
            vertex_data.append(('NodeVector', int(index), (x, y, z)))
            print(f"Parsed NodeVector - Index: {index}, x: {x}, y: {y}, z: {z}")

        # LinerSegments 내부의 SampledPoints 파싱
        for segment in liner_segments:
            sampled_points = segment.get('sampledPoints', [])
            for point in sampled_points:
                x = float(point.get('x', 0.0))
                y = float(point.get('y', 0.0))
                z = float(point.get('z', 0.0))
                vertex_data.append(('SampledPoint', None, (x, y, z)))
                print(f"Parsed SampledPoint - x: {x}, y: {y}, z: {z}")

        return vertex_data
    except Exception as e:
        print(f"Error parsing response: {e}")
        return []

def create_vertices_from_queue():
    node_vertices = []
    sampled_vertices = []
    created = False

    while not vertex_queue.empty():
        vertex_type, index, coords = vertex_queue.get()
        x, y, z = coords
        print(f"Data from queue - Type: {vertex_type}, Index: {index}, x: {x}, y: {y}, z: {z}")

        if vertex_type == 'NodeVector':
            node_vertices.append(coords)
        elif vertex_type == 'SampledPoint':
            sampled_vertices.append(coords)

    if node_vertices or sampled_vertices:
        # 기존에 생성된 메쉬 오브젝트가 있다면 삭제
        if "VertexMesh" in bpy.data.objects:
            bpy.data.objects.remove(bpy.data.objects["VertexMesh"], do_unlink=True)

        # 모든 정점을 하나의 리스트에 결합
        all_vertices = node_vertices + sampled_vertices

        # 메쉬와 오브젝트 생성
        mesh = bpy.data.meshes.new("VertexMesh")
        obj = bpy.data.objects.new("VertexMesh", mesh)

        bpy.context.collection.objects.link(obj)

        # 메쉬에 정점 추가 (엣지나 페이스는 추가하지 않음)
        mesh.from_pydata(all_vertices, [], [])
        mesh.update()

        # 포인트 시각화를 위해 쉐이딩 설정 조정
        obj.display_type = 'SOLID'  # 또는 'WIRE', 'TEXTURED' 등 필요에 따라 변경
        obj.show_in_front = True

        # 재질(Material) 추가하여 정점 색상 지정 (옵션)
        mat = bpy.data.materials.new(name="VertexMaterial")
        mat.diffuse_color = (1.0, 1.0, 1.0, 1.0)  # 흰색
        obj.data.materials.append(mat)

        # 정점 표시를 위해 파티클 시스템 사용 (옵션)
        # 이 부분은 필요에 따라 추가할 수 있습니다.

        print("All vertices have been added to the mesh.")
        created = True

    if created:
        print("Vertices have been successfully created in the mesh.")
    return 1.0  # 다시 1초 후에 실행

def blender_to_cpp_example():
    # Blender에서 모든 오브젝트 선택 해제
    bpy.ops.object.select_all(action='DESELECT')

    # 여러 메시지를 전송하여 C++ 서버에서 속성 매니저 호출
    messages = ["call_attributes_manager"]  # 예제: 메시지 1회 전송
    communicate_with_cpp_server(messages)

def run_in_thread():
    blender_to_cpp_example()

def main():
    print("Starting Blender to C++ server communication example...")
    run_in_thread()
    # 큐를 처리하기 위한 타이머 등록
    # 타이머가 중복 등록되지 않도록 기존 타이머가 있는지 확인
    if not hasattr(main, "timer_registered"):
        bpy.app.timers.register(create_vertices_from_queue)
        main.timer_registered = True
    print("Blender is communicating with the C++ server in the background.")

# Blender 스크립트 실행
if __name__ == "__main__":
    main()
