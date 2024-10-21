# 최소 CMake 버전 설정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 언어 설정
project(TheNewDream VERSION 1.0 LANGUAGES CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 실행 파일에 포함될 소스 파일 지정
set(SOURCES
    main.cpp
    module/CoordinateConverter.cpp
    module/NodeVector.cpp
    module/BearingVector.cpp
    module/LinerSegment.cpp
    module/SurfaceSegment.cpp
    module/AttributesManager.cpp
    module/SocketServer.cpp
    module/YamlConvertor.cpp
    module/Draw.cpp
)

# 실행 파일 생성
add_executable(NodeBearingVectorSystem ${SOURCES})

# 포함 디렉토리 설정 (헤더 파일 위치)
target_include_directories(NodeBearingVectorSystem PRIVATE
    ${PROJECT_SOURCE_DIR}/module
)

# Find and link OpenGL
find_package(OpenGL REQUIRED)

# Link libraries (OpenGL and GLUT)
target_link_libraries(NodeBearingVectorSystem PRIVATE
    OpenGL::GL
    "-framework GLUT"
    "-framework OpenGL"
)

# Suppress OpenGL deprecation warnings
target_compile_definitions(NodeBearingVectorSystem PRIVATE GL_SILENCE_DEPRECATION)
