cmake_minimum_required(VERSION 2.8)

project(Eldritch)
include (FindPkgConfig)

option(ALSA "Set ON to enable ALSA backend" OFF)
option(NULL "Set ON to enable Null backend" OFF)
option(OPENAL "Set ON to enable OpenAL backend" OFF)
option(OSS "Set ON to enable OSS backend" OFF)
option(PORTAUDIO "Set ON to enable PortAudio backend" OFF)
option(SDL2_STATIC "Default backend" ON)

option(GLES "Set to ON if using GLES2 instead of desktop OpenGL" ${GLES})

option(DEBUG_ENABLED "Build a DEBUG version (if not, then it's a RELEASE version)" OFF)
option(FINAL_ENABLED "Build a FINAL version" OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(DEBUG_ENABLED)
  add_definitions(-D_DEBUG)
endif()
if(FINAL_ENABLED)
  add_definitions(-D_FINAL)
endif()
if(ALSA)
  add_definitions(-DWITH_ALSA)
endif(ALSA)
if(NULL)
  add_definitions(-DWITH_NULL)
endif(NULL)
if(SDL2_STATIC)
  add_definitions(-DWITH_SDL2_STATIC)
endif(SDL2_STATIC)
if(OPENAL)
  add_definitions(-DWITH_OPENAL)
endif(OPENAL)
if(OSS)
  add_definitions(-DWITH_OSS)
endif(OSS)
if(PORTAUDIO)
  add_definitions(-DWITH_PORTAUDIO)
endif(PORTAUDIO)
if(GLES)
  add_definitions(-DHAVE_GLES)
endif(GLES)

if(SWITCH_LIBNX)
  set(CMAKE_CXX_FLAGS "-O3 -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE -ffunction-sections -fdata-sections -ftls-model=local-exec")
  include_directories($ENV{DEVKITPRO}/libnx/include $ENV{DEVKITPRO}/portlibs/switch/include)
  add_definitions(-D__SWITCH__)
  add_custom_target(${CMAKE_PROJECT_NAME}.nro
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMAND nacptool --create "Eldritch" "usineur" "406" ${CMAKE_PROJECT_NAME}.nacp
    COMMAND elf2nro Projects/Eld/${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.nro
        --icon=${CMAKE_SOURCE_DIR}/icon.jpg --nacp=${CMAKE_PROJECT_NAME}.nacp
  )
  add_custom_target(nxlink
    DEPENDS ${CMAKE_PROJECT_NAME}.nro
    COMMAND nxlink -a $(SWITCHIP) ${CMAKE_PROJECT_NAME}.nro -s -p ${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}.nro
  )
endif(SWITCH_LIBNX)

add_subdirectory(Libraries/Core/)
add_subdirectory(Libraries/Math/)
add_subdirectory(Libraries/3D/)
add_subdirectory(Libraries/Audio/)
add_subdirectory(Libraries/SoLoud/)
add_subdirectory(Libraries/Workbench/)
add_subdirectory(Libraries/Rodin/)
add_subdirectory(Libraries/UI/)
add_subdirectory(Libraries/Framework/)
add_subdirectory(Projects/Eld/)
add_subdirectory(Tools/Checksum/)
add_subdirectory(Tools/ConfigCompiler/)
add_subdirectory(Tools/FilePacker/)
add_subdirectory(Tools/MeshCompiler/)
