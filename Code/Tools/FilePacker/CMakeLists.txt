cmake_minimum_required(VERSION 2.6)

include_directories(../../Libraries/Core/src)

link_directories(${CMAKE_SOURCE_DIR}/Libraries/Core/)

set(FilePacker_src
  src/filepacker.cpp
  src/main.cpp
)

add_executable(FilePacker ${FilePacker_src})

target_link_libraries(FilePacker Core)
