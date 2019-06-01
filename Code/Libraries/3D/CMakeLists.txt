cmake_minimum_required(VERSION 2.6)

include_directories(../Math/src)
include_directories(../Core/src)
include_directories(../GLEW/src)
include_directories(src/)

link_directories(${CMAKE_SOURCE_DIR}/Libraries/Core/)
link_directories(${CMAKE_SOURCE_DIR}/Libraries/Math/)

set(3d_src
  src/3d.cpp
  src/animation.cpp
  src/animationstate.cpp
  src/animevent.cpp
  src/animeventfactory.cpp
  src/bonearray.cpp
  src/bucket.cpp
  src/cubemapcommon.cpp
  src/dynamicmeshmanager.cpp
  src/font.cpp
  src/fontmanager.cpp
  src/GL2/gl2cubemap.cpp
  src/GL2/gl2cuberendertarget.cpp
  src/GL2/gl2indexbuffer.cpp
  src/GL2/gl2pixelshader.cpp
  src/GL2/gl2renderer.cpp
  src/GL2/gl2rendertarget.cpp
  src/GL2/gl2shaderprogram.cpp
  src/GL2/gl2texture.cpp
  src/GL2/gl2vertexbuffer.cpp
  src/GL2/gl2vertexdeclaration.cpp
  src/GL2/gl2vertexshader.cpp
  src/linebatcher.cpp
  src/material.cpp
  src/mesh.cpp
  src/meshfactory.cpp
  src/renderercommon.cpp
  src/renderstates.cpp
  src/sdpfactory.cpp
  src/SDPs/sdpbase.cpp
  src/sectorbound.cpp
  src/shaderdataprovider.cpp
  src/shadermanager.cpp
  src/surface.cpp
  src/texturecommon.cpp
  src/texturemanager.cpp
  src/tga.cpp
  src/vertexdeclarationmanager.cpp
  src/view.cpp
)

if(GLES)
  set(3d_src ${3d_src}
    src/GL2/util.cpp
  )
endif(GLES)

add_library(3D ${3d_src})

if(GLES)
  target_link_libraries(3D Core Math)
else(GLES)
  target_link_libraries(3D Core Math GL GLEW)
endif(GLES)
