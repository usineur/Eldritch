cmake_minimum_required(VERSION 2.6)

include_directories(../../Libraries/Core/src)

link_directories(${CMAKE_SOURCE_DIR}/Libraries/Core/)

set(ConfigCompiler_src
  src/main.cpp
)

add_executable(ConfigCompiler ${ConfigCompiler_src})

target_link_libraries(ConfigCompiler Core)
