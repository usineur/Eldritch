cmake_minimum_required(VERSION 2.6)

include_directories(../Core/src)

set(math_src
  src/aabb.cpp
  src/angles.cpp
  src/box2d.cpp
  src/collisioninfo.cpp
  src/collisioninfo2d.cpp
  src/crypto.cpp
  src/cylinder.cpp
  src/ellipsoid.cpp
  src/frustum.cpp
  src/hsv.cpp
  src/line.cpp
  src/mathfunc.cpp
  src/matrix.cpp
  src/noise.cpp
  src/plane.cpp
  src/quat.cpp
  src/random.cpp
  src/ray.cpp
  src/segment.cpp
  src/segment2d.cpp
  src/sphere.cpp
  src/triangle.cpp
  src/triangle2d.cpp
  src/vector.cpp
  src/vector2.cpp
  src/vector4.cpp
)

add_library(Math ${math_src})
