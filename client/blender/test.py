import socket
import bpy
import re
import threading
import queue

# 스레드 간에 데이터를 전달할 큐 생성
box_queue = queue.Queue()

# 스레드 상태를 추적하기 위한 플래그
thread_running = False

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

                    # 응답에서 박스 생성에 필요한 데이터를 큐에 넣기
                    box_data = parse_response(response)
                    if box_data:
                        for box in box_data:
                            box_queue.put(box)
        except socket.error as e:
            print(f"Socket error: {e}")
        finally:
            thread_running = False

    # 소켓 통신을 별도의 스레드에서 실행
    thread = threading.Thread(target=socket_thread, daemon=True)
    thread.start()

def parse_response(response):
    try:
        # NodeVectors 섹션 추출
        node_vectors_match = re.search(r'NodeVectors:\s*(.*?)\s*(BearingVectors|LinerSegments|$)', response, re.DOTALL)
        if not node_vectors_match:
            print("NodeVectors 섹션을 찾을 수 없습니다.")
            return []

        node_vectors_str = node_vectors_match.group(1)
        print(f"NodeVectors 섹션:\n{node_vectors_str}")  # 디버깅 출력

        # 각 노드 벡터 추출을 위한 개선된 정규 표현식
        # 이 패턴은 각 노드의 index, x, y, z 값을 개별적으로 추출합니다.
        node_pattern = r'-\s*index:\s*(\d+).*?cartesian:\s*x:\s*([-.\deE]+).*?y:\s*([-.\deE]+).*?z:\s*([-.\deE]+)'
        matches = re.findall(node_pattern, node_vectors_str, re.DOTALL)

        print(f"찾은 노드 수: {len(matches)}")  # 디버깅 출력
        print(f"매칭된 노드들: {matches}")      # 디버깅 출력

        box_data = []
        for match in matches:
            index, x, y, z = match
            print(f"파싱된 노드 - Index: {index}, x: {x}, y: {y}, z: {z}")  # 디버깅 출력
            # 문자열을 float으로 변환
            try:
                x = float(x)
                y = float(y)
                z = float(z)
            except ValueError as ve:
                print(f"좌표 변환 오류: {ve}")
                continue

            box_data.append((index, x, y, z))

        return box_data
    except Exception as e:
        print(f"파싱 중 에러 발생: {e}")
        return []

def create_boxes_from_queue():
    created = False
    while not box_queue.empty():
        index, x, y, z = box_queue.get()
        print(f"큐에서 가져온 데이터 - Index: {index}, x: {x}, y: {y}, z: {z}")  # 디버깅 출력
        box_name = f"Box_{index}"
        bpy.ops.mesh.primitive_cube_add(size=1, location=(x, y, z))
        obj = bpy.context.active_object
        obj.name = box_name
        print(f"박스 생성: {box_name} 위치=({x}, {y}, {z})")
        created = True
    if created:
        print("모든 박스 생성 완료.")
    return 1.0  # 다시 1초 후에 실행

def blender_to_cpp_example():
    # Blender에서 모든 오브젝트 선택 (필요한 경우)
    bpy.ops.object.select_all(action='SELECT')

    # 여러 메시지를 전송하여 C++ 서버에서 속성 매니저 호출
    messages = ["call_attributes_manager"]  # 예제: 메시지 1회 전송
    communicate_with_cpp_server(messages)

def run_in_thread():
    blender_to_cpp_example()

def main():
    print("Blender에서 C++ 서버 통신 예제 시작...")
    run_in_thread()
    # 큐를 처리하기 위한 타이머 등록
    # 타이머가 중복 등록되지 않도록 기존 타이머가 있는지 확인
    if not hasattr(main, "timer_registered"):
        bpy.app.timers.register(create_boxes_from_queue)
        main.timer_registered = True
    print("Blender에서 C++ 서버 통신을 백그라운드 스레드에서 실행 중입니다.")

# Blender 스크립트 실행
if __name__ == "__main__":
    main()
