cmake_minimum_required(VERSION 2.6)

include_directories(../../Libraries/Core/src)
include_directories(../../Libraries/Math/src)
include_directories(../../Libraries/3D/src)

link_directories(${CMAKE_SOURCE_DIR}/Libraries/Core/)
link_directories(${CMAKE_SOURCE_DIR}/Libraries/Math/)
link_directories(${CMAKE_SOURCE_DIR}/Libraries/3D/)

set(MeshCompiler_src
  src/meshcompiler.cpp
  src/main.cpp
)

add_executable(MeshCompiler ${MeshCompiler_src})

target_link_libraries(MeshCompiler Core Math 3D)
