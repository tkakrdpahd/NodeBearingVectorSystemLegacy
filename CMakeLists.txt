# Minimum CMake version
cmake_minimum_required(VERSION 3.12)

# Project setup
project(TheNewDream VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 자동으로 소스 파일 검색 (CMake 3.12 이상에서 ** 사용 가능)
file(GLOB_RECURSE SOURCES "main.cpp" "module/*.cpp" "module/**/*.cpp")

# Source files
# set(SOURCES
#     main.cpp
#     module/CoordinateConverter.cpp
#     module/NodeVector.cpp
#     module/BearingVector.cpp
#     module/LinerSegment.cpp
#     module/SurfaceSegment.cpp
#     module/AttributesManager.cpp
#     module/SocketServer.cpp
#     module/YamlConverter.cpp
#     module/Draw.cpp
# )

# Add executable
add_executable(NodeBearingVectorSystem ${SOURCES})

# Include directories
target_include_directories(NodeBearingVectorSystem PRIVATE
    ${PROJECT_SOURCE_DIR}/module
    ${PROJECT_SOURCE_DIR}/module/vectors
    ${PROJECT_SOURCE_DIR}/module/segment
    ${PROJECT_SOURCE_DIR}/module/operator
    /opt/homebrew/include # Manually add yaml-cpp include path here
)

# Find and link OpenGL
find_package(OpenGL REQUIRED)

# Link OpenGL and GLUT
target_link_libraries(NodeBearingVectorSystem PRIVATE
    OpenGL::GL
    "-framework GLUT"
    "-framework OpenGL"
)

# Suppress OpenGL deprecation warnings
target_compile_definitions(NodeBearingVectorSystem PRIVATE GL_SILENCE_DEPRECATION)

# Manually link yaml-cpp library
target_link_libraries(NodeBearingVectorSystem PRIVATE
    /opt/homebrew/lib/libyaml-cpp.dylib
)
