cmake_minimum_required(VERSION 2.6)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Math/src)
include_directories(../Core/src)
include_directories(../Workbench/src)
include_directories(src)

set(rodin_src
  src/Actions/wbactionrodinblackboardwrite.cpp
  src/BTNodes/rodinbtnodeactiondecorator.cpp
  src/BTNodes/rodinbtnodeblackboardwrite.cpp
  src/BTNodes/rodinbtnodecastresult.cpp
  src/BTNodes/rodinbtnodecomposite.cpp
  src/BTNodes/rodinbtnodecompositesingular.cpp
  src/BTNodes/rodinbtnodeconditionpe.cpp
  src/BTNodes/rodinbtnodeconditiontimeout.cpp
  src/BTNodes/rodinbtnodedecorator.cpp
  src/BTNodes/rodinbtnodelog.cpp
  src/BTNodes/rodinbtnodelookup.cpp
  src/BTNodes/rodinbtnodeloop.cpp
  src/BTNodes/rodinbtnodenull.cpp
  src/BTNodes/rodinbtnodeparallel.cpp
  src/BTNodes/rodinbtnodeplayactions.cpp
  src/BTNodes/rodinbtnodeselector.cpp
  src/BTNodes/rodinbtnodesendevent.cpp
  src/BTNodes/rodinbtnodesequence.cpp
  src/BTNodes/rodinbtnodesleep.cpp
  src/BTNodes/rodinbtnodetimeout.cpp
  src/BTNodes/rodinbtnodeuseresource.cpp
  src/BTNodes/rodinbtnodeusestatmod.cpp
  src/BTNodes/rodinbtnodewait.cpp
  src/BTNodes/rodinbtnodewaitforevent.cpp
  src/Components/wbcomprodinbehaviortree.cpp
  src/Components/wbcomprodinblackboard.cpp
  src/Components/wbcomprodinknowledge.cpp
  src/Components/wbcomprodinresourcemap.cpp
  src/PEs/wbperodinblackboardget.cpp
  src/PEs/wbperodingetknowledge.cpp
  src/PEs/wbperodingetlastknownlocation.cpp
  src/PEs/wbperodingetlastknowntime.cpp
  src/rodinbtnode.cpp
  src/rodinbtnodefactory.cpp
)

add_library(Rodin ${rodin_src})
