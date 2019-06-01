cmake_minimum_required(VERSION 2.6)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Math/src)
include_directories(../Core/src)
include_directories(../GLEW/src)
include_directories(../SoLoud/src)


set(audio_src
  src/audio.cpp
  src/audiosystemcommon.cpp
  src/soloudaudiosystem.cpp
  src/soloudsound.cpp
  src/soloudsoundinstance.cpp
  src/sound3dlistener.cpp
  src/soundcommon.cpp
  src/soundinstancecommon.cpp
  src/soundmanager.cpp
)

add_library(Audio ${audio_src})
