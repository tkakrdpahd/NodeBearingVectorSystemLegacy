import maya.cmds as cmds
import math

def get_main_camera_data(camera_name='persp'):
    """
    지정된 메인 카메라의 모든 주요 데이터를 반환합니다.
    
    :param camera_name: 데이터를 가져올 카메라의 이름 (기본값: 'persp')
    :return: 딕셔너리 형태의 카메라 데이터
    """
    camera_data = {}
    
    # 카메라 shape 노드 이름 구성
    camera_shape = camera_name + "Shape"
    
    # 카메라 shape 노드가 존재하는지 확인
    if not cmds.objExists(camera_shape):
        cmds.error(f"카메라 shape 노드 '{camera_shape}'가 존재하지 않습니다.")
        return None
    
    # 카메라의 트랜스폼 노드(부모 노드) 가져오기
    camera_transform = cmds.listRelatives(camera_shape, parent=True)[0]
    
    # 위치 정보 가져오기
    position = cmds.getAttr(f"{camera_transform}.translate")[0]
    camera_data['position'] = position
    
    # 회전 정보 가져오기
    rotation = cmds.getAttr(f"{camera_transform}.rotate")[0]
    camera_data['rotation'] = rotation
    
    # 바라보는 방향 벡터 계산
    look_at = calculate_look_at(camera_transform)
    camera_data['look_at'] = look_at
    
    # 시야각(Field of View) 가져오기
    # 기본 카메라의 경우 horizontalFieldOfView 속성을 사용
    if cmds.attributeQuery('horizontalFieldOfView', node=camera_shape, exists=True):
        fov = cmds.getAttr(f"{camera_shape}.horizontalFieldOfView")
    elif cmds.attributeQuery('focalLength', node=camera_shape, exists=True):
        # 시야각을 focalLength로부터 계산 (근사값)
        focal_length = cmds.getAttr(f"{camera_shape}.focalLength")
        sensor_width = cmds.getAttr(f"{camera_shape}.horizontalFilmAperture") * 0.0254  # inch to meters
        fov = 2 * math.atan((sensor_width / 2) / focal_length) * (180.0 / math.pi)
    else:
        fov = None
    camera_data['field_of_view'] = fov
    
    # 클리핑 거리 가져오기
    near_clip = cmds.getAttr(f"{camera_shape}.nearClipPlane")
    far_clip = cmds.getAttr(f"{camera_shape}.farClipPlane")
    camera_data['near_clip'] = near_clip
    camera_data['far_clip'] = far_clip
    
    # 카메라의 목적지(Target) 노드가 있는지 확인 (타겟 카메라일 경우)
    # 일반적으로 `persp` 카메라는 목적지 노드가 없으므로 생략
    # 필요 시 추가 구현 가능
    
    return camera_data

def calculate_look_at(camera_transform):
    """
    카메라의 바라보는 방향 벡터를 계산합니다.
    
    :param camera_transform: 카메라의 트랜스폼 노드 이름
    :return: 바라보는 방향의 단위 벡터 (x, y, z)
    """
    # 카메라의 월드 행렬 가져오기
    matrix = cmds.xform(camera_transform, query=True, matrix=True, worldSpace=True)
    
    # Maya의 행렬은 row-major 순서이며, 아래와 같이 변환
    # Maya의 Z 축이 forward 방향
    # 행렬의 12,13,14 인덱스는 translation
    # 행렬의 8,9,10 인덱스는 Z 축 방향 벡터
    z_dir = (matrix[8], matrix[9], matrix[10])
    
    # 단위 벡터로 정규화
    length = math.sqrt(z_dir[0]**2 + z_dir[1]**2 + z_dir[2]**2)
    if length == 0:
        return (0.0, 0.0, 0.0)
    normalized = (z_dir[0]/length, z_dir[1]/length, z_dir[2]/length)
    return normalized

def print_camera_data(camera_data):
    """
    카메라 데이터를 깔끔하게 출력합니다.
    
    :param camera_data: 카메라 데이터 딕셔너리
    """
    if not camera_data:
        print("카메라 데이터를 가져오지 못했습니다.")
        return
    
    print("=== 메인 카메라 데이터 ===")
    print(f"위치 (Translate): X={camera_data['position'][0]:.3f}, Y={camera_data['position'][1]:.3f}, Z={camera_data['position'][2]:.3f}")
    print(f"회전 (Rotate): X={camera_data['rotation'][0]:.3f}, Y={camera_data['rotation'][1]:.3f}, Z={camera_data['rotation'][2]:.3f}")
    print(f"바라보는 방향 (Look At): X={camera_data['look_at'][0]:.3f}, Y={camera_data['look_at'][1]:.3f}, Z={camera_data['look_at'][2]:.3f}")
    if camera_data['field_of_view'] is not None:
        print(f"시야각 (Field of View): {camera_data['field_of_view']:.3f} degrees")
    else:
        print("시야각 (Field of View): 정보 없음")
    print(f"클리핑 거리 (Near Clip): {camera_data['near_clip']}")
    print(f"클리핑 거리 (Far Clip): {camera_data['far_clip']}")
    print("==========================")

# 사용 예제
if __name__ == "__main__":
    camera_data = get_main_camera_data()
    print_camera_data(camera_data)
