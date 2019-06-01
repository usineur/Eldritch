cmake_minimum_required(VERSION 2.6)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Math/src)
include_directories(../Core/src)
include_directories(../3D/src)
include_directories(../Workbench/src)
include_directories(../Audio/src)
include_directories(../Audio/src)
include_directories(src)

set(soloud_src
  src/core/soloud_fft_lut.cpp
  src/core/soloud_core_basicops.cpp
  src/core/soloud_thread.cpp
  src/core/soloud_fader.cpp
  src/core/soloud_filter.cpp
  src/core/soloud.cpp
  src/core/soloud_core_voicegroup.cpp
  src/core/soloud_core_getters.cpp
  src/core/soloud_audiosource.cpp
  src/core/soloud_fft.cpp
  src/core/soloud_bus.cpp
  src/core/soloud_core_faderops.cpp
  src/core/soloud_core_3d.cpp
  src/core/soloud_core_setters.cpp
  src/core/soloud_file.cpp
  src/core/soloud_core_filterops.cpp
  src/core/soloud_core_voiceops.cpp
  src/audiosource/wav/stb_vorbis.c
  src/audiosource/wav/soloud_wav.cpp
  src/audiosource/wav/soloud_wavstream.cpp
  src/filter/soloud_dcremovalfilter.cpp
  src/filter/soloud_lofifilter.cpp
  src/filter/soloud_biquadresonantfilter.cpp
  src/filter/soloud_echofilter.cpp
  src/filter/soloud_fftfilter.cpp
  src/filter/soloud_bassboostfilter.cpp
  src/filter/soloud_flangerfilter.cpp
)

if(ALSA)
  set(soloud_src ${soloud_src}
    src/backend/alsa/soloud_alsa.cpp
  )
  set(soloud_link ${soloud_link} asound)
endif(ALSA)

if(NULL)
  set(soloud_src ${soloud_src}
    src/backend/null/soloud_null.cpp
  )
endif(NULL)

if(SDL2_STATIC)
  set(soloud_src ${soloud_src}
    src/backend/sdl2_static/soloud_sdl2_static.cpp
  )
endif(SDL2_STATIC)

if(OPENAL)
  set(soloud_src ${soloud_src}
    src/backend/openal/soloud_openal_dll.c
    src/backend/openal/soloud_openal.cpp
  )
  set(soloud_link ${soloud_link} openal dl)
endif(OPENAL)

if(OSS)
  set(soloud_src ${soloud_src}
    src/backend/oss/soloud_oss.cpp
  )
endif(OSS)

if(PORTAUDIO)
  set(soloud_src ${soloud_src}
    src/backend/portaudio/soloud_portaudio.cpp
    src/backend/portaudio/soloud_portaudio_dll.c
  )
  set(soloud_link ${soloud_link} portaudio dl)
endif(PORTAUDIO)

add_library(SoLoud ${soloud_src})

target_link_libraries(SoLoud ${soloud_link})
