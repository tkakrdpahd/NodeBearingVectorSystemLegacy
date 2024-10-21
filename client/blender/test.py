import socket
import bpy
import re

def communicate_with_cpp_server(messages):
    HOST = "127.0.0.1"  # C++ 서버의 호스트 주소
    PORT = 8080         # C++ 서버에서 열려 있는 포트

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            # C++ 서버에 연결
            s.connect((HOST, PORT))
            for message in messages:
                s.sendall(message.encode())  # 서버로 메시지를 전송
                response = s.recv(4096).decode()  # 서버에서 응답 수신 (버퍼 크기 증가)
                print(f"Response from C++ Server:\n{response}")

                # Cartesian 좌표값을 파싱하여 박스 생성
                create_boxes_from_response(response)
        except socket.error as e:
            print(f"Socket error: {e}")

def create_boxes_from_response(response):
    # Cartesian 좌표를 파싱하기 위한 정규 표현식
    cartesian_pattern = r"Stored Node \(Cartesian\): Index = (\d+), x = ([\d\.\-e]+), y = ([\d\.\-e]+), z = ([\d\.\-e]+)"
    matches = re.findall(cartesian_pattern, response)

    for match in matches:
        index, x, y, z = match
        # 문자열을 float로 변환
        x = float(x)
        y = float(y)
        z = float(z)

        # Blender에서 Cartesian 좌표에 박스 생성
        box_name = f"box_{index}"
        bpy.ops.mesh.primitive_cube_add(size=2, location=(x, y, z))  # 박스 생성 및 위치 지정
        cube = bpy.context.object
        cube.name = box_name  # 박스 이름 지정

# Blender에서 사용하는 기능 예제
def blender_to_cpp_example():
    # 블렌더 씬에서 모든 오브젝트 선택 후 삭제
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)

    # 여러 메시지를 전송하여 C++ 서버에서 속성 매니저 호출
    messages = ["call_attributes_manager"]  # 예제: 메시지 1회 전송
    communicate_with_cpp_server(messages)

# 메인 함수
def main():
    print("Starting Blender to C++ server communication example...")
    blender_to_cpp_example()
    print("Completed Blender to C++ server communication.")

# 메인 함수 호출
if __name__ == "__main__":
    main()
