cmake_minimum_required(VERSION 2.6)

set(core_src
  src/TinyXML/tinyxml.cpp
  src/TinyXML/tinyxmlerror.cpp
  src/TinyXML/tinyxmlparser.cpp
  src/allocatorchunk.cpp
  src/allocator.cpp
  src/bufferedfilestream.cpp
  src/checksum.cpp
  src/clock.cpp
  src/configmanager.cpp
  src/configparser.cpp
  src/configvar.cpp
  src/customassert.cpp
  src/customnew.cpp
  src/datapipe.cpp
  src/display.cpp
  src/dynamicmemorystream.cpp
  src/exceptiontrace.cpp
  src/filestream.cpp
  src/fileutil.cpp
  src/hashedstring.cpp
  src/inputcontext.cpp
  src/inputsystem.cpp
  src/keyboard.cpp
  src/memorystream.cpp
  src/mouse.cpp
  src/packstream.cpp
  src/printmanager.cpp
  src/profiler.cpp
  src/profilerhook.cpp
  src/profilertable.cpp
  src/reversehash.cpp
  src/simplestring.cpp
  src/stopwatch.cpp
  src/stringmanager.cpp
  src/test.cpp
  src/timer.cpp
  src/timedate.cpp
  src/windowwrapper.cpp
  src/xinputcontroller.cpp
)

add_library(Core ${core_src})

target_link_libraries(Core z)
