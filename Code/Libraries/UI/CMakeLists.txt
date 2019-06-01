cmake_minimum_required(VERSION 2.6)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Math/src)
include_directories(../Core/src)
include_directories(../3D/src)
include_directories(../Workbench/src)
include_directories(../Audio/src)
include_directories(../Audio/src)
include_directories(src)

set(ui_src
  src/Actions/wbactionuipushscreen.cpp
  src/Actions/wbactionuisetwidgetimage.cpp
  src/Actions/wbactionuishowhidewidget.cpp
  src/Actions/wbactionuishowyesnodialog.cpp
  src/Common/uimanagercommon.cpp
  src/Screens/uiscreen-base.cpp
  src/Screens/uiscreen-fade.cpp
  src/Screens/uiscreen-okdialog.cpp
  src/Screens/uiscreen-waitdialog.cpp
  src/Screens/uiscreen-yesnodialog.cpp
  src/Widgets/uiwidget-composite.cpp
  src/Widgets/uiwidget-frame.cpp
  src/Widgets/uiwidget-image.cpp
  src/Widgets/uiwidget-slider.cpp
  src/Widgets/uiwidget-spacer.cpp
  src/Widgets/uiwidget-text.cpp
  src/uifactory.cpp
  src/uimanager.cpp
  src/uiscreen.cpp
  src/uistack.cpp
  src/uiwidget.cpp
)

add_library(UI ${ui_src})
